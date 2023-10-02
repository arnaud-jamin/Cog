#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CogSampleFunctionLibrary_Gameplay.h"
#include "CogSampleAttributeSet_Misc.generated.h"

UCLASS()
class UCogSampleAttributeSet_Misc : public UAttributeSet
{
    GENERATED_BODY()

public:

    friend struct FCogSampleDamageStatics;

    UCogSampleAttributeSet_Misc();

    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Misc, Scale);

    UFUNCTION()
    virtual void OnRep_Scale(const FGameplayAttributeData& PrevScale);

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:

    UPROPERTY(BlueprintReadOnly, Category = "Scale", ReplicatedUsing = OnRep_Scale, meta = (AllowPrivateAccess = "true"))
    FGameplayAttributeData Scale;

    void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const;

};

