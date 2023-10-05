#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CogSampleFunctionLibrary_Gameplay.h"
#include "CogSampleAttributeSet_Caster.generated.h"

UCLASS()
class UCogSampleAttributeSet_Caster : public UAttributeSet
{
    GENERATED_BODY()

public:

    UCogSampleAttributeSet_Caster();

    UFUNCTION()
    virtual void OnRep_BaseDamage(const FGameplayAttributeData& PrevBaseDamage);

    UFUNCTION()
    virtual void OnRep_CooldownReduction(const FGameplayAttributeData& PrevCooldownReduction);

    UFUNCTION()
    virtual void OnRep_AreaRadiusModifier(const FGameplayAttributeData& PrevAreaRadiusModifier);

    UFUNCTION()
    virtual void OnRep_Stamina(const FGameplayAttributeData& PrevStamina);

    UFUNCTION()
    virtual void OnRep_MinStamina(const FGameplayAttributeData& PrevMinStamina);

    UFUNCTION()
    virtual void OnRep_MaxStamina(const FGameplayAttributeData& PrevMaxStamina);

    UFUNCTION()
    virtual void OnRep_StaminaRegen(const FGameplayAttributeData& PrevStaminaRegen);

    UFUNCTION()
    virtual void OnRep_StaminaCostReduction(const FGameplayAttributeData& PrevStaminaCostReduction);

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:

    UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_BaseDamage, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData BaseDamage;

    UPROPERTY(BlueprintReadOnly, Category = "Modifier", ReplicatedUsing = OnRep_CooldownReduction, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData CooldownReduction;

    UPROPERTY(BlueprintReadOnly, Category = "Modifier", ReplicatedUsing = OnRep_AreaRadiusModifier, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData AreaRadiusModifier;
    
    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData Stamina;

    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_MinStamina, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData MinStamina;

    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_MaxStamina, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData MaxStamina;
    
    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_StaminaRegen, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData StaminaRegen;

    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_StaminaCostReduction, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData StaminaCostReduction;

    void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const;

public:
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Caster, BaseDamage);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Caster, CooldownReduction);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Caster, AreaRadiusModifier);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Caster, Stamina);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Caster, StaminaRegen)
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Caster, MinStamina);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Caster, MaxStamina);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Caster, StaminaCostReduction);
};

