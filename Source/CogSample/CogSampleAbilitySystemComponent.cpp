#include "CogSampleAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "GameplayEffectTypes.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAbilitySystemComponent::ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters) const
{
    if (AActor* ActorAvatar = AbilityActorInfo->AvatarActor.Get())
    {
        UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(ActorAvatar, GameplayCueTag, EGameplayCueEvent::Type::Executed, GameplayCueParameters);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAbilitySystemComponent::AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters) const
{
    if (AActor* ActorAvatar = AbilityActorInfo->AvatarActor.Get())
    {
        UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(ActorAvatar, GameplayCueTag, EGameplayCueEvent::Type::OnActive, GameplayCueParameters);
        UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(ActorAvatar, GameplayCueTag, EGameplayCueEvent::Type::WhileActive, GameplayCueParameters);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAbilitySystemComponent::RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters) const
{
    if (AActor* ActorAvatar = AbilityActorInfo->AvatarActor.Get())
    {
        UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(ActorAvatar, GameplayCueTag, EGameplayCueEvent::Type::Removed, GameplayCueParameters);
    }
}
