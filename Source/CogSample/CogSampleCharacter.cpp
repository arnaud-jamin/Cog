#include "CogSampleCharacter.h"

#include "Camera/CameraComponent.h"
#include "CogCommon.h"
#include "CogSampleAbilitySystemComponent.h"
#include "CogSampleAttributeSet_Health.h"
#include "CogSampleAttributeSet_Misc.h"
#include "CogSampleCharacterMovementComponent.h"
#include "CogSampleFunctionLibrary_Tag.h"
#include "CogSampleFunctionLibrary_Team.h"
#include "CogSampleGameplayAbility.h"
#include "CogSampleLogCategories.h"
#include "CogSampleRootMotionParams.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/CollisionProfile.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/CheatManagerDefines.h"
#include "GameFramework/Controller.h"
#include "GameFramework/RootMotionSource.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

#if ENABLE_COG
#include "CogAbilityReplicator.h"
#include "CogDebugMetric.h"
#include "CogDebugPlot.h"
#endif //ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
ACogSampleCharacter::ACogSampleCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UCogSampleCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    AbilitySystem = CreateDefaultSubobject<UCogSampleAbilitySystemComponent>(TEXT("AbilitySystem"));
    AbilitySystem->SetIsReplicated(true);
    AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Full);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;

    Params.Condition = COND_None;
    DOREPLIFETIME_WITH_PARAMS_FAST(ACogSampleCharacter, Team, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ACogSampleCharacter, ProgressionLevel, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ACogSampleCharacter, Scale, Params);

    Params.Condition = COND_OwnerOnly;
    DOREPLIFETIME_WITH_PARAMS_FAST(ACogSampleCharacter, ActiveAbilityHandles, Params);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    InitializeAbilitySystem();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

    if (GetWorld()->GetNetMode() == NM_DedicatedServer)
    {
        GetMesh()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    }
    

    TryFinishInitialize();
    RefreshServerAnimTickOption();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::MarkComponentsAsPendingKill()
{
    Super::MarkComponentsAsPendingKill();

    UnregisterFromAbilitySystemEvents();
}

//--------------------------------------------------------------------------------------------------------------------------
UAbilitySystemComponent* ACogSampleCharacter::GetAbilitySystemComponent() const 
{
    return AbilitySystem; 
}


