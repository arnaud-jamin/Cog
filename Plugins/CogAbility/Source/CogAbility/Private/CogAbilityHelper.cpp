#include "CogAbilityHelper.h"

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
void FCogAbilityHelper::RenderTagContainer(const FGameplayTagContainer& Container)
{
    TArray<FGameplayTag> GameplayTags;
    Container.GetGameplayTagArray(GameplayTags);
    for (FGameplayTag Tag : GameplayTags)
    {
        ImGui::Text("%s", TCHAR_TO_ANSI(*Tag.ToString()));
    }
}