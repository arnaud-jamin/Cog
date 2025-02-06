#include "CogAbilityCheat_Execution_ActivateAbility.h"

#include "CogAbilityDataAsset.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CogAbilityConfig_Alignment.h"
#include "CogCommon.h"
#include "CogImguiHelper.h"
#include "CogWindow.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityCheat_Execution_ActivateAbility::Execute_Implementation(const UObject* WorldContextObject, const AActor* Instigator, const TArray<AActor*>& Targets) const
{
    if (Ability == nullptr)
    { return; }
    
    for (AActor* Target : Targets)
    {
        UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target, true);
        if (AbilitySystem == nullptr)
        {
            COG_LOG_FUNC(LogCogImGui, ELogVerbosity::Warning, TEXT("Target:%s | Invalid Target AbilitySystem"), *GetNameSafe(Target));
            continue;
        }

        FGameplayAbilitySpecHandle Handle;
        const FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromClass(Ability);
        if (Spec != nullptr)
        {
            if (Spec->IsActive())
            {
                AbilitySystem->CancelAbilityHandle(Spec->Handle);
                if (RemoveAbilityOnEnd)
                {
                    AbilitySystem->ClearAbility(Handle);
                }
                return;
            }

            Handle = Spec->Handle;
        }
        else
        {
            Handle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(Ability, 1, INDEX_NONE, Target));
            Spec = AbilitySystem->FindAbilitySpecFromHandle(Handle, EConsiderPending::All);
        }

        AbilitySystem->TryActivateAbility(Handle);

        if (Spec->IsActive() == false)
        {
            if (RemoveAbilityOnEnd)
            {
                AbilitySystem->ClearAbility(Handle);
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
ECogEngineCheat_ActiveState UCogAbilityCheat_Execution_ActivateAbility::IsActiveOnTargets_Implementation(const UObject* WorldContextObject, const TArray<AActor*>& Targets) const
{
    if (Ability == nullptr)
    {
        return ECogEngineCheat_ActiveState::Inactive;
    }

    int32 NumActiveAbilities = 0;
    for (const AActor* Target : Targets)
    {
		const UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target, true);
		if (AbilitySystem == nullptr)
		{ continue; }

        const FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromClass(Ability);
        if (Spec == nullptr)
        { continue; }

        if (Spec->IsActive())
        {
            NumActiveAbilities++;
        }
    }

    if (NumActiveAbilities == 0)
    {
		return ECogEngineCheat_ActiveState::Inactive;
    }

	if (NumActiveAbilities == Targets.Num())
    {
	    return ECogEngineCheat_ActiveState::Active;
    }

    return ECogEngineCheat_ActiveState::Partial;
}


//--------------------------------------------------------------------------------------------------------------------------
bool UCogAbilityCheat_Execution_ActivateAbility::GetColor(const FCogWindow& InCallingWindow, FLinearColor& OutColor) const
{
	if (Ability == nullptr)
	{ return false; }

	const UGameplayAbility* GameplayAbility = Ability->GetDefaultObject<UGameplayAbility>();
    if (GameplayAbility == nullptr)
    { return false; }

	const UCogAbilityConfig_Alignment* Config = InCallingWindow.GetConfig<UCogAbilityConfig_Alignment>();
    const UCogAbilityDataAsset* Asset = InCallingWindow.GetAsset<UCogAbilityDataAsset>();

    return Config->GetAbilityColor(Asset, *GameplayAbility, OutColor);
}
