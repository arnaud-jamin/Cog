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
void FCogAbilityHelper::RenderTagContainer(
    const FGameplayTagContainer& ContainerTags,
    const FGameplayTagContainer& TagsToMatch,
    const bool InverseMatch,
    const bool OnlyShowMatches,
    const bool Inline,
    const ImVec4& NormalColor,
    const ImVec4& MatchColor)
{
    TArray<FGameplayTag> Tags;
    ContainerTags.GetGameplayTagArray(Tags);

    for (const FGameplayTag& Tag : Tags)
    {
        bool hasTag = TagsToMatch.HasTag(Tag);
        hasTag = InverseMatch ? !hasTag : hasTag;

        if (OnlyShowMatches && hasTag == false) {
            continue;
        }

        const ImVec4 Color = hasTag ? MatchColor : NormalColor;
        FCogWindowWidgets::SmallButton(StringCast<ANSICHAR>(*Tag.ToString()).Get(), Color);
        if (Inline)
        {
            ImGui::SameLine();
        }
    }
}