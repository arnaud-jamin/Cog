#include "CogAbilityHelper.h"

#include "CogWindowWidgets.h"
#include "GameplayTagContainer.h"
#include "imgui.h"

//--------------------------------------------------------------------------------------------------------------------------
FString FCogAbilityHelper::CleanupName(FString Str)
{
    Str.RemoveFromStart(TEXT("Default__"));
    Str.RemoveFromEnd(TEXT("_c"));
    return Str;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityHelper::RenderTagContainer(const FGameplayTagContainer& Container, const bool Inline, const ImVec4& Color)
{
    TArray<FGameplayTag> GameplayTags;
    Container.GetGameplayTagArray(GameplayTags);
    for (const FGameplayTag& Tag : GameplayTags)
    {
        FCogWindowWidgets::SmallButton(StringCast<ANSICHAR>(*Tag.ToString()).Get(), Color);
        if (Inline)
        {
            ImGui::SameLine();
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityHelper::RenderTagContainerHighlighted(const FGameplayTagContainer& Container, const FGameplayTagContainer& TagsToHighlight, const bool Inline, const ImVec4& NormalColor, const ImVec4& HighlightColor)
{
    TArray<FGameplayTag> GameplayTags;
    Container.GetGameplayTagArray(GameplayTags);
    for (const FGameplayTag& Tag : GameplayTags)
    {
        const ImVec4 Color = TagsToHighlight.HasTag(Tag) ? HighlightColor : NormalColor;
        FCogWindowWidgets::SmallButton(StringCast<ANSICHAR>(*Tag.ToString()).Get(), Color);
        if (Inline)
        {
            ImGui::SameLine();
        }
    }
}