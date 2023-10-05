#include "CogSampleFunctionLibrary_Gameplay.h"

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CogSampleGameplayEffectContext.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameplayCueNotifyTypes.h"
#include "GameplayEffectTypes.h"
#include "Particles/ParticleSystemComponent.h"
#include "ScalableFloat.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleFunctionLibrary_Gameplay::AdjustAttributeForMaxChange(UAbilitySystemComponent* AbilityComponent, FGameplayAttributeData& AffectedAttribute, float OldValue, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
    if (AbilityComponent == nullptr)
    {
        return;
    }

    if (FMath::IsNearlyEqual(OldValue, NewMaxValue))
    {
        return;
    }

    // Change current value to maintain the current Val / Max percent
    const float CurrentValue = AffectedAttribute.GetCurrentValue();
    const float NewDelta = (OldValue > 0.f) ? (CurrentValue * NewMaxValue / OldValue) - CurrentValue : NewMaxValue;

    AbilityComponent->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
}

//--------------------------------------------------------------------------------------------------------------------------
FVector UCogSampleFunctionLibrary_Gameplay::GetActorBottomLocation(const AActor* Actor)
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
bool UCogSampleFunctionLibrary_Gameplay::FindAbilitySpecHandleFromClass(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> AbilityClass, FGameplayAbilitySpecHandle& SpecHandle)
{
    if (const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass))
    {
        SpecHandle = Spec->Handle;
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleFunctionLibrary_Gameplay::ApplyAllGameplayEffectContextValues(const FGameplayCueParameters& Parameters, const FGameplayCueNotify_SpawnResult& SpawnResult)
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
void UCogSampleFunctionLibrary_Gameplay::SendMontageEvent(AActor* Actor, FGameplayTag EventTag, FGameplayEventData Payload)
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

//--------------------------------------------------------------------------------------------------------------------------
float UCogSampleFunctionLibrary_Gameplay::GetFloatValue(const FScalableFloat& ScalableFloat, int32 Level)
{
    return ScalableFloat.GetValueAtLevel(Level);
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleFunctionLibrary_Gameplay::GetBoolValue(const FScalableFloat& ScalableFloat, int32 Level)
{
    return ScalableFloat.GetValueAtLevel(Level) > 0;
}

//--------------------------------------------------------------------------------------------------------------------------
int32 UCogSampleFunctionLibrary_Gameplay::GetIntValue(const FScalableFloat& ScalableFloat, int32 Level)
{
    return (int32)ScalableFloat.GetValueAtLevel(Level);
}

