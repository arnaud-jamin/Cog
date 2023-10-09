#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ScalableFloat.h"
#include "CogSampleExecCalculation_PoolRegen.generated.h"

UENUM()
enum class ECogSamplePoolRegenMode : uint8
{
    Attribute,
    MaxPoolDuration,
    ScalableFloatRate,
};

UCLASS()
class UCogSampleExecCalculation_PoolRegen : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UCogSampleExecCalculation_PoolRegen();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttribute AmountAttribute;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttribute MaxAmountAttribute;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag DisableRegenTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ECogSamplePoolRegenMode Mode = ECogSamplePoolRegenMode::MaxPoolDuration;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditConditionHides, EditCondition = "Mode == ECogSamplePoolRegenMode::Attribute"))
    FGameplayAttribute RegenRateAttribute;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditConditionHides, EditCondition = "Mode == ECogSamplePoolRegenMode::ScalableFloatRate"))
    FScalableFloat RegenRate;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditConditionHides, EditCondition = "Mode == ECogSamplePoolRegenMode::MaxPoolDuration"))
    FScalableFloat TotalDuration;

    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
