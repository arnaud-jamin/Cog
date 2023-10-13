#pragma once

#include "CoreMinimal.h"
#include "imgui.h"

class UCogAbilityDataAsset;
class UGameplayEffect;
namespace EGameplayModOp { enum Type; }

class COGABILITY_API FCogAbilityHelper
{
public:

    static FString CleanupName(FString Str);

    static ImVec4 GetAttributeColor(const UCogAbilityDataAsset* Asset, float BaseValue, float CurrentValue);

    static ImVec4 GetEffectColor(const UCogAbilityDataAsset* Asset, const UGameplayEffect& Effect);

    static ImVec4 GetEffectModifierColor(const UCogAbilityDataAsset* Asset, float ModifierValue, EGameplayModOp::Type ModifierOp, float BaseValue);
};
