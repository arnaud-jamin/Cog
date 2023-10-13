#include "CogAbilityHelper.h"

#include "GameplayEffect.h"
#include "CogAbilityDataAsset.h"
#include "CogImguiHelper.h"
#include "imgui.h"

//--------------------------------------------------------------------------------------------------------------------------
FString FCogAbilityHelper::CleanupName(FString Str)
{
    Str.RemoveFromStart(TEXT("Default__"));
    Str.RemoveFromEnd(TEXT("_c"));
    return Str;
}


//--------------------------------------------------------------------------------------------------------------------------
ImVec4 FCogAbilityHelper::GetAttributeColor(const UCogAbilityDataAsset* Asset, float BaseValue, float CurrentValue)
{
    FLinearColor Color = FLinearColor::White;
    if (Asset != nullptr)
    {
        if (CurrentValue > BaseValue)
        {
            Color = Asset->PositiveEffectColor;
        }
        else if (CurrentValue < BaseValue)
        {
            Color = Asset->NegativeEffectColor;
        }
        else
        {
            Color = Asset->NeutralEffectColor;
        }
    }

    return FCogImguiHelper::ToImVec4(Color);
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec4 FCogAbilityHelper::GetEffectColor(const UCogAbilityDataAsset* Asset, const UGameplayEffect& Effect)
{
    FLinearColor Color = FLinearColor::White;

    if (Asset != nullptr)
    {
        const FGameplayTagContainer& Tags = Effect.InheritableGameplayEffectTags.CombinedTags;

        if (Tags.HasTag(Asset->NegativeEffectTag))
        {
            Color = Asset->NegativeEffectColor;
        }
        else if (Tags.HasTag(Asset->PositiveEffectTag))
        {
            Color = Asset->PositiveEffectColor;
        }
        else
        {
            Color = Asset->NeutralEffectColor;
        }
    }

    return FCogImguiHelper::ToImVec4(Color);
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec4 FCogAbilityHelper::GetEffectModifierColor(const UCogAbilityDataAsset* Asset, float ModifierValue, EGameplayModOp::Type ModifierOp, float BaseValue)
{
    FLinearColor Color = Asset->NeutralEffectColor;

    switch (ModifierOp)
    {
        case EGameplayModOp::Additive:
        {
            if (ModifierValue > 0.0f)
            {
                Color = Asset->PositiveEffectColor;
            }
            else if (ModifierValue < 0.0f)
            {
                Color = Asset->NegativeEffectColor;
            }
            break;
        }

        case EGameplayModOp::Multiplicitive:
        {
            if (ModifierValue > 1.0f)
            {
                Color = Asset->PositiveEffectColor;
            }
            else if (ModifierValue < 1.0f)
            {
                Color = Asset->NegativeEffectColor;
            }
            break;
        }

        case EGameplayModOp::Division:
        {
            if (ModifierValue < 1.0f)
            {
                Color = Asset->PositiveEffectColor;
            }
            else if (ModifierValue > 1.0f)
            {
                Color = Asset->NegativeEffectColor;
            }
            break;
        }

        case EGameplayModOp::Override:
        {
            if (ModifierValue > BaseValue)
            {
                Color = Asset->PositiveEffectColor;
            }
            else if (ModifierValue < BaseValue)
            {
                Color = Asset->NegativeEffectColor;
            }
            break;
        }
    }

    return FCogImguiHelper::ToImVec4(Color);
}