#include "CogSampleGameplayAbility.h"

#include "CogSampleAttributeSet_Caster.h"
#include "CogSampleFunctionLibrary_Gameplay.h"
#include "CogSampleFunctionLibrary_Tag.h"
#include "CogSampleGameplayEffectContext.h"
#include "CogSampleLogCategories.h"
#include "CogSamplePlayerController.h"
#include "CogSampleSpawnPredictionComponent.h"

#if ENABLE_COG
#include "CogDebugPlot.h"
#endif

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleGameplayAbility::UCogSampleGameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    ActivationBlockedTags.AddTag(Tag_Status_Dead);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
#if ENABLE_COG

    COG_LOG_ABILITY(ELogVerbosity::Verbose, this, TEXT(""));

    FCogDebugPlot::PlotEventStart(this, "Ability", GetFName())
		.AddParam("Name", GetNameSafe(this))
		.AddParam("Owner", GetNameSafe(ActorInfo->OwnerActor.Get()))
		.AddParam("Avatar", GetNameSafe(ActorInfo->AvatarActor.Get()))
		.AddParam("Player Controller", GetNameSafe(ActorInfo->PlayerController.Get()))
		.AddParam("Prediction Key", ActivationInfo.GetActivationPredictionKey().ToString())
		.AddParam("Event Tag", TriggerEventData ? *TriggerEventData->EventTag.ToString() : FString("None"))
		.AddParam("Event Magnitude", TriggerEventData ? TriggerEventData->EventMagnitude : 0.0f);

#endif

    Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);


}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    COG_LOG_ABILITY(ELogVerbosity::Verbose, this, TEXT(""));
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

#if ENABLE_COG

    COG_LOG_ABILITY(ELogVerbosity::Verbose, this, TEXT(""));

    FCogDebugPlot::PlotEventStop(this, "Ability", GetFName());

#endif
}

//--------------------------------------------------------------------------------------------------------------------------
float UCogSampleGameplayAbility::GetUnmitigatedCooldownDuration_Implementation() const
{
    return Cooldown.GetValueAtLevel(GetAbilityLevel());
}

//--------------------------------------------------------------------------------------------------------------------------
float UCogSampleGameplayAbility::GetUnmitigatedCost_Implementation() const
{
    return Cost.GetValueAtLevel(GetAbilityLevel());
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleGameplayAbility::CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
    if (ActorInfo == nullptr)
    {
        return false;
    }

    if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(Tag_Status_NoCooldown))
    {
        return true;
    }

    if (Super::CheckCooldown(Handle, ActorInfo, OptionalRelevantTags))
    {
        return true;
    }

    if (OptionalRelevantTags != nullptr)
    {
        OptionalRelevantTags->AddTag(Tag_Ability_Activation_Fail_Cooldown);
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
    if (ActorInfo == nullptr)
    {
        return false;
    }

    if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(Tag_Status_NoCost))
    {
        return true;
    }

    UGameplayEffect* CostEffect = GetCostGameplayEffect();
    if (CostEffect != nullptr)
    {
        //-----------------------------------------------------------------------------------
        // Can always start ability if cost == 0.
        //-----------------------------------------------------------------------------------
        if (IsCostGameplayEffectIsZero(CostEffect, GetAbilityLevel(Handle, ActorInfo), MakeEffectContext(Handle, ActorInfo)))
        {
            return true;
        }

        //-----------------------------------------------------------------------------------
        // Can start ability if it cost stamina and stamina > 0 
        //-----------------------------------------------------------------------------------
        if (CostEffect->GetAssetTags().HasTag(Tag_Effect_Type_Cost_Stamina))
        {
            const float Stamina = ActorInfo->AbilitySystemComponent->GetNumericAttribute(UCogSampleAttributeSet_Caster::GetStaminaAttribute());
            if (Stamina > 0)
            {
                return true;
            }
        }
    }

    //-----------------------------------------------------------------------------------
    // Handle general case
    //-----------------------------------------------------------------------------------
    if (Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
    {
        return true;
    }

    if (OptionalRelevantTags != nullptr)
    {
        OptionalRelevantTags->AddTag(Tag_Ability_Activation_Fail_Cost);
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
const FGameplayTagContainer* UCogSampleGameplayAbility::GetCooldownTags() const
{
    FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&CachedCooldownTags);
    MutableTags->Reset(); 
    const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
    if (ParentTags)
    {
        MutableTags->AppendTags(*ParentTags);
    }
    MutableTags->AddTag(CooldownTag);

    return MutableTags;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
    if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(Tag_Status_NoCooldown))
    {
        return;
    }

    if (GetUnmitigatedCooldownDuration() <= 0.0f)
    {
        return;
    }

    if (UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
    {
        FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownEffect->GetClass(), GetAbilityLevel());
        SpecHandle.Data.Get()->DynamicGrantedTags.AddTag(CooldownTag);
        ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
    if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(Tag_Status_NoCost))
    {
        return;
    }

    if (GetUnmitigatedCost() <= 0.0f)
    {
        return;
    }

    if (UGameplayEffect* CostEffect = GetCostGameplayEffect())
    {
        FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostEffect->GetClass(), GetAbilityLevel());
        ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
    }
}


