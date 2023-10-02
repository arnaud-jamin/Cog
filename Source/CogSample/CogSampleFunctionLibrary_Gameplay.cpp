#include "CogSampleFunctionLibrary_Gameplay.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"

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