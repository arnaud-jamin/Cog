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

    UCogWindowConfig_Settings* Settings = GetOwner()->GetSettings();
    RenderLoadLayoutMenuItem(1, Settings->Shortcut_LoadLayout1);
    RenderLoadLayoutMenuItem(2, Settings->Shortcut_LoadLayout2);
    RenderLoadLayoutMenuItem(3, Settings->Shortcut_LoadLayout3);
    RenderLoadLayoutMenuItem(4, Settings->Shortcut_LoadLayout4);

    ImGui::Separator();
    RenderSaveLayoutMenuItem(1, Settings->Shortcut_SaveLayout1);
    RenderSaveLayoutMenuItem(2, Settings->Shortcut_SaveLayout2);
    RenderSaveLayoutMenuItem(3, Settings->Shortcut_SaveLayout3);
    RenderSaveLayoutMenuItem(4, Settings->Shortcut_SaveLayout4);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Layouts::RenderLoadLayoutMenuItem(int InLayoutIndex, const FInputChord& InInputChord)
{
    const auto Shortcut = StringCast<ANSICHAR>(*FCogImguiInputHelper::InputChordToString(InInputChord));
    const auto Text = StringCast<ANSICHAR>(*FString::Printf(TEXT("Load Layout %d"), InLayoutIndex));
    
    if (ImGui::MenuItem(Text.Get(), Shortcut.Get()))
    {
        GetOwner()->LoadLayout(InLayoutIndex + 1);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindow_Layouts::RenderSaveLayoutMenuItem(int InLayoutIndex, const FInputChord& InInputChord)
{
    const auto Shortcut = StringCast<ANSICHAR>(*FCogImguiInputHelper::InputChordToString(InInputChord));
    const auto Text = StringCast<ANSICHAR>(*FString::Printf(TEXT("Save Layout %d"), InLayoutIndex));
    
    if (ImGui::MenuItem(Text.Get(), Shortcut.Get()))
    {
        GetOwner()->SaveLayout(InLayoutIndex + 1);
    }
}