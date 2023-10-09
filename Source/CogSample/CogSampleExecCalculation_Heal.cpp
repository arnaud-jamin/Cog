#include "CogSampleExecCalculation_Heal.h"

#include "CogSampleAttributeSet_Health.h"
#include "CogSampleCharacter.h"
#include "CogSampleFunctionLibrary_Tag.h"

//--------------------------------------------------------------------------------------------------------------------------
struct FCogSampleHealStatics
{

    FCogSampleHealStatics()
    {
    }
};

//--------------------------------------------------------------------------------------------------------------------------
static const FCogSampleHealStatics& HealStatics()
{
    static FCogSampleHealStatics __FCogSampleHealStatics;
    return __FCogSampleHealStatics;
}

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleExecCalculation_Heal::UCogSampleExecCalculation_Heal()
{
#if WITH_EDITORONLY_DATA
    ValidTransientAggregatorIdentifiers.AddTag(Tag_Effect_Data_Heal);
#endif // #if WITH_EDITORONLY_DATA
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleExecCalculation_Heal::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
    const FGameplayEffectContextHandle Context = EffectSpec.GetContext();


    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
    EvaluationParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

    UAbilitySystemComponent* TargetAbilitySystem = ExecutionParams.GetTargetAbilitySystemComponent();
    UAbilitySystemComponent* SourceAbilitySystem = ExecutionParams.GetSourceAbilitySystemComponent();

    FGameplayTagContainer SpecAssetTags;
    EffectSpec.GetAllAssetTags(SpecAssetTags);

    //-----------------------------------------------------------------------------------------------------
    // Get flat Heal
    //-----------------------------------------------------------------------------------------------------
    float Heal = 0.0f;
    ExecutionParams.AttemptCalculateTransientAggregatorMagnitude(Tag_Effect_Data_Heal, EvaluationParameters, Heal);

    //-----------------------------------------------------------------------------------------------------
    // Apply modifiers
    //-----------------------------------------------------------------------------------------------------
    const bool IsDead = TargetAbilitySystem->HasMatchingGameplayTag(Tag_Status_Dead);
    const bool IsEffectReviving = SpecAssetTags.HasTag(Tag_Effect_Type_Heal_Revive);
    if (IsDead && IsEffectReviving == false)
    {
        return;
    }

    if (SpecAssetTags.HasTag(Tag_Effect_Type_Heal_Full))
    {
        Heal = TargetAbilitySystem->GetNumericAttribute(UCogSampleAttributeSet_Health::GetMaxHealthAttribute());
    }

    //-----------------------------------------------------------------------------------------------------
    // Apply Heal
    //-----------------------------------------------------------------------------------------------------
    if (Heal > 0.0f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCogSampleAttributeSet_Health::GetHealthAttribute(), EGameplayModOp::Additive, Heal));
    }
}
