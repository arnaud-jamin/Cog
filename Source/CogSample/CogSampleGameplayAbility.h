#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CogSampleGameplayAbility.generated.h"

class UCogSampleSpawnPredictionComponent;

UCLASS()
class UCogSampleGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:

    UCogSampleGameplayAbility();

    //----------------------------------------------------------------------------------------------------------------------
    // UGameplayAbility overrides
    //----------------------------------------------------------------------------------------------------------------------
    virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) override;
    
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    //----------------------------------------------------------------------------------------------------------------------
    // Cooldown & Cost
    //----------------------------------------------------------------------------------------------------------------------
    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = Ability)
    float GetUnmitigatedCooldownDuration() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = Ability)
    float GetUnmitigatedCost() const;

    const FGameplayTag& GetCooldownTag() const { return CooldownTag; }

    void SetCooldownTag(FGameplayTag Value) { CooldownTag = Value; }

    virtual const FGameplayTagContainer* GetCooldownTags() const override;

    virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
    
    virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
    
    virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
    
    virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

    UFUNCTION(BlueprintCallable, Category = Ability, BlueprintPure=false)
    virtual void GetCooldownInfos(float& TimeRemaining, float& CooldownDuration) const;

    //----------------------------------------------------------------------------------------------------------------------
    // Scalable Float
    //----------------------------------------------------------------------------------------------------------------------

    UFUNCTION(BlueprintPure)
    float GetFloatValueAtAbilityLevel(const FScalableFloat& ScalableFloat) const; 

    UFUNCTION(BlueprintPure)
    bool GetBoolValueAtAbilityLevel(const FScalableFloat& ScalableFloat) const;

    UFUNCTION(BlueprintPure)
    int32 GetIntValueAtAbilityLevel(const FScalableFloat& ScalableFloat) const;

    //----------------------------------------------------------------------------------------------------------------------
    // Predicated Actors
    //----------------------------------------------------------------------------------------------------------------------

    UFUNCTION(BlueprintCallable)
    void SetupSpawnPrediction(AActor* PredictedSpawn, int32 InstanceIndex);

    UFUNCTION(BlueprintCallable)
    void SetupSpawnPredictionComponent(UCogSampleSpawnPredictionComponent* SpawnPrediction, int32 InstanceIndex);

private:

    bool IsCostGameplayEffectIsZero(const UGameplayEffect* GameplayEffect, float Level, const FGameplayEffectContextHandle& EffectContext) const;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FGameplayTag CooldownTag;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldowns", meta = (AllowPrivateAccess = "true"))
    FScalableFloat Cooldown;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Costs", meta = (AllowPrivateAccess = "true"))
    FScalableFloat Cost;

    UPROPERTY(Transient)
    FGameplayTagContainer CachedCooldownTags;

};
