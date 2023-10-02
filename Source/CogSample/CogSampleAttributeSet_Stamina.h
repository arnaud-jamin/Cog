#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CogSampleFunctionLibrary_Gameplay.h"
#include "CogSampleAttributeSet_Stamina.generated.h"

UCLASS()
class UCogSampleAttributeSet_Stamina : public UAttributeSet
{
    GENERATED_BODY()

public:

    UCogSampleAttributeSet_Stamina();

    UFUNCTION()
    virtual void OnRep_Stamina(const FGameplayAttributeData& PrevStamina);		

    UFUNCTION()
    virtual void OnRep_MinStamina(const FGameplayAttributeData& PrevMinStamina);

    UFUNCTION()
    virtual void OnRep_MaxStamina(const FGameplayAttributeData& PrevMaxStamina);

    UFUNCTION()
    virtual void OnRep_StaminaRegen(const FGameplayAttributeData& PrevStaminaRegen);

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:

    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData Stamina;

    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_MinStamina, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData MinStamina;

    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_MaxStamina, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData MaxStamina;
    
    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_StaminaRegen, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData StaminaRegen;

    void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const;

public:
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Stamina, Stamina);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Stamina, StaminaRegen)
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Stamina, MinStamina);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Stamina, MaxStamina);

};

