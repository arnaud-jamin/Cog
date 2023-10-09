#include "CogSampleTask_ListenForGameplayEffectChanges.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleTask_ListenForGameplayEffectChanges* UCogSampleTask_ListenForGameplayEffectChanges::ListenForGameplayEffectChanges(UAbilitySystemComponent* AbilitySystemComponent)
{
    if (!IsValid(AbilitySystemComponent))
    {
        return nullptr;
    }

    UCogSampleTask_ListenForGameplayEffectChanges* WaitForGameplayEffectChangesTask = NewObject<UCogSampleTask_ListenForGameplayEffectChanges>();
    WaitForGameplayEffectChangesTask->AbilitySystemComponent = AbilitySystemComponent;

    return WaitForGameplayEffectChangesTask;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleTask_ListenForGameplayEffectChanges::Activate()
{
    AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UCogSampleTask_ListenForGameplayEffectChanges::OnGameplayEffectAdded);
    AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UCogSampleTask_ListenForGameplayEffectChanges::OnGameplayEffectRemoved);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleTask_ListenForGameplayEffectChanges::EndTask()
{
    if (IsValid(AbilitySystemComponent))
    {
        AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
        AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
    }

    SetReadyToDestroy();
    MarkAsGarbage();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleTask_ListenForGameplayEffectChanges::OnGameplayEffectAdded(UAbilitySystemComponent* InAbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle Handle)
{
    OnAdded.Broadcast(Handle);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleTask_ListenForGameplayEffectChanges::OnGameplayEffectRemoved(const FActiveGameplayEffect& RemovedGameplayEffect)
{
    OnRemoved.Broadcast(RemovedGameplayEffect.Handle);
}