#include "CogSampleModifierCalculation_Cost.h"

#include "CogSampleGameplayAbility.h"
#include "CogSampleAttributeSet_Caster.h"
#include "CogSampleTagLibrary.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleModifierCalculation_Cost::UCogSampleModifierCalculation_Cost()
{
    StaminaCostReductionDef.AttributeToCapture = UCogSampleAttributeSet_Caster::GetStaminaCostReductionAttribute();
    StaminaCostReductionDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
    StaminaCostReductionDef.bSnapshot = false;

    RelevantAttributesToCapture.Add(StaminaCostReductionDef);
}

//--------------------------------------------------------------------------------------------------------------------------
float UCogSampleModifierCalculation_Cost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;

    float CostReduction = 0.0f;
    GetCapturedAttributeMagnitude(StaminaCostReductionDef, Spec, EvaluationParameters, CostReduction);

    const UCogSampleGameplayAbility* Ability = Cast<UCogSampleGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());
    const float UnmitigatedCost = (Ability != nullptr) ? Ability->GetUnmitigatedCost() : 0.0f;
    float MitigatedCost = UnmitigatedCost * FMath::Max(0.0f, 1.0f - CostReduction);

    if (SourceTags->HasTag(Tag_Effect_Type_Cost_Overtime))
    {
        const float Period = Spec.GetPeriod();
        MitigatedCost *= Period;
    }

    return -MitigatedCost;
}


