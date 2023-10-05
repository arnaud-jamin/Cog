#include "CogSampleModifierCalculation_Cooldown.h"

#include "CogSampleGameplayAbility.h"
#include "CogSampleAttributeSet_Caster.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleModifierCalculation_Cooldown::UCogSampleModifierCalculation_Cooldown()
{
    CooldownReductionDef.AttributeToCapture = UCogSampleAttributeSet_Caster::GetCooldownReductionAttribute();
    CooldownReductionDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
    CooldownReductionDef.bSnapshot = false;

    RelevantAttributesToCapture.Add(CooldownReductionDef);
}

//--------------------------------------------------------------------------------------------------------------------------
float UCogSampleModifierCalculation_Cooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    FAggregatorEvaluateParameters EvaluationParameters;
    float CooldownReduction = 0.0f;
    GetCapturedAttributeMagnitude(CooldownReductionDef, Spec, EvaluationParameters, CooldownReduction);

    const UCogSampleGameplayAbility* Ability = Cast<UCogSampleGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());

    const float UnmitigatedCooldown = (Ability != nullptr) ? Ability->GetUnmitigatedCooldownDuration() : 0.0f;
    const float MitigatedCooldown = UnmitigatedCooldown * FMath::Max(0.0f, 1.0f - CooldownReduction);

    return MitigatedCooldown;
}
