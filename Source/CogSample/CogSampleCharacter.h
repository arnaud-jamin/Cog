#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ActiveGameplayEffectHandle.h"
#include "AttributeSet.h"
#include "CogDefines.h"
#include "CogInterfaceAllegianceActor.h"
#include "CogInterfaceDebugFilteredActor.h"
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
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ECogSampleAllegianceFilter : uint8
{
    None    = 0 UMETA(Hidden),
    Ally    = 1 << 0,
    Neutral = 1 << 1,
    Enemy   = 1 << 2,
};
ENUM_CLASS_FLAGS(ECogSampleAllegianceFilter);

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
UCLASS(config=Game)
class ACogSampleCharacter : public ACharacter
    , public IAbilitySystemInterface
    , public ICogInterfacesDebugFilteredActor
    , public ICogInterfacesAllegianceActor
{
	GENERATED_BODY()

public:
    ACogSampleCharacter(const FObjectInitializer& ObjectInitializer);
    
    //----------------------------------------------------------------------------------------------------------------------
    // ACharacter overrides
    //----------------------------------------------------------------------------------------------------------------------
    virtual void BeginPlay();

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

    virtual void MarkComponentsAsPendingKill() override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void PossessedBy(AController* NewController) override;

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
    void OnAcknowledgePossession(APlayerController* InController);
    
    void OnDamageReceived(float DamageAmount, float UnmitigatedDamageAmount, AActor* DamageDealer, const FGameplayEffectSpec& EffectSpec);
    
    void OnDamageDealt(float DamageAmount, float UnmitigatedDamageAmount, AActor* DamageReceiver, const FGameplayEffectSpec& EffectSpec);
    
    void OnKilled(AActor* InInstigator, AActor* InCauser, const FGameplayEffectSpec& InEffectSpec, float InMagnitude);
    
    void OnRevived(AActor* InInstigator, AActor* InCauser, const FGameplayEffectSpec& InEffectSpec, float InMagnitude);
    
    //----------------------------------------------------------------------------------------------------------------------
    // Camera
    //----------------------------------------------------------------------------------------------------------------------
    USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
    //----------------------------------------------------------------------------------------------------------------------
    // Input
    //----------------------------------------------------------------------------------------------------------------------
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

    /** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* GhostMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

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

    //----------------------------------------------------------------------------------------------------------------------
    // Team
    //----------------------------------------------------------------------------------------------------------------------
    
    UFUNCTION(BlueprintPure)
    int32 GetTeamID() const { return TeamID; }

    UFUNCTION(BlueprintCallable)
    void SetTeamID(int32 Value);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Team, Replicated, meta = (AllowPrivateAccess = "true"))
    int32 TeamID = 0;
    
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

    //----------------------------------------------------------------------------------------------------------------------
    // Ability system
    //----------------------------------------------------------------------------------------------------------------------
    void InitializeAbilitySystem();

    void ShutdownAbilitySystem();

    void OnGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle Handle);

    void OnGameplayEffectRemoved(const FActiveGameplayEffect& RemovedGameplayEffect);

    void OnGhostTagNewOrRemoved(const FGameplayTag InTag, int32 NewCount);

    void OnScaleAttributeChanged(const FOnAttributeChangeData& Data);

    void UpdateActiveAbilitySlots();

    UFUNCTION()
    void OnRep_ActiveAbilityHandles();

    //----------------------------------------------------------------------------------------------------------------------
    // Root Motion 
    //----------------------------------------------------------------------------------------------------------------------
    UFUNCTION(Reliable, Client)
    void Client_ApplyRootMotion(const FCogSampleRootMotionParams& Params);

    uint16 ApplyRootMotionShared(const FCogSampleRootMotionParams& Params);

    //----------------------------------------------------------------------------------------------------------------------
    UPROPERTY(ReplicatedUsing=OnRep_ActiveAbilityHandles, Transient)
    TArray<FGameplayAbilitySpecHandle> ActiveAbilityHandles;
    
    FDelegateHandle GameplayEffectAddedHandle;

    FDelegateHandle GameplayEffectRemovedHandle;

    FDelegateHandle GhostTagDelegateHandle;

    FDelegateHandle ScaleAttributeDelegateHandle;

    bool bIsGhost = false;

    bool bIsInitialized = false;

};

