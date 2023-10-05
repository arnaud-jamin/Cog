#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GameplayEffectTypes.h"
#include "CogSampleModifierCalculation_Cooldown.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UCogSampleModifierCalculation_Cooldown : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:

    UCogSampleModifierCalculation_Cooldown();

    float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:

    FGameplayEffectAttributeCaptureDefinition CooldownReductionDef;
};
