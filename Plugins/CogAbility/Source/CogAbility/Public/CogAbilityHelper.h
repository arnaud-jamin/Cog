#pragma once

#include "CoreMinimal.h"
#include "imgui.h"


class UCogAbilityDataAsset;
class UGameplayEffect;
namespace EGameplayModOp { enum Type : int; }
struct FGameplayTagContainer;

class COGABILITY_API FCogAbilityHelper
{
public:

    static FString CleanupName(FString Str);

    static void RenderTagContainer(const FGameplayTagContainer& Container, const bool Inline = false, const ImVec4& Color = ImVec4(0.4f, 0.4f, 0.4f, 1.0f));

    static void RenderTagContainerHighlighted(const FGameplayTagContainer& Container, const FGameplayTagContainer& TagsToHighlight, const bool Inline = false, const ImVec4& NormalColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f), const ImVec4& HighlightColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

};
