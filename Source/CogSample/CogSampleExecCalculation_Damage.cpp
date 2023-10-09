#include "CogSampleExecCalculation_Damage.h"

#include "CogSampleAttributeSet_Health.h"
#include "CogSampleCharacter.h"
#include "CogSampleFunctionLibrary_Tag.h"

//--------------------------------------------------------------------------------------------------------------------------
struct FCogSampleDamageStatics
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(DamageResistance);

    FCogSampleDamageStatics()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(UCogSampleAttributeSet_Health, DamageResistance, Target, false);
    }
};

//--------------------------------------------------------------------------------------------------------------------------
static const FCogSampleDamageStatics& DamageStatics()
{
    static FCogSampleDamageStatics __FCogSampleDamageStatics;
    return __FCogSampleDamageStatics;
}

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleExecCalculation_Damage::UCogSampleExecCalculation_Damage()
{
    RelevantAttributesToCapture.Add(FCogSampleDamageStatics().DamageResistanceDef);

#if WITH_EDITORONLY_DATA
    ValidTransientAggregatorIdentifiers.AddTag(Tag_Effect_Data_Damage);
    InvalidScopedModifierAttributes.Add(FCogSampleDamageStatics().DamageResistanceDef);
#endif // #if WITH_EDITORONLY_DATA
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleExecCalculation_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
    const FGameplayEffectContextHandle Context = EffectSpec.GetContext();

    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
    EvaluationParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

    UAbilitySystemComponent* TargetAbilitySystem = ExecutionParams.GetTargetAbilitySystemComponent();
    UAbilitySystemComponent* SourceAbilitySystem = ExecutionParams.GetSourceAbilitySystemComponent();
    ACogSampleCharacter* TargetCharacter = (TargetAbilitySystem != nullptr) ? Cast<ACogSampleCharacter>(TargetAbilitySystem->AbilityActorInfo.IsValid() ? TargetAbilitySystem->GetAvatarActor() : nullptr) : nullptr;
    ACogSampleCharacter* SourceCharacter = (SourceAbilitySystem != nullptr) ? Cast<ACogSampleCharacter>(SourceAbilitySystem->AbilityActorInfo.IsValid() ? SourceAbilitySystem->GetAvatarActor() : nullptr) : nullptr;

    if (TargetCharacter == nullptr)
    {
        return;
    }

    FGameplayTagContainer SpecAssetTags;
    EffectSpec.GetAllAssetTags(SpecAssetTags);

    //-----------------------------------------------------------------------------------------------------
    // Get flat Damage
    //-----------------------------------------------------------------------------------------------------
    float UnmitigatedDamage = 0.0f;
    ExecutionParams.AttemptCalculateTransientAggregatorMagnitude(Tag_Effect_Data_Damage, EvaluationParameters, UnmitigatedDamage);

    //-----------------------------------------------------------------------------------------------------
    // Apply resistances
    //-----------------------------------------------------------------------------------------------------
    float MitigatedDamage = 0.0f;
    if (TargetAbilitySystem->HasMatchingGameplayTag(Tag_Status_Immune_Damage) == false)
    {
        float Resistances = 0.0f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageResistanceDef, EvaluationParameters, Resistances);
        Resistances = FMath::Min(Resistances, 1.0f);
        MitigatedDamage = UnmitigatedDamage * (1.0f - Resistances);
    }

    if (SpecAssetTags.HasTag(Tag_Effect_Type_Damage_Kill))
    {
        UnmitigatedDamage = TargetAbilitySystem->GetNumericAttribute(UCogSampleAttributeSet_Health::GetMaxHealthAttribute());
        MitigatedDamage  = UnmitigatedDamage;
    }

    //-----------------------------------------------------------------------------------------------------
    // Apply Damage
    //-----------------------------------------------------------------------------------------------------
    if (MitigatedDamage > 0.0f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCogSampleAttributeSet_Health::GetHealthAttribute(), EGameplayModOp::Additive, -MitigatedDamage));

        FCogSampleDamageEventParams Params;
        Params.DamageDealer = SourceCharacter;
        Params.DamageReceiver = TargetCharacter;
        Params.MitigatedDamage = MitigatedDamage;
        Params.UnmitigatedDamage = UnmitigatedDamage;

        TargetCharacter->HandleDamageReceived(Params);

        if (SourceCharacter != nullptr)
        {
            SourceCharacter->HandleDamageDealt(Params);
        }
    }
}
