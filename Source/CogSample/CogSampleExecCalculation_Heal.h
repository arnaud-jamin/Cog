#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "CogSampleExecCalculation_Heal.generated.h"

UCLASS()
class UCogSampleExecCalculation_Heal : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UCogSampleExecCalculation_Heal();

    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