//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::PossessedBy(AController* NewController)
{
    COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Verbose, this, TEXT("Controller:%s"), *GetNameSafe(NewController));

    if (InitialController == nullptr)
    {
        InitialController = NewController;
    }

    Super::PossessedBy(NewController);

    if (bIsInitialized == false)
    {
        InitializeAbilitySystem();
    }
    else
    {
        //-------------------------------------------------------------------------------------------
        // When possessing a NPC, we need to refresh the ability system actor info, so it knows about
        // the new controller to be able to activate abilities.
        //-------------------------------------------------------------------------------------------
        AbilitySystem->InitAbilityActorInfo(this, this);

        //-------------------------------------------------------------------------------------------
        // We might be possessed when in a middle of an ability. Currently we prefer to cancel it.
        //-------------------------------------------------------------------------------------------
        AbilitySystem->CancelAllAbilities();

        if (UCogSampleCharacterMovementComponent* MovementComp = Cast<UCogSampleCharacterMovementComponent>(GetMovementComponent()))
        {
            MovementComp->PossessedBy(NewController);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::UnPossessed()
{
    COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Verbose, this, TEXT(""));

    if (UCogSampleCharacterMovementComponent* MovementComp = Cast<UCogSampleCharacterMovementComponent>(GetMovementComponent()))
    {
        MovementComp->UnPossessed();
    }

    Super::UnPossessed();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::AcknowledgePossession(AController* NewController)
{
    COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Verbose, this, TEXT("Controller:%s - NewController:%s"), *GetNameSafe(NewController), *GetNameSafe(Controller));

    //-------------------------------------------------------------------------------------------
    // Set the controller otherwise when the player possesses a NPC, he would not be able to cast
    // any ability. The ability system component needs to know the controller and therefore it
    // needs to be set before calling InitAbilityActorInfo.
    // See FGameplayAbilityActorInfo::InitFromActor
    //-------------------------------------------------------------------------------------------
    Controller = NewController;

    if (AbilitySystem != nullptr)
    {
        AbilitySystem->InitAbilityActorInfo(this, this);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::AcknowledgeUnpossession()
{
    COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Verbose, this, TEXT("OldController:%s"), *GetNameSafe(Controller));

    Controller = nullptr;

    AbilitySystem->InitAbilityActorInfo(this, this);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::InitializeAbilitySystem()
{   
    if (bIsAbilitySystemInitialized)
    {
        return;
    }

    AbilitySystem->InitAbilityActorInfo(this, this);

    if (AbilitySystem->IsOwnerActorAuthoritative())
    {
        for (const TSubclassOf<UAttributeSet>& AttributeSet : AttributeSets)
        {
            if (IsValid(AttributeSet) == false)
            {
                continue;
            }

            UAttributeSet* AttributeSetInstance = NewObject<UAttributeSet>(this, AttributeSet);
            AbilitySystem->AddAttributeSetSubobject(AttributeSetInstance);
        }

        for (TSubclassOf<UGameplayEffect> Effect : Effects)
        {
            AbilitySystem->BP_ApplyGameplayEffectToSelf(Effect, 1, AbilitySystem->MakeEffectContext());
        }

        for (FPassiveAbilityInfo& AbilityInfo : PassiveAbilities)
        {
            const FGameplayAbilitySpec Spec(AbilityInfo.Ability, 1, INDEX_NONE, this);
            AbilitySystem->GiveAbility(Spec);
        }

        for (int32 i = 0; i < ActiveAbilities.Num(); ++i)
        {
            const FActiveAbilityInfo& AbilityInfo = ActiveAbilities[i];
            const FGameplayAbilitySpec Spec(AbilityInfo.Ability, 1, INDEX_NONE, this);
            const FGameplayAbilitySpecHandle Handle = AbilitySystem->GiveAbility(Spec);
            ActiveAbilityHandles.Add(Handle);

            const FGameplayAbilitySpec* AddedSpec = AbilitySystem->FindAbilitySpecFromHandle(Handle);
            if (AddedSpec == nullptr)
            {
                continue;
            }

            UCogSampleGameplayAbility* Ability = Cast<UCogSampleGameplayAbility>(AddedSpec->GetPrimaryInstance());
            if (Ability == nullptr)
            {
                continue;
            }
            
            if (UCogSampleFunctionLibrary_Tag::ActiveAbilityCooldownTags.IsValidIndex(i) == false)
            {
                continue;
            }
            
            Ability->SetCooldownTag(UCogSampleFunctionLibrary_Tag::ActiveAbilityCooldownTags[i]);
        }

        UpdateActiveAbilitySlots();

        MARK_PROPERTY_DIRTY_FROM_NAME(ACogSampleCharacter, ActiveAbilityHandles, this);
    }

    bIsAbilitySystemInitialized = true;

    AbilitySystem->AddLooseGameplayTags(InitialTags);

    TryFinishInitialize();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::RegisterToAbilitySystemEvents()
{
    GameplayTagPropertyMap.Initialize(this, AbilitySystem);

    //----------------------------------------
    // Register to Tag change events
    //----------------------------------------
    GhostTagDelegateHandle = AbilitySystem->RegisterGameplayTagEvent(Tag_Status_Ghost, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ACogSampleCharacter::OnGhostTagNewOrRemoved);

    //----------------------------------------
    // Register to Attribute change events
    //----------------------------------------
    if (const UCogSampleAttributeSet_Misc* MiscAttributeSet = Cast<UCogSampleAttributeSet_Misc>(AbilitySystem->GetAttributeSet(UCogSampleAttributeSet_Misc::StaticClass())))
    {
        ScaleAttributeDelegateHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(MiscAttributeSet->GetScaleAttribute()).AddUObject(this, &ACogSampleCharacter::OnScaleAttributeChanged);
    }

    //----------------------------------------
    // Register to GameplayEffect events
    //----------------------------------------
    GameplayEffectAddedHandle = AbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &ACogSampleCharacter::OnGameplayEffectAdded);
    GameplayEffectRemovedHandle = AbilitySystem->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &ACogSampleCharacter::OnGameplayEffectRemoved);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::UnregisterFromAbilitySystemEvents()
{
    //----------------------------------------
   // Unregister to Attribute events
   //----------------------------------------
    if (const UCogSampleAttributeSet_Misc* MiscAttributeSet = Cast<UCogSampleAttributeSet_Misc>(AbilitySystem->GetAttributeSet(UCogSampleAttributeSet_Misc::StaticClass())))
    {
        AbilitySystem->GetGameplayAttributeValueChangeDelegate(MiscAttributeSet->GetScaleAttribute()).Remove(ScaleAttributeDelegateHandle);
    }

    //----------------------------------------
    // Unregister to Tags events
    //----------------------------------------
    AbilitySystem->UnregisterGameplayTagEvent(GhostTagDelegateHandle, Tag_Status_Ghost, EGameplayTagEventType::NewOrRemoved);

    //----------------------------------------
    // Unregister to GameplayEffect events
    //----------------------------------------
    AbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.Remove(GameplayEffectAddedHandle);
    AbilitySystem->OnAnyGameplayEffectRemovedDelegate().Remove(GameplayEffectRemovedHandle);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::TryFinishInitialize()
{
    if (bIsInitialized)
    {
        return;
    }

    if (HasActorBegunPlay() == false)
    {
        return;
    }

#if ENABLE_COG
	ACogAbilityReplicator::TryApplyAllTweaksOnActor(this);
#endif //ENABLE_COG

    RefreshScale();

    RegisterToAbilitySystemEvents();

    bIsInitialized = true;

    OnInitialized.Broadcast(this);
}


//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACogSampleCharacter::Move);
		EnhancedInputComponent->BindAction(MoveZAction, ETriggerEvent::Triggered, this, &ACogSampleCharacter::MoveZ);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACogSampleCharacter::Look);

        int32 AbilityIndex = 0;
        for (const FActiveAbilityInfo& AbilityInfo : ActiveAbilities)
        {
            EnhancedInputComponent->BindAction(AbilityInfo.InputAction, ETriggerEvent::Started, this, &ACogSampleCharacter::OnAbilityInputStarted, AbilityIndex);
            EnhancedInputComponent->BindAction(AbilityInfo.InputAction, ETriggerEvent::Completed, this, &ACogSampleCharacter::OnAbilityInputCompleted, AbilityIndex);
            AbilityIndex++;
        }

        int32 ItemIndex = 0;
        for (const UInputAction* ItemAction : ItemActions)
        {
            EnhancedInputComponent->BindAction(ItemAction, ETriggerEvent::Started, this, &ACogSampleCharacter::ActivateItem, ItemIndex);
            ItemIndex++;
        }
	}
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnAbilityInputStarted(const FInputActionValue& Value, int32 Index)
{
    COG_LOG_OBJECT(LogCogInput, ELogVerbosity::Verbose, this, TEXT("%d"), Index);

    if (ActiveAbilityHandles.IsValidIndex(Index) == false)
    {
        return;
    }

    FGameplayAbilitySpecHandle Handle = ActiveAbilityHandles[Index];
    FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromHandle(Handle);
    if (Spec == nullptr)
    {
        return;
    }

    Spec->InputPressed = true;

    //-----------------------------------------------------
    // Replicate button press if ability is already active
    //-----------------------------------------------------
    if (Spec->IsActive())
    {
        const UGameplayAbility* AbilityToActivate = Spec->GetPrimaryInstance();
        if (AbilityToActivate->bReplicateInputDirectly && AbilitySystem->IsOwnerActorAuthoritative() == false)
        {
            AbilitySystem->ServerSetInputPressed(Spec->Handle);
        }
        AbilitySystem->AbilitySpecInputPressed(*Spec);
        AbilitySystem->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec->Handle, Spec->ActivationInfo.GetActivationPredictionKey());
    }
    else
    {
        AbilitySystem->TryActivateAbility(Handle);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnAbilityInputCompleted(const FInputActionValue& Value, int32 Index)
{
    COG_LOG_OBJECT(LogCogInput, ELogVerbosity::Verbose, this, TEXT("%d"), Index);

    if (ActiveAbilityHandles.IsValidIndex(Index) == false)
    {
        return;
    }

    FGameplayAbilitySpecHandle Handle = ActiveAbilityHandles[Index];
    FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromHandle(Handle);
    if (Spec == nullptr)
    {
        return;
    }

    Spec->InputPressed = false;

    UGameplayAbility* Ability= Spec->GetPrimaryInstance();
    if (Ability == nullptr)
    {
        return;
    }

    if (Spec->IsActive() == false)
    {
        return;
    }

    if (Ability->bReplicateInputDirectly && AbilitySystem->IsOwnerActorAuthoritative() == false)
    {
        AbilitySystem->ServerSetInputReleased(Spec->Handle);
    }

    AbilitySystem->AbilitySpecInputReleased(*Spec);
    AbilitySystem->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec->Handle, Spec->ActivationInfo.GetActivationPredictionKey());
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::ActivateItem(const FInputActionValue& Value, int32 Index)
{
    COG_LOG_OBJECT(LogCogInput, ELogVerbosity::Verbose, this, TEXT("%d"), Index);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MoveInput2D = Value.Get<FVector2D>();

    MoveInput.X = MoveInput2D.X;
    MoveInput.Y = MoveInput2D.Y;

    MoveInputInWorldSpace = MoveInput;

	if (Controller != nullptr)
	{
        MoveInputInWorldSpace = TransformInputInWorldSpace(MoveInput);
		AddMovementInput(MoveInputInWorldSpace);
	}
}

//--------------------------------------------------------------------------------------------------------------------------
FVector ACogSampleCharacter::TransformInputInWorldSpace(const FVector& Input) const
{
    if (Controller == nullptr)
    {
        return Input;
    }

    FRotator ControlRotation = Controller->GetControlRotation();
    ControlRotation.Pitch = 0.0f;
    ControlRotation.Roll = 0.0f;

    FVector WorldInput = ControlRotation.RotateVector(FVector(Input.Y, Input.X, 0.0f));

    return WorldInput;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::MoveZ(const FInputActionValue& Value)
{
    const float ZInput = Value.Get<float>();

    if (Controller != nullptr)
    {
        AddMovementInput(FVector::UpVector, ZInput);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

//--------------------------------------------------------------------------------------------------------------------------
ECogCommonAllegiance ACogSampleCharacter::GetAllegianceWithOtherActor(const AActor* OtherActor) const
{
    const ECogCommonAllegiance Allegiance = static_cast<ECogCommonAllegiance>(UCogSampleFunctionLibrary_Team::GetActorsAllegiance(this, OtherActor));
    return Allegiance;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::HandleDamageReceived(const FCogSampleDamageEventParams& Params) 
{
    OnDamageReceived.Broadcast(Params);

#if ENABLE_COG
    FCogDebugMetric::AddMetric(this, "Damage Received", Params.MitigatedDamage, Params.UnmitigatedDamage, false);
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::HandleDamageDealt(const FCogSampleDamageEventParams& Params)
{
    OnDamageDealt.Broadcast(Params);

#if ENABLE_COG
    FCogDebugMetric::AddMetric(this, "Damage Dealt", Params.MitigatedDamage, Params.UnmitigatedDamage, false);
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
bool ACogSampleCharacter::IsDead() const
{
    return bIsDead;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnKilled(AActor* InInstigator, AActor* InCauser, const FGameplayEffectSpec& InEffectSpec, float InMagnitude)
{
    bIsDead = true;

    if (AbilitySystem != nullptr)
    {
        FGameplayEventData Payload;
        Payload.EventTag = Tag_GameplayEvent_Killed;
        Payload.Instigator = InInstigator;
        Payload.Target = AbilitySystem->GetAvatarActor();
        Payload.OptionalObject = InEffectSpec.Def;
        Payload.ContextHandle = InEffectSpec.GetEffectContext();
        Payload.InstigatorTags = *InEffectSpec.CapturedSourceTags.GetAggregatedTags();
        Payload.TargetTags = *InEffectSpec.CapturedTargetTags.GetAggregatedTags();
        Payload.EventMagnitude = InMagnitude;

        FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
        AbilitySystem->HandleGameplayEvent(Payload.EventTag, &Payload);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnRevived(AActor* InInstigator, AActor* InCauser, const FGameplayEffectSpec& InEffectSpec, float InMagnitude)
{
    bIsDead = false;

    if (AbilitySystem != nullptr)
    {
        FGameplayEventData Payload;
        Payload.EventTag = Tag_GameplayEvent_Revived;
        Payload.Instigator = InInstigator;
        Payload.Target = AbilitySystem->GetAvatarActor();
        Payload.OptionalObject = InEffectSpec.Def;
        Payload.ContextHandle = InEffectSpec.GetEffectContext();
        Payload.InstigatorTags = *InEffectSpec.CapturedSourceTags.GetAggregatedTags();
        Payload.TargetTags = *InEffectSpec.CapturedTargetTags.GetAggregatedTags();
        Payload.EventMagnitude = InMagnitude;

        FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
        AbilitySystem->HandleGameplayEvent(Payload.EventTag, &Payload);
    }
}

// ----------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle Handle)
{
#if ENABLE_COG
    FCogDebugPlot::PlotEvent(this, "Effects", GameplayEffectSpec.Def->GetFName(), GameplayEffectSpec.GetDuration() == 0.0f)
                    .AddParam("Name", AbilitySystemComponent->CleanupName(GetNameSafe(GameplayEffectSpec.Def)))
                    .AddParam("Effect Instigator", GetNameSafe(GameplayEffectSpec.GetEffectContext().GetInstigator()))
                    .AddParam("Effect Level", GameplayEffectSpec.GetLevel())
                    .AddParam("Effect Duration", GameplayEffectSpec.GetDuration());
#endif //ENABLE_COG
}

// ----------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnGameplayEffectRemoved(const FActiveGameplayEffect& RemovedGameplayEffect)
{
#if ENABLE_COG
    FCogDebugPlot::PlotEventStop(this, "Effects", RemovedGameplayEffect.Spec.Def->GetFName());
#endif //ENABLE_COG
}

// ----------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnGhostTagNewOrRemoved(const FGameplayTag InTag, int32 NewCount)
{
#if UE_WITH_CHEAT_MANAGER

    check(InTag == Tag_Status_Ghost);

    bool bHasGhostTags = NewCount > 0;
    if (bIsInGhostMode == bHasGhostTags)
    {
        return;
    }
    
    bIsInGhostMode = bHasGhostTags;

    SetActorEnableCollision(bIsInGhostMode == false);
    CameraBoom->bDoCollisionTest = bIsInGhostMode == false;

    if (UCogSampleCharacterMovementComponent* MovementComponent = Cast<UCogSampleCharacterMovementComponent>(GetMovementComponent()))
    {
        MovementComponent->bCheatFlying = bIsInGhostMode;
        MovementComponent->SetMovementMode(bIsInGhostMode ? MOVE_Flying : MOVE_Falling);
    }

    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (bIsInGhostMode)
            {
                Subsystem->AddMappingContext(GhostMappingContext, 0);
            }
            else
            {
                Subsystem->RemoveMappingContext(GhostMappingContext);
            }
        }
    }

#endif //UE_WITH_CHEAT_MANAGER
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnScaleAttributeChanged(const FOnAttributeChangeData& Data)
{
    RefreshScale();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::RefreshScale()
{
    const float CurrentScaleValue = AbilitySystem->GetNumericAttribute(UCogSampleAttributeSet_Misc::GetScaleAttribute());
    Scale = CurrentScaleValue;

    MARK_PROPERTY_DIRTY_FROM_NAME(ACogSampleCharacter, Scale, this);
    OnRep_Scale();

    RefreshServerAnimTickOption();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::RefreshServerAnimTickOption()
{
    const UCapsuleComponent* Capsule = GetCapsuleComponent();
    if (Capsule == nullptr)
    {
        return;
    }

    USkeletalMeshComponent* SkeletalMesh = GetMesh();
    if (SkeletalMesh == nullptr)
    {
        return;
    }

    const float ScaledHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
    const bool IsBigEnoughToSimulateBonesOnServer = ScaledHalfHeight > 200;
    SkeletalMesh->VisibilityBasedAnimTickOption = IsBigEnoughToSimulateBonesOnServer ? EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones
                                                                                     : EVisibilityBasedAnimTickOption::OnlyTickMontagesWhenNotRendered;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnRep_Scale()
{
    SetActorScale3D(FVector(Scale));
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::SetTeam(int32 Value)
{
    COMPARE_ASSIGN_AND_MARK_PROPERTY_DIRTY(ACogSampleCharacter, Team, Value, this);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::SetProgressionLevel(int32 Value)
{
    COMPARE_ASSIGN_AND_MARK_PROPERTY_DIRTY(ACogSampleCharacter, ProgressionLevel, Value, this);
}

//--------------------------------------------------------------------------------------------------------------------------
int32 ACogSampleCharacter::ApplyRootMotion(const FCogSampleRootMotionParams& Params)
{
    if (HasAuthority() == false)
    {
        return (uint16)ERootMotionSourceID::Invalid;;
    }

    if (IsValid(Params.Effect))
    {
        FGameplayEffectContextHandle EffectContextHandle = AbilitySystem->MakeEffectContext();
        EffectContextHandle.AddInstigator(Params.Instigator, Params.Causer);

        FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(Params.Effect, 1.0f, EffectContextHandle);
        SpecHandle.Data->SetDuration(Params.Duration, true);

        if (SpecHandle.IsValid())
        {
            AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }

    Client_ApplyRootMotion(Params);
    int32 RootMotionSourceID = ApplyRootMotionShared(Params);
    return RootMotionSourceID;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::Client_ApplyRootMotion_Implementation(const FCogSampleRootMotionParams& Params)
{
    if (GetWorld()->GetNetMode() == NM_Client)
    {
        ApplyRootMotionShared(Params);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
uint16 ACogSampleCharacter::ApplyRootMotionShared(const FCogSampleRootMotionParams& Params)
{
    UCogSampleCharacterMovementComponent* MovementComponent = Cast<UCogSampleCharacterMovementComponent>(GetMovementComponent());
    if (MovementComponent == nullptr)
    {
        return (uint16)ERootMotionSourceID::Invalid;;
    }
    
    TSharedPtr<FRootMotionSource_JumpForce> JumpForce = MakeShared<FRootMotionSource_JumpForce>();
    JumpForce->InstanceName = "ForceMove";
    JumpForce->AccumulateMode = Params.IsAdditive ? ERootMotionAccumulateMode::Additive : ERootMotionAccumulateMode::Override;
    JumpForce->Priority = (uint16)Params.Priority;
    JumpForce->Duration = Params.Duration;
    JumpForce->Rotation = Params.Rotation;
    JumpForce->Distance = Params.Distance;
    JumpForce->Height = Params.Height;
    JumpForce->bDisableTimeout = Params.bFinishOnLanded; // If we finish on landed, we need to disable force's timeout
    JumpForce->PathOffsetCurve = Params.PathOffsetCurve;
    JumpForce->TimeMappingCurve = Params.TimeMappingCurve;
    JumpForce->FinishVelocityParams.Mode = Params.FinishVelocityMode;
    JumpForce->FinishVelocityParams.SetVelocity = Params.FinishSetVelocity;
    JumpForce->FinishVelocityParams.ClampVelocity = Params.FinishClampVelocity;

    uint16 RootMotionSourceID = MovementComponent->ApplyRootMotionSource(JumpForce);
    return RootMotionSourceID;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnRep_ActiveAbilityHandles()
{
    UpdateActiveAbilitySlots();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::UpdateActiveAbilitySlots()
{
    for (int32 i = 0; i < ActiveAbilityHandles.Num(); ++i)
    {
        FGameplayAbilitySpecHandle& Handle = ActiveAbilityHandles[i];

        if (UCogSampleFunctionLibrary_Tag::ActiveAbilityCooldownTags.IsValidIndex(i) == false)
        {
            return;
        }

        FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromHandle(Handle);
        if (Spec == nullptr)
        {
            continue;
        }

        UCogSampleGameplayAbility* AbilityInstance = Cast<UCogSampleGameplayAbility>(Spec->GetPrimaryInstance());
        if (AbilityInstance == nullptr)
        {
            continue;
        }

        FGameplayTag SlotTag = UCogSampleFunctionLibrary_Tag::ActiveAbilityCooldownTags[i];
        AbilityInstance->SetCooldownTag(SlotTag);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FVector ACogSampleCharacter::GetTargetLocation() const
{
    return GetActorLocation();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::GetTargetCapsules(TArray<const UCapsuleComponent*>& Capsules) const
{
    Capsules.Add(GetCapsuleComponent());
}

//--------------------------------------------------------------------------------------------------------------------------
bool ACogSampleCharacter::GetMontage(FName MontageName, UAnimMontage*& Montage, bool bPrintWarning) const
{
    Montage = nullptr;

    if (MontageTable == nullptr)
    {
        return false;
    }

    static FString ContextString(TEXT("GetMontage"));
    FCogSampleMontageTableRow* Row = MontageTable->FindRow<FCogSampleMontageTableRow>(MontageName, ContextString, bPrintWarning);

    if (Row == nullptr)
    {
        return false;
    }

    Montage = Row->Montage;
    return Montage != nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::SetIsAiming(bool Value)
{
    if (bIsAiming == Value)
    {
        return;
    }

    bIsAiming = Value;
    OnAimingChanged.Broadcast(this, bIsAiming);
}

