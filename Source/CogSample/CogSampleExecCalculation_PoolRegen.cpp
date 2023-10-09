#include "CogSampleExecCalculation_PoolRegen.h"

#include "AbilitySystemComponent.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleExecCalculation_PoolRegen::UCogSampleExecCalculation_PoolRegen()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleExecCalculation_PoolRegen::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    UAbilitySystemComponent* TargetAbilitySystem = ExecutionParams.GetTargetAbilitySystemComponent();

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    float MaxAmount = 0.0f;
    if (MaxAmountAttribute.IsValid())
    {
        const float Amount = TargetAbilitySystem->GetNumericAttribute(AmountAttribute);
        MaxAmount = TargetAbilitySystem->GetNumericAttribute(MaxAmountAttribute);

        if (Amount >= MaxAmount)
        {
            return;
        }
    }

    const float Period = ExecutionParams.GetOwningSpec().GetPeriod();

    float RegenRateValue = 0.0f;

    if (TargetAbilitySystem->HasMatchingGameplayTag(DisableRegenTag) == false)
    {
        switch (Mode)
        {
            case ECogSamplePoolRegenMode::Attribute:
            {
                RegenRateValue = TargetAbilitySystem->GetNumericAttribute(RegenRateAttribute);
            }
            break;

            case ECogSamplePoolRegenMode::ScalableFloatRate:
            {
                RegenRateValue = RegenRate.GetValueAtLevel(Spec.GetLevel());
            }

            case ECogSamplePoolRegenMode::MaxPoolDuration:
            {
                const float TotalDurationValue = TotalDuration.GetValueAtLevel(Spec.GetLevel());
                RegenRateValue = TotalDurationValue > 0 ? (MaxAmount / TotalDurationValue) * Period : 0.0f;
            }
            break;

        default:
            break;
        }
    }

    if (RegenRateValue != 0.0f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(AmountAttribute, EGameplayModOp::Additive, RegenRateValue));
    }
}
