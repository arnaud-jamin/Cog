#include "CogSampleGameplayLibrary.h"

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CogSampleGameplayEffectContext.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameplayCueNotifyTypes.h"
#include "Particles/ParticleSystemComponent.h"

//--------------------------------------------------------------------------------------------------------------------------
FVector UCogSampleGameplayLibrary::GetActorBottomLocation(const AActor* Actor)
{
    const FVector Location = Actor->GetActorLocation();

    if (const ACharacter* Character = Cast<ACharacter>(Actor))
    {
        const UCapsuleComponent* CapsuleComponent = Character->GetCapsuleComponent();
        const FVector BottomLocation = Location - FVector::UpVector * CapsuleComponent->GetScaledCapsuleHalfHeight();
        return BottomLocation;
    }

    return Location;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleGameplayLibrary::FindAbilitySpecHandleFromClass(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> AbilityClass, FGameplayAbilitySpecHandle& SpecHandle)
{
    if (const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass))
    {
        SpecHandle = Spec->Handle;
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleGameplayLibrary::ApplyAllGameplayEffectContextValues(const FGameplayCueParameters& Parameters, const FGameplayCueNotify_SpawnResult& SpawnResult)
{
    TArray<FCogSampleGameplayEffectContextFloatValue> Entries;
    UCogSampleEffectContextLibrary::EffectContextGetAllFloatValues(Parameters.EffectContext, Entries);

    for (TObjectPtr<UFXSystemComponent> FXSystemComponent : SpawnResult.FxSystemComponents)
    {
        if (UFXSystemComponent* FXSystemComponentPtr = FXSystemComponent.Get())
        {
            for (const FCogSampleGameplayEffectContextFloatValue& Entry : Entries)
            {
                FXSystemComponentPtr->SetFloatParameter(Entry.Name, Entry.Value);
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleGameplayLibrary::SendMontageEvent(AActor* Actor, FGameplayTag EventTag, FGameplayEventData Payload)
{
    if (::IsValid(Actor))
    {
        UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
        if (AbilitySystemComponent != nullptr && IsValidChecked(AbilitySystemComponent))
        {
            FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
            AbilitySystemComponent->HandleGameplayEvent(EventTag, &Payload);
        }
    }
}