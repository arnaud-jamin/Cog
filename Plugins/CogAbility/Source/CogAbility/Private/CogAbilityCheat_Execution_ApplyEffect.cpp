#include "CogAbilityCheat_Execution_ApplyEffect.h"

#include "CogAbilityDataAsset.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CogAbilityConfig_Alignment.h"
#include "CogImguiHelper.h"
#include "CogWindow.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityCheat_Execution_ApplyEffect::Execute_Implementation(const UObject* WorldContextObject, const AActor* Instigator, const TArray<AActor*>& Targets) const
{
    UAbilitySystemComponent* DefaultInstigatorAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Instigator, true);

    for (AActor* Target : Targets)
    {
        UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target, true);
        if (TargetAbilitySystem == nullptr)
        {
            UE_LOG(LogCogImGui, Warning, TEXT("ACogAbilityReplicator::Server_ApplyCheat_Implementation | Target:%s | Invalid Target AbilitySystem"), *GetNameSafe(Target));
            continue;
        }

        if (TargetAbilitySystem->GetGameplayEffectCount(Effect, nullptr) > 0)
        {
            TargetAbilitySystem->RemoveActiveGameplayEffectBySourceEffect(Effect, nullptr);
        }
        else
        {
            //-----------------------------------------------------------------------------------
            // When executing a cheat directly on the game server, there is not an obvious
            // local player to use as the instigator. Instead, we use the target ability system.
            //-----------------------------------------------------------------------------------
            UAbilitySystemComponent* InstigatorAbilitySystem = (DefaultInstigatorAbilitySystem != nullptr) ? DefaultInstigatorAbilitySystem : TargetAbilitySystem;

            FGameplayEffectContextHandle ContextHandle = InstigatorAbilitySystem->MakeEffectContext();
            ContextHandle.AddSourceObject(InstigatorAbilitySystem);
            FGameplayEffectSpecHandle SpecHandle = InstigatorAbilitySystem->MakeOutgoingSpec(Effect, 1, ContextHandle);

            if (const FGameplayEffectSpec* EffectSpec = SpecHandle.Data.Get())
            {
                FHitResult HitResult;
                HitResult.HitObjectHandle = FActorInstanceHandle(Target);
                HitResult.Normal = FVector::ForwardVector;
                HitResult.ImpactNormal = FVector::ForwardVector;
                HitResult.Location = Target->GetActorLocation();
                HitResult.ImpactPoint = Target->GetActorLocation();
                HitResult.PhysMaterial = nullptr;
                ContextHandle.AddHitResult(HitResult, true);

                InstigatorAbilitySystem->ApplyGameplayEffectSpecToTarget(*EffectSpec, TargetAbilitySystem);
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
ECogEngineCheat_ActiveState UCogAbilityCheat_Execution_ApplyEffect::IsActiveOnTargets_Implementation(const UObject* WorldContextObject, const TArray<AActor*>& Targets) const
{
    if (Effect == nullptr)
    {
        return ECogEngineCheat_ActiveState::Inactive;
    }

    int32 NumWithEffect = 0;
    for (const AActor* Target : Targets)
    {
		const UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target, true);
		if (AbilitySystem == nullptr)
		{
	        continue;
	    }

        const int32 Count = AbilitySystem->GetGameplayEffectCount(Effect, nullptr);
        if (Count > 0)
        {
            NumWithEffect++;
        }
    }

    if (NumWithEffect == 0)
    {
		return ECogEngineCheat_ActiveState::Inactive;
    }

	if (NumWithEffect == Targets.Num())
    {
	    return ECogEngineCheat_ActiveState::Active;
    }

    return ECogEngineCheat_ActiveState::Partial;
}


//--------------------------------------------------------------------------------------------------------------------------
bool UCogAbilityCheat_Execution_ApplyEffect::GetColor(const FCogWindow& InCallingWindow, FLinearColor& OutColor) const
{
	if (Effect == nullptr)
	{ return false; }

	const UGameplayEffect* GameplayEffect = Effect->GetDefaultObject<UGameplayEffect>();
    if (GameplayEffect == nullptr)
    { return false; }

	const UCogAbilityConfig_Alignment* Config = InCallingWindow.GetConfig<UCogAbilityConfig_Alignment>();
    const UCogAbilityDataAsset* Asset = InCallingWindow.GetAsset<UCogAbilityDataAsset>();

    return Config->GetEffectColor(Asset, *GameplayEffect, OutColor);
}
