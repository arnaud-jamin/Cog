#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ActiveGameplayEffectHandle.h"
#include "AttributeSet.h"
#include "CogDefines.h"
#include "CogInterfaceAllegianceActor.h"
#include "CogInterfaceDebugFilteredActor.h"
#include "CogSampleDamageEvent.h"
#include "CogSampleTargetableInterface.h"
#include "CogSampleTeamInterface.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "CogSampleCharacter.generated.h"

class UAbilitySystemComponent;
class UCogAbilitySystemComponent;
class UCameraComponent;
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCogSampleCharacterEventDelegate, ACogSampleCharacter*, Character);

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(config=Game)
class ACogSampleCharacter : public ACharacter
    , public IAbilitySystemInterface
    , public ICogInterfacesDebugFilteredActor
    , public ICogInterfacesAllegianceActor
    , public ICogSampleTeamInterface
    , public ICogSampleTargetableInterface
{
	GENERATED_BODY()

public:
    ACogSampleCharacter(const FObjectInitializer& ObjectInitializer);
    
    //----------------------------------------------------------------------------------------------------------------------
    // ACharacter overrides
    //----------------------------------------------------------------------------------------------------------------------
    
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void MarkComponentsAsPendingKill() override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void PossessedBy(AController* NewController) override;

    void OnAcknowledgePossession(APlayerController* InController);

    //----------------------------------------------------------------------------------------------------------------------
    // IAbilitySystemInterface overrides
    //----------------------------------------------------------------------------------------------------------------------
    
    UFUNCTION(BlueprintPure)
    UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    //----------------------------------------------------------------------------------------------------------------------
    // ICogInterfacesAllegianceActor overrides
    //----------------------------------------------------------------------------------------------------------------------
    
    ECogInterfacesAllegiance GetAllegianceWithOtherActor(const AActor* OtherActor) const override;

    //----------------------------------------------------------------------------------------------------------------------
    // ICogSampleTargetInterface overrides
    //----------------------------------------------------------------------------------------------------------------------
    
    virtual FVector GetTargetLocation() const override;

    virtual void GetTargetCapsules(TArray<const UCapsuleComponent*>& Capsules) const override;

    //----------------------------------------------------------------------------------------------------------------------
    // Team
    //----------------------------------------------------------------------------------------------------------------------
    
    UFUNCTION(BlueprintPure)
    virtual int32 GetTeam() const override { return Team; }

    UFUNCTION(BlueprintCallable)
    void SetTeamID(int32 Value);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Team, Replicated, meta = (AllowPrivateAccess = "true"))
    int32 Team = 0;

    //----------------------------------------------------------------------------------------------------------------------
    // Camera
    //----------------------------------------------------------------------------------------------------------------------
    USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
    //----------------------------------------------------------------------------------------------------------------------
    // Input
    //----------------------------------------------------------------------------------------------------------------------
    FVector TransformInputInWorldSpace(const FVector& Input) const;

    FVector GetMoveInput() const { return MoveInput; }

    FVector GetMoveInputInWorldSpace() const { return MoveInputInWorldSpace; }

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

    /** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* GhostMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

    /** Move Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveZAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TArray<UInputAction*> ItemActions;

    //----------------------------------------------------------------------------------------------------------------------
    // Ability
    //----------------------------------------------------------------------------------------------------------------------
    UFUNCTION(BlueprintPure)
    bool IsInitialized() const { return bIsInitialized; }

    void HandleDamageReceived(const FCogSampleDamageEventParams& Params);

    void HandleDamageDealt(const FCogSampleDamageEventParams& Params);

    void OnKilled(AActor* InInstigator, AActor* InCauser, const FGameplayEffectSpec& InEffectSpec, float InMagnitude);

    void OnRevived(AActor* InInstigator, AActor* InCauser, const FGameplayEffectSpec& InEffectSpec, float InMagnitude);

    UPROPERTY(BlueprintReadOnly, Category = Ability, meta = (AllowPrivateAccess = "true"))
    UAbilitySystemComponent* AbilitySystem = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    TArray<TSubclassOf<UAttributeSet>> AttributeSets;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    TArray<FActiveAbilityInfo> ActiveAbilities;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    TArray<FPassiveAbilityInfo> PassiveAbilities;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    TArray<TSubclassOf<UGameplayEffect>> Effects;

    UPROPERTY(BlueprintAssignable)
    FCogSampleCharacterEventDelegate OnInitialized;

    UPROPERTY(BlueprintAssignable)
    FCogSampleDamageEventDelegate OnDamageDealt;

    UPROPERTY(BlueprintAssignable)
    FCogSampleDamageEventDelegate OnDamageReceived;

    //----------------------------------------------------------------------------------------------------------------------
    // Root Motion
    //----------------------------------------------------------------------------------------------------------------------
    UFUNCTION(BlueprintCallable)
    int32 ApplyRootMotion(const FCogSampleRootMotionParams& Params);

private:

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

    void ShutdownAbilitySystem();

    void OnGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle Handle);

    void OnGameplayEffectRemoved(const FActiveGameplayEffect& RemovedGameplayEffect);

    void OnGhostTagNewOrRemoved(const FGameplayTag InTag, int32 NewCount);

    void OnScaleAttributeChanged(const FOnAttributeChangeData& Data);

    void UpdateActiveAbilitySlots();

    UFUNCTION()
    void OnRep_ActiveAbilityHandles();

    UPROPERTY(ReplicatedUsing=OnRep_ActiveAbilityHandles, Transient)
    TArray<FGameplayAbilitySpecHandle> ActiveAbilityHandles;
    
    FDelegateHandle GameplayEffectAddedHandle;

    FDelegateHandle GameplayEffectRemovedHandle;

    FDelegateHandle GhostTagDelegateHandle;

    FDelegateHandle ScaleAttributeDelegateHandle;

    bool bIsAbilitySystemInitialized = false;

    bool bIsInitialized = false;

    bool bIsInGhostMode = false;

    //----------------------------------------------------------------------------------------------------------------------
    // Root Motion 
    //----------------------------------------------------------------------------------------------------------------------

    UFUNCTION(Reliable, Client)
    void Client_ApplyRootMotion(const FCogSampleRootMotionParams& Params);

    uint16 ApplyRootMotionShared(const FCogSampleRootMotionParams& Params);
};

