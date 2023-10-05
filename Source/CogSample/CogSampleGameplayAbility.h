#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CogSampleGameplayAbility.generated.h"

UCLASS()
class UCogSampleGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:

    UCogSampleGameplayAbility();

    const FGameplayTag& GetSlotTag() const { return SlotTag; }

    void SetSlotTag(FGameplayTag Value) { SlotTag = Value; }

    //----------------------------------------------------------------------------------------------------------------------
    // Cooldown & Cost
    //----------------------------------------------------------------------------------------------------------------------

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = Ability)
    float GetUnmitigatedCooldownDuration() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = Ability)
    float GetUnmitigatedCost() const;

    virtual const FGameplayTagContainer* GetCooldownTags() const override;

    virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
    
    virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
    
    virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
    
    virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;


    //----------------------------------------------------------------------------------------------------------------------
    // Scalable Float
    //----------------------------------------------------------------------------------------------------------------------

    UFUNCTION(BlueprintPure)
    float GetFloatValueAtAbilityLevel(const FScalableFloat& ScalableFloat) const; 

    UFUNCTION(BlueprintPure)
    bool GetBoolValueAtAbilityLevel(const FScalableFloat& ScalableFloat) const;

    UFUNCTION(BlueprintPure)
    int32 GetIntValueAtAbilityLevel(const FScalableFloat& ScalableFloat) const;

private:

    bool IsCostGameplayEffectIsZero(const UGameplayEffect* GameplayEffect, float Level, const FGameplayEffectContextHandle& EffectContext) const;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FGameplayTag SlotTag;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldowns", meta = (AllowPrivateAccess = "true"))
    FScalableFloat Cooldown;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Costs", meta = (AllowPrivateAccess = "true"))
    FScalableFloat Cost;

    UPROPERTY(Transient)
    FGameplayTagContainer CachedCooldownTags;

};
