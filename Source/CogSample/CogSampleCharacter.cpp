#include "CogSampleCharacter.h"

#include "Camera/CameraComponent.h"
#include "CogDebugLogMacros.h"
#include "CogSampleAttributeSet_Health.h"
#include "CogSampleAttributeSet_Misc.h"
#include "CogSampleCharacterMovementComponent.h"
#include "CogSampleLogCategories.h"
#include "CogSampleTagLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/CheatManagerDefines.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

#if USE_COG
#include "CogDebugDraw.h"
#include "CogDebugPlot.h"
#endif //USE_COG

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

    AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
    AbilitySystem->SetIsReplicated(true);
    AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    Params.Condition = COND_OwnerOnly;

    DOREPLIFETIME_WITH_PARAMS_FAST(ACogSampleCharacter, ActiveAbilityHandles, Params);
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
    ShutdownAbilitySystem();
}

//--------------------------------------------------------------------------------------------------------------------------
UAbilitySystemComponent* ACogSampleCharacter::GetAbilitySystemComponent() const 
{
    return AbilitySystem; 
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::InitializeAbilitySystem()
{   
    if (bIsInitialized)
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

        for (FActiveAbilityInfo& AbilityInfo : ActiveAbilities)
        {
            const FGameplayAbilitySpec Spec(AbilityInfo.Ability, 1, INDEX_NONE, this);
            FGameplayAbilitySpecHandle Handle = AbilitySystem->GiveAbility(Spec);
            ActiveAbilityHandles.Add(Handle);
        }
        MARK_PROPERTY_DIRTY_FROM_NAME(ACogSampleCharacter, ActiveAbilityHandles, this);
    }

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

    bIsInitialized = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::ShutdownAbilitySystem()
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


    AbilitySystem->ClearActorInfo();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    InitializeAbilitySystem();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnAcknowledgePossession(APlayerController* InController)
{
    InitializeAbilitySystem();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

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
    AbilitySystem->TryActivateAbility(Handle);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnAbilityInputCompleted(const FInputActionValue& Value, int32 Index)
{
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
    COG_LOG_ACTOR(LogCogInput, ELogVerbosity::Verbose, this, TEXT("%d"), Index);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
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
void ACogSampleCharacter::OnDamageReceived(float ReceivedDamage, float IncomingDamage, AActor* DamageDealer, const FGameplayEffectSpec& EffectSpec)
{
#if USE_COG
    FCogAbilityDamageParams Params;
    Params.ReceivedDamage = ReceivedDamage;
    Params.IncomingDamage = IncomingDamage;
    Params.DamageDealer = DamageDealer;
    Params.DamageReceiver = this;
    OnDamageEventDelegate.Broadcast(Params);
#endif //USE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnDamageDealt(float ReceivedDamage, float IncomingDamage, AActor* DamageReceiver, const FGameplayEffectSpec& EffectSpec)
{
#if USE_COG
    FCogAbilityDamageParams Params;
    Params.ReceivedDamage = ReceivedDamage;
    Params.IncomingDamage = IncomingDamage;
    Params.DamageDealer = this;
    Params.DamageReceiver = DamageReceiver;
    OnDamageEventDelegate.Broadcast(Params);
#endif //USE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnKilled(AActor* InInstigator, AActor* InCauser, const FGameplayEffectSpec& InEffectSpec, float InMagnitude)
{
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
#if USE_COG
    FCogDebugPlot::PlotEvent(this, "Effects", GameplayEffectSpec.Def->GetFName(), GameplayEffectSpec.GetDuration() == 0.0f)
                    .AddParam("Name", AbilitySystemComponent->CleanupName(GetNameSafe(GameplayEffectSpec.Def)))
                    .AddParam("Effect Instigator", GetNameSafe(GameplayEffectSpec.GetEffectContext().GetInstigator()))
                    .AddParam("Effect Level", GameplayEffectSpec.GetLevel())
                    .AddParam("Effect Duration", GameplayEffectSpec.GetDuration());
#endif //USE_COG
}

// ----------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnGameplayEffectRemoved(const FActiveGameplayEffect& RemovedGameplayEffect)
{
#if USE_COG
    FCogDebugPlot::PlotEventStop(this, "Effects", RemovedGameplayEffect.Spec.Def->GetFName());
#endif //USE_COG
}

// ----------------------------------------------------------------------------------------------------------------
void ACogSampleCharacter::OnGhostTagNewOrRemoved(const FGameplayTag InTag, int32 NewCount)
{
#if UE_WITH_CHEAT_MANAGER

    check(InTag == Tag_Status_Ghost);

    bool bHasGhostTags = NewCount > 0;
    if (bIsGhost == bHasGhostTags)
    {
        return;
    }
    
    bIsGhost = bHasGhostTags;

    SetActorEnableCollision(bIsGhost == false);
    CameraBoom->bDoCollisionTest = bIsGhost == false;

    if (UCogSampleCharacterMovementComponent* MovementComponent = Cast<UCogSampleCharacterMovementComponent>(GetMovementComponent()))
    {
        MovementComponent->bCheatFlying = bIsGhost;
        MovementComponent->SetMovementMode(bIsGhost ? MOVE_Flying : MOVE_Falling);
    }

    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (bIsGhost)
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
    SetActorScale3D(FVector(Data.NewValue));
}
