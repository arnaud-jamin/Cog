#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CogSampleFunctionLibrary_Gameplay.h"
#include "CogSampleAttributeSet_Health.generated.h"

UCLASS()
class UCogSampleAttributeSet_Health : public UAttributeSet
{
    GENERATED_BODY()

public:

    friend struct FCogSampleDamageStatics;

    UCogSampleAttributeSet_Health();

    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Health, Health);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Health, MaxHealth);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Health, HealthRegen)
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Health, Armor);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Health, MaxArmor);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Health, ArmorRegen)
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Health, DamageResistance)

    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& PrevHealth);

    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& PrevMaxHealth);

    UFUNCTION()
    virtual void OnRep_HealthRegen(const FGameplayAttributeData& PrevHealthRegen);

    UFUNCTION()
    virtual void OnRep_Armor(const FGameplayAttributeData& PrevArmor);
    
    UFUNCTION()
    virtual void OnRep_MaxArmor(const FGameplayAttributeData& PrevMaxArmor);

    UFUNCTION()
    virtual void OnRep_ArmorRegen(const FGameplayAttributeData& PrevArmorRegen);

    UFUNCTION()
    virtual void OnRep_DamageResistance(const FGameplayAttributeData& PrevDamageResistance);

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:

    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData Health;

    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData MaxHealth;

    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_HealthRegen, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData HealthRegen;

    UPROPERTY(BlueprintReadOnly, Category = "Armor", ReplicatedUsing = OnRep_Armor, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData Armor;

    UPROPERTY(BlueprintReadOnly, Category = "Armor", ReplicatedUsing = OnRep_MaxArmor, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData MaxArmor;

    UPROPERTY(BlueprintReadOnly, Category = "Armor", ReplicatedUsing = OnRep_ArmorRegen, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData ArmorRegen;

    UPROPERTY(BlueprintReadOnly, Category = "Armor", ReplicatedUsing = OnRep_DamageResistance, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData DamageResistance;

    void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const;

    bool bOutOfHealth = false;
};

