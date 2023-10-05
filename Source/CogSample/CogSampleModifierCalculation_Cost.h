#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GameplayEffectTypes.h"
#include "CogSampleModifierCalculation_Cost.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UCogSampleModifierCalculation_Cost : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:

    UCogSampleModifierCalculation_Cost();

    float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:

    FGameplayEffectAttributeCaptureDefinition StaminaCostReductionDef;
};