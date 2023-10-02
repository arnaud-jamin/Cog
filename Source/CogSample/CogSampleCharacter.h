#pragma once

#include "CoreMinimal.h"
#include "CogDefines.h"
#include "AbilitySystemInterface.h"
#include "ActiveGameplayEffectHandle.h"
#include "AttributeSet.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"

#if USE_COG
#include "CogAbilityDamageActorInterface.h"
#include "CogDebugFilteredActorInterface.h"
#endif //USE_COG

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
UCLASS(config=Game)
class ACogSampleCharacter : public ACharacter
    , public IAbilitySystemInterface
#if USE_COG
    , public ICogDebugFilteredActorInterface
    , public ICogAbilityDamageActorInterface
#endif //USE_COG
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

#if USE_COG
    virtual FCogAbilityOnDamageEvent& OnDamageEvent() override { return OnDamageEventDelegate; }
    virtual bool IsActorFilteringDebug() const override { return true; }
#endif //USE_COG


    void OnAcknowledgePossession(APlayerController* InController);
    
    void OnDamageReceived(float DamageAmount, float UnmitigatedDamageAmount, AActor* DamageDealer, const FGameplayEffectSpec& EffectSpec);
    
    void OnDamageDealt(float DamageAmount, float UnmitigatedDamageAmount, AActor* DamageReceiver, const FGameplayEffectSpec& EffectSpec);
    
    void OnKilled(AActor* InInstigator, AActor* InCauser, const FGameplayEffectSpec& InEffectSpec, float InMagnitude);
    
    void OnRevived(AActor* InInstigator, AActor* InCauser, const FGameplayEffectSpec& InEffectSpec, float InMagnitude);
    
    USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    UFUNCTION(BlueprintPure)
    UAbilitySystemComponent* GetAbilitySystemComponent() const  override;

    //----------------------------------------------------------------------------------------------------------------------
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
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

#if USE_COG
    FCogAbilityOnDamageEvent OnDamageEventDelegate;
#endif //USE_COG

private:

    void InitializeAbilitySystem();
    void ShutdownAbilitySystem();
	void Move(const FInputActionValue& Value);
    void MoveZ(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
    void OnAbilityInputStarted(const FInputActionValue& Value, int32 Index);
    void OnAbilityInputCompleted(const FInputActionValue& Value, int32 Index);
    void ActivateItem(const FInputActionValue& Value, int32 Index);
    void OnGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle Handle);
    void OnGameplayEffectRemoved(const FActiveGameplayEffect& RemovedGameplayEffect);
    void OnGhostTagNewOrRemoved(const FGameplayTag InTag, int32 NewCount);
    void OnScaleAttributeChanged(const FOnAttributeChangeData& Data);

    UPROPERTY(Replicated, Transient)
    TArray<FGameplayAbilitySpecHandle> ActiveAbilityHandles;
    
    FDelegateHandle GameplayEffectAddedHandle;
    FDelegateHandle GameplayEffectRemovedHandle;
    FDelegateHandle GhostTagDelegateHandle;
    FDelegateHandle ScaleAttributeDelegateHandle;
    bool bIsGhost = false;
    bool bIsInitialized = false;
};