//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleGameplayAbility::IsCostGameplayEffectIsZero(const UGameplayEffect* GameplayEffect, float Level, const FGameplayEffectContextHandle& EffectContext) const
{
    FGameplayEffectSpec Spec(GameplayEffect, EffectContext, Level);
    Spec.CalculateModifierMagnitudes();

    for (int32 ModIdx = 0; ModIdx < Spec.Modifiers.Num(); ++ModIdx)
    {
        const FGameplayModifierInfo& ModDef = Spec.Def->Modifiers[ModIdx];
        const FModifierSpec& ModSpec = Spec.Modifiers[ModIdx];
        const float CostValue = ModSpec.GetEvaluatedMagnitude();

        //----------------------------------------------------------------------------------------------
        // The Cost in the Data is positive, but UCogSampleModifierCalculation_Cost negates it.
        // Therefore, a cost less than zero is an actual cost, and a cost of 0 or greater can be ignored
        //----------------------------------------------------------------------------------------------
        if (CostValue < 0)
        {
            return false;
        }
    }
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
float UCogSampleGameplayAbility::GetFloatValueAtAbilityLevel(const FScalableFloat& ScalableFloat) const
{
    return UCogSampleFunctionLibrary_Gameplay::GetFloatValue(ScalableFloat, GetAbilityLevel());
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleGameplayAbility::GetBoolValueAtAbilityLevel(const FScalableFloat& ScalableFloat) const
{
    return UCogSampleFunctionLibrary_Gameplay::GetBoolValue(ScalableFloat, GetAbilityLevel());
}

//--------------------------------------------------------------------------------------------------------------------------
int32 UCogSampleGameplayAbility::GetIntValueAtAbilityLevel(const FScalableFloat& ScalableFloat) const
{
    return UCogSampleFunctionLibrary_Gameplay::GetIntValue(ScalableFloat, GetAbilityLevel());
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleGameplayAbility::GetCooldownInfos(float& TimeRemaining, float& CooldownDuration) const
{
    //-------------------------------------------------------------------------------------
    // Provide a default Handle as it is not used in GetCooldownTimeRemainingAndDuration
    //-------------------------------------------------------------------------------------
    FGameplayAbilitySpecHandle Handle;
    GetCooldownTimeRemainingAndDuration(Handle, CurrentActorInfo, TimeRemaining, CooldownDuration);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleGameplayAbility::SetupSpawnPrediction(AActor* Actor, int32 InstanceIndex)
{
    if (Actor == nullptr)
    {
        return;
    }

    UCogSampleSpawnPredictionComponent* SpawnPrediction = Actor->FindComponentByClass<UCogSampleSpawnPredictionComponent>();
    if (SpawnPrediction == nullptr)
    {
        return;
    }

    SetupSpawnPredictionComponent(SpawnPrediction, InstanceIndex);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleGameplayAbility::SetupSpawnPredictionComponent(UCogSampleSpawnPredictionComponent* SpawnPrediction, int32 InstanceIndex)
{
    if (SpawnPrediction == nullptr)
    {
        return;
    }

    SpawnPrediction->SetCreator(CurrentActorInfo->AvatarActor.Get());

    FCogSampleSpawnPredictionKey PredictedActorKey = FCogSampleSpawnPredictionKey::MakeFromAbility(*this, InstanceIndex);
    SpawnPrediction->SetSpawnPredictionKey(PredictedActorKey);

    if (GetCurrentActorInfo()->IsNetAuthority())
    {
        SpawnPrediction->SetRole(ECogSampleSpawnPredictionRole::Server);
    }
    else
    {
        SpawnPrediction->SetRole(ECogSampleSpawnPredictionRole::Predicted);

        if (ACogSamplePlayerController* PlayerController = Cast<ACogSamplePlayerController>(GetCurrentActorInfo()->PlayerController))
        {
            PlayerController->SpawnPredictions.Add(SpawnPrediction);
        }
    }
}