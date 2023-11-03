#include "CogAbilityConfig_Alignment.h"

#include "AbilitySystemComponent.h"
#include "CogAbilityDataAsset.h"
#include "CogImguiHelper.h"
#include "GameplayEffect.h"

//--------------------------------------------------------------------------------------------------------------------------
FVector4f UCogAbilityConfig_Alignment::GetAttributeColor(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayAttribute& Attribute) const
{
    const float BaseValue = AbilitySystemComponent.GetNumericAttributeBase(Attribute);
    const float CurrentValue = AbilitySystemComponent.GetNumericAttribute(Attribute);

    if (CurrentValue > BaseValue)
    {
        return PositiveColor;
    }

    if (CurrentValue < BaseValue)
    {
        return NegativeColor;
    }

    return NeutralColor;
}

//--------------------------------------------------------------------------------------------------------------------------
FVector4f UCogAbilityConfig_Alignment::GetEffectColor(const UCogAbilityDataAsset* Asset, const UGameplayEffect& Effect) const
{
    const FGameplayTagContainer& Tags = Effect.InheritableGameplayEffectTags.CombinedTags;

    if (Tags.HasTag(Asset->NegativeEffectTag))
    {
        return NegativeColor;
    }

    if (Tags.HasTag(Asset->PositiveEffectTag))
    {
        return PositiveColor;
    }

    return NeutralColor;
}

//--------------------------------------------------------------------------------------------------------------------------
FVector4f UCogAbilityConfig_Alignment::GetEffectModifierColor(const FModifierSpec& ModSpec, const FGameplayModifierInfo& ModInfo, float BaseValue) const
{
    return GetEffectModifierColor(ModSpec.GetEvaluatedMagnitude(), ModInfo.ModifierOp, BaseValue);
}

//--------------------------------------------------------------------------------------------------------------------------
FVector4f UCogAbilityConfig_Alignment::GetEffectModifierColor(float ModifierValue, EGameplayModOp::Type ModifierOp, float BaseValue) const
{
    switch (ModifierOp)
    {
        case EGameplayModOp::Additive:
        {
            if (ModifierValue > 0.0f)
            {
                return PositiveColor;
            }

            if (ModifierValue < 0.0f)
            {
                return NegativeColor;
            }
            break;
        }

        case EGameplayModOp::Multiplicitive:
        {
            if (ModifierValue > 1.0f)
            {
                return PositiveColor;
            }
            else if (ModifierValue < 1.0f)
            {
                return NegativeColor;
            }
            break;
        }

        case EGameplayModOp::Division:
        {
            if (ModifierValue < 1.0f)
            {
                return PositiveColor;
            }

            if (ModifierValue > 1.0f)
            {
                return NegativeColor;
            }
            break;
        }

        case EGameplayModOp::Override:
        {
            if (ModifierValue > BaseValue)
            {
                return PositiveColor;
            }

            if (ModifierValue < BaseValue)
            {
                return NegativeColor;
            }
            break;
        }
    }

    return NeutralColor;
}

