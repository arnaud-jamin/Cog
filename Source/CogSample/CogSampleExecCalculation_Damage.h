#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "CogSampleExecCalculation_Damage.generated.h"

UCLASS()
class UCogSampleExecCalculation_Damage : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UCogSampleExecCalculation_Damage();

    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
