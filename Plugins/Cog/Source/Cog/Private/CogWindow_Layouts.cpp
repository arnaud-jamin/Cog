#include "CogWindow_Layouts.h"

#include "CogImguiInputHelper.h"
#include "CogSubsystem.h"
#include "CogWindow_Settings.h"
#include "InputCoreTypes.h"

//--------------------------------------------------------------------------------------------------------------------------
FCogWindow_Layouts::FCogWindow_Layouts()
{
    bShowInMainMenu = false;
    bHasMenu = false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Layouts::RenderContent()
{
    const UPlayerInput* PlayerInput = FCogImguiInputHelper::GetPlayerInput(*GetWorld());
    if (PlayerInput == nullptr)
    {
        return;
    }

    if (ImGui::MenuItem("Reset Window Layout"))
    {
        GetOwner()->ResetLayout();
    }

    ImGui::Separator();
    for (int32 i = 0; i < 4; ++i)
    {
        RenderLoadLayoutMenuItem(PlayerInput, i);
    }

    ImGui::Separator();
    for (int32 i = 0; i < 4; ++i)
    {
        RenderSaveLayoutMenuItem(PlayerInput, i);
    }

}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Layouts::RenderLoadLayoutMenuItem(const UPlayerInput* PlayerInput, int LayoutIndex)
{
    FString Shortcut;
    if (GetOwner()->GetSettings()->LoadLayoutShortcuts.IsValidIndex(LayoutIndex))
    {
        Shortcut = FCogImguiInputHelper::KeyInfoToString(GetOwner()->GetSettings()->LoadLayoutShortcuts[LayoutIndex]);
    }
    
    if (ImGui::MenuItem(TCHAR_TO_ANSI(*FString::Printf(TEXT("Load Layout %d"), LayoutIndex + 1)), TCHAR_TO_ANSI(*Shortcut)))
    {
        GetOwner()->LoadLayout(LayoutIndex + 1);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Layouts::RenderSaveLayoutMenuItem(const UPlayerInput* PlayerInput, int LayoutIndex)
{
    FString Shortcut;
    if (GetOwner()->GetSettings()->LoadLayoutShortcuts.IsValidIndex(LayoutIndex))
    {
        Shortcut = FCogImguiInputHelper::KeyInfoToString(GetOwner()->GetSettings()->SaveLayoutShortcuts[LayoutIndex]);
    }
    
    if (ImGui::MenuItem(TCHAR_TO_ANSI(*FString::Printf(TEXT("Save Layout %d"), LayoutIndex + 1)), TCHAR_TO_ANSI(*Shortcut)))
    {
        GetOwner()->SaveLayout(LayoutIndex + 1);
    }
}