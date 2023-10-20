#include "CogSampleExecCalculation_Damage.h"

#include "CogSampleAbilitySystemComponent.h"
#include "CogSampleAttributeSet_Health.h"
#include "CogSampleCharacter.h"
#include "CogSampleDamageableInterface.h"
#include "CogSampleFunctionLibrary_Tag.h"
#include "CogSampleGameplayEffectContext.h"

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
    FGameplayEffectContextHandle EffectContext = EffectSpec.GetContext();

    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
    EvaluationParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

    UCogSampleAbilitySystemComponent* TargetAbilitySystem = Cast<UCogSampleAbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
    AActor* TargetActor = (TargetAbilitySystem != nullptr) ? TargetAbilitySystem->GetAvatarActor() : nullptr;

    UCogSampleAbilitySystemComponent* SourceAbilitySystem = Cast<UCogSampleAbilitySystemComponent>(ExecutionParams.GetSourceAbilitySystemComponent());
    AActor* SourceActor = (SourceAbilitySystem != nullptr) ? SourceAbilitySystem->GetAvatarActor() : nullptr;

    if (TargetActor == nullptr)
    {
        return;
    }

    FHitResult HitResult;
    if (const FHitResult* HitResultPtr = EffectContext.GetHitResult())
    {
        HitResult = *HitResultPtr;
    }
    else
    {
        HitResult.Location = TargetActor->GetActorLocation();
        HitResult.Normal = TargetActor->GetActorForwardVector();
        HitResult.ImpactNormal = TargetActor->GetActorForwardVector();
        EffectContext.AddHitResult(HitResult, true);
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
        Params.DamageDealer = SourceActor;
        Params.DamageReceiver = TargetActor;
        Params.MitigatedDamage = MitigatedDamage;
        Params.UnmitigatedDamage = UnmitigatedDamage;

        if (ICogSampleDamageableInterface* DamageReceiver = Cast<ICogSampleDamageableInterface>(TargetActor))
        {
            DamageReceiver->HandleDamageReceived(Params);
        }

        if (ICogSampleDamageableInterface* DamageDealer = Cast<ICogSampleDamageableInterface>(SourceActor))
        {
            DamageDealer->HandleDamageDealt(Params);
        }
    }

    UCogSampleFunctionLibrary_Damage::ExecuteDamageGameplayCue(TargetAbilitySystem, EffectSpec, HitResult, MitigatedDamage, EffectContext, false);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleFunctionLibrary_Damage::ExecuteDamageGameplayCue(
    UCogSampleAbilitySystemComponent* TargetAbilitySystem,
    const FGameplayEffectSpec& EffectSpec,
    const FHitResult& HitResult,
    float Damage,
    const FGameplayEffectContextHandle& EffectContext,
    bool IsPredicted)
{
    //-----------------------------------------------------------------------------------------------------
    // We check for >= 1 instead of >= 0 because we can use damage values between 0 and 1.
    //-----------------------------------------------------------------------------------------------------
    if (Damage == 0.0f)
    {
        return;
    }

    ensure(TargetAbilitySystem);
    if (TargetAbilitySystem == nullptr)
    {
        return;
    }

    AActor* TargetActor = TargetAbilitySystem->GetAvatarActor();
    if (TargetActor == nullptr)
    {
        return;
    }

    FCogSampleGameplayEffectContext* TypedContext = FCogSampleGameplayEffectContext::ExtractEffectContext(EffectContext);
    ensure(TypedContext);
    if (TypedContext == nullptr)
    {
        return;
    }

    TypedContext->bGameplayCueIsPredicted = IsPredicted;

    FGameplayCueParameters GameplayCueParameters;
    GameplayCueParameters.RawMagnitude = Damage;
    GameplayCueParameters.EffectContext = EffectContext;
    GameplayCueParameters.Instigator = TypedContext->GetInstigator();
    GameplayCueParameters.EffectCauser = TypedContext->GetEffectCauser();
    GameplayCueParameters.PhysicalMaterial = HitResult.PhysMaterial;
    GameplayCueParameters.Location = HitResult.Location;
    GameplayCueParameters.Normal = HitResult.ImpactNormal;

    TArray<FGameplayTag> Tags;
    SpecAssetTags.GetGameplayTagArray(Tags);
    
    FGameplayTag* FoundTag = Tags.FindByPredicate([](const FGameplayTag& Tag) { return Tag.MatchesTag(Tag_GameplayCue_DamageReceived); });
    FGameplayTag DamageTag = FoundTag != nullptr ? *FoundTag : Tag_GameplayCue_DamageReceived;

    if (IsPredicted)
    {
        TargetAbilitySystem->ExecuteGameplayCueLocal(DamageTag, GameplayCueParameters);
    }
    else
    {
        TargetAbilitySystem->ExecuteGameplayCue(DamageTag, GameplayCueParameters);
    }
}