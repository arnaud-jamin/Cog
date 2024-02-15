#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemInterface.h"
#include "ActiveGameplayEffectHandle.h"
#include "AttributeSet.h"
#include "CogCommonAllegianceActorInterface.h"
#include "CogCommonDebugFilteredActorInterface.h"
#include "CogSampleDamageableInterface.h"
#include "CogSampleDefines.h"
#include "CogSampleProgressionLevelInterface.h"
#include "CogSampleTargetableInterface.h"
#include "CogSampleTeamInterface.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"

#include "CogSampleCharacter.generated.h"

class UCameraComponent;
class UCogAbilitySystemComponent;
class UCogSampleAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
struct FActiveGameplayEffect;
struct FCogSampleRootMotionParams;
struct FGameplayEffectSpec;
struct FOnAttributeChangeData;

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FActiveAbilityInfo
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UGameplayAbility> Ability;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UInputAction* InputAction = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FPassiveAbilityInfo
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UGameplayAbility> Ability;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool ActivateWhenGiven = false;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FCogSampleMontageTableRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    FCogSampleMontageTableRow() {}

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UAnimMontage* Montage = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCogSampleCharacterEventDelegate, ACogSampleCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCogSampleAimingChangedEventDelegate, ACogSampleCharacter*, Character, bool, IsAiming);

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(config=Game)
class ACogSampleCharacter : public ACharacter
    , public IAbilitySystemInterface
    , public ICogCommonDebugFilteredActorInterface
    , public ICogCommonAllegianceActorInterface
    , public ICogSampleTeamInterface
    , public ICogSampleProgressionLevelInterface
    , public ICogSampleTargetableInterface
    , public ICogSampleDamageableInterface
{
	GENERATED_BODY()

public:
    ACogSampleCharacter(const FObjectInitializer& ObjectInitializer);
    
    //----------------------------------------------------------------------------------------------------------------------
    // ACharacter overrides
    //----------------------------------------------------------------------------------------------------------------------
    
    virtual void PostInitializeComponents() override;

    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void MarkComponentsAsPendingKill() override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void PossessedBy(AController* NewController) override;

    virtual void UnPossessed() override;

    virtual void AcknowledgePossession(AController* NewController);

    virtual void AcknowledgeUnpossession();

    //----------------------------------------------------------------------------------------------------------------------
    // IAbilitySystemInterface overrides
    //----------------------------------------------------------------------------------------------------------------------
    
    UFUNCTION(BlueprintPure)
    UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    //----------------------------------------------------------------------------------------------------------------------
    // ICogInterfacesAllegianceActor overrides
    //----------------------------------------------------------------------------------------------------------------------
    
    ECogCommonAllegiance GetAllegianceWithOtherActor(const AActor* OtherActor) const override;

    //----------------------------------------------------------------------------------------------------------------------
    // ICogSampleTargetInterface overrides
    //----------------------------------------------------------------------------------------------------------------------
    
    virtual FVector GetTargetActorLocation() const override;

    virtual void GetTargetCapsules(TArray<const UCapsuleComponent*>& Capsules) const override;

    //----------------------------------------------------------------------------------------------------------------------
    // ICogSampleDamageableInterface overrides
    //----------------------------------------------------------------------------------------------------------------------

    virtual void HandleDamageReceived(const FCogSampleDamageEventParams& Params) override;

    virtual void HandleDamageDealt(const FCogSampleDamageEventParams& Params) override;

    virtual bool IsDead() const override;

    //----------------------------------------------------------------------------------------------------------------------
    // Aiming
    //----------------------------------------------------------------------------------------------------------------------

    UFUNCTION(BlueprintCallable)
    virtual void SetIsAiming(bool Value);

    UFUNCTION(BlueprintPure)
    virtual bool GetIsAiming() const { return bIsAiming; }

    UPROPERTY(BlueprintAssignable)
    FCogSampleAimingChangedEventDelegate OnAimingChanged;

    //----------------------------------------------------------------------------------------------------------------------
    // Team
    //----------------------------------------------------------------------------------------------------------------------
    
    UFUNCTION(BlueprintPure)
    virtual int32 GetTeam() const override { return Team; }

    UFUNCTION(BlueprintCallable)
    virtual void SetTeam(int32 Value) override;

    //----------------------------------------------------------------------------------------------------------------------
    // Level
    //----------------------------------------------------------------------------------------------------------------------

    virtual int32 GetProgressionLevel() const override { return ProgressionLevel; }

    UFUNCTION(BlueprintCallable)
    virtual void SetProgressionLevel(int32 Value) override;

    //----------------------------------------------------------------------------------------------------------------------
    // Camera
    //----------------------------------------------------------------------------------------------------------------------

    USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
	UCameraComponent* FollowCamera;
	
    //----------------------------------------------------------------------------------------------------------------------
    // Input
    //----------------------------------------------------------------------------------------------------------------------

    FVector TransformInputInWorldSpace(const FVector& Input) const;

    FVector GetMoveInput() const { return MoveInput; }

    FVector GetMoveInputInWorldSpace() const { return MoveInputInWorldSpace; }

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
	UInputMappingContext* DefaultMappingContext;

    /** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
    UInputMappingContext* GhostMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
	UInputAction* MoveAction;

    /** Move Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
    UInputAction* MoveZAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
	UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
    TArray<UInputAction*> ItemActions;

    //----------------------------------------------------------------------------------------------------------------------
    // Ability
    //----------------------------------------------------------------------------------------------------------------------
    UFUNCTION(BlueprintPure)
    bool IsInitialized() const { return bIsInitialized; }

    void OnKilled(AActor* InInstigator, AActor* InCauser, const FGameplayEffectSpec& InEffectSpec, float InMagnitude);

    void OnRevived(AActor* InInstigator, AActor* InCauser, const FGameplayEffectSpec& InEffectSpec, float InMagnitude);

    UPROPERTY(BlueprintReadOnly, Category = Ability, meta = (AllowPrivateAccess))
    UCogSampleAbilitySystemComponent* AbilitySystem = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    TArray<TSubclassOf<UAttributeSet>> AttributeSets;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    TArray<FActiveAbilityInfo> ActiveAbilities;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    TArray<FPassiveAbilityInfo> PassiveAbilities;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    TArray<TSubclassOf<UGameplayEffect>> Effects;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    FGameplayTagContainer InitialTags;

    UPROPERTY(BlueprintAssignable)
    FCogSampleCharacterEventDelegate OnInitialized;

    UPROPERTY(BlueprintAssignable)
    FCogSampleDamageEventDelegate OnDamageDealt;

    UPROPERTY(BlueprintAssignable)
    FCogSampleDamageEventDelegate OnDamageReceived;

    //UPROPERTY(BlueprintAssignable)
    //FCogSampleGameplayEffectAddedEventDelegate OnEffectAdded;

    //UPROPERTY(BlueprintAssignable)
    //FCogSampleGameplayEffectRemovedEventDelegate OnEffectRemoved;

    //----------------------------------------------------------------------------------------------------------------------
    // Root Motion
    //----------------------------------------------------------------------------------------------------------------------
    UFUNCTION(BlueprintCallable)
    int32 ApplyRootMotion(const FCogSampleRootMotionParams& Params);

    //----------------------------------------------------------------------------------------------------------------------
    // Montage
    //----------------------------------------------------------------------------------------------------------------------
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (RowType = "/Script/CogSample.FCogSampleMontageTableRow"))
    UDataTable* MontageTable = nullptr;

    UFUNCTION(BlueprintCallable, BlueprintPure=false)
    bool GetMontage(FName MontageName, UAnimMontage*& Montage, bool bPrintWarning) const;

protected:
    friend class ACogSamplePlayerController;

    void RefreshServerAnimTickOption();

    UPROPERTY(EditAnywhere, Category = Team, Replicated, meta = (AllowPrivateAccess))
    int32 Team = 0;

    UPROPERTY(EditAnywhere, Category = Level, Replicated, meta = (AllowPrivateAccess))
    int32 ProgressionLevel = 0;

    UPROPERTY()
    AController* InitialController = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
    FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

    //----------------------------------------------------------------------------------------------------------------------
    // Inputs
    //----------------------------------------------------------------------------------------------------------------------

	void Move(const FInputActionValue& Value);

    void MoveZ(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

    void OnAbilityInputStarted(const FInputActionValue& Value, int32 Index);

    void OnAbilityInputCompleted(const FInputActionValue& Value, int32 Index);

    void ActivateItem(const FInputActionValue& Value, int32 Index);

    FVector MoveInput;

    FVector MoveInputInWorldSpace;

    //----------------------------------------------------------------------------------------------------------------------
    // Ability
    //----------------------------------------------------------------------------------------------------------------------

    void TryFinishInitialize();

    void InitializeAbilitySystem();

    void RegisterToAbilitySystemEvents();

    void UnregisterFromAbilitySystemEvents();

    void OnGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle Handle);

    void OnGameplayEffectRemoved(const FActiveGameplayEffect& RemovedGameplayEffect);

    void OnGhostTagNewOrRemoved(const FGameplayTag InTag, int32 NewCount);

    void OnScaleAttributeChanged(const FOnAttributeChangeData& Data);

    void UpdateActiveAbilitySlots();

    void RefreshScale();

    UFUNCTION()
    void OnRep_Scale();

    UFUNCTION()
    void OnRep_ActiveAbilityHandles();

    UPROPERTY(ReplicatedUsing=OnRep_ActiveAbilityHandles, Transient)
    TArray<FGameplayAbilitySpecHandle> ActiveAbilityHandles;
    
    UPROPERTY(ReplicatedUsing = OnRep_Scale, Transient)
    float Scale = 1.0f;

    FDelegateHandle GameplayEffectAddedHandle;

    FDelegateHandle GameplayEffectRemovedHandle;

    FDelegateHandle GhostTagDelegateHandle;

    FDelegateHandle ScaleAttributeDelegateHandle;

    bool bIsAbilitySystemInitialized = false;

    bool bIsInitialized = false;

    bool bIsInGhostMode = false;

    bool bIsDead = false;

    //----------------------------------------------------------------------------------------------------------------------
    // Root Motion 
    //----------------------------------------------------------------------------------------------------------------------

    UFUNCTION(Reliable, Client)
    void Client_ApplyRootMotion(const FCogSampleRootMotionParams& Params);

    uint16 ApplyRootMotionShared(const FCogSampleRootMotionParams& Params);

    //----------------------------------------------------------------------------------------------------------------------
    // Aiming
    //----------------------------------------------------------------------------------------------------------------------
    
    bool bIsAiming = false;

};

