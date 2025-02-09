#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CogSampleFunctionLibrary_Gameplay.h"
#include "CogSampleAttributeSet_Ability_Shield.generated.h"

UCLASS()
class UCogSampleAttributeSet_Ability_Shield : public UAttributeSet
{
    GENERATED_BODY()

public:

    UCogSampleAttributeSet_Ability_Shield();

private:

    UPROPERTY(BlueprintReadOnly, Replicated, Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Cooldown;

    UPROPERTY(BlueprintReadOnly, Replicated, Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Cost;


public:
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Ability_Shield, Cooldown);
    ATTRIBUTE_ACCESSORS(UCogSampleAttributeSet_Ability_Shield, Cost);
};

