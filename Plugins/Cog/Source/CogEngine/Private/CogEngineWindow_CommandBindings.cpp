#include "CogEngineWindow_CommandBindings.h"

#include "CogWindowManager.h"
#include "CogWindowWidgets.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "imgui.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CommandBindings::RenderHelp()
{
    ImGui::Text(
        "This window can be used to configure the command bindings. "
        "Bindings are used to trigger console commands from a keyboard shortcuts. "
        );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CommandBindings::RenderContent()
{
    Super::RenderContent();

    const APlayerController* PlayerController = GetLocalPlayerController();
    if (PlayerController == nullptr)
    {
        return;
    }

    UPlayerInput* PlayerInput = PlayerController->PlayerInput;
    if (PlayerInput == nullptr)
    {
        return;
    }

    int32 Index = 0;
    int32 IndexToRemove = INDEX_NONE;

    if (FCogWindowWidgets::ButtonWithTooltip("Add", "Add a new item in the array"))
    {
        PlayerInput->DebugExecBindings.AddDefaulted();
        PlayerInput->SaveConfig();
    }

    ImGui::SameLine();
    if (FCogWindowWidgets::ButtonWithTooltip("Sort", "Sort the array"))
    {
        UCogWindowManager::SortCommands(PlayerInput);
        PlayerInput->SaveConfig();
    }

    ImGui::SameLine();
    if (FCogWindowWidgets::ButtonWithTooltip(
        "Disable Conflicting Commands", 
        "Disable the existing Unreal command shortcuts mapped to same shortcuts Cog is using. Typically, if the F1 shortcut is used to toggle Inputs, the Unreal wireframe command will get disabled."
    ))
    {
        GetOwner()->DisableConflictingCommands();
    }

    for (FKeyBind& KeyBind : PlayerInput->DebugExecBindings)
    {
        ImGui::PushID(Index);

        if (FCogWindowWidgets::DeleteArrayItemButton())
        {
            IndexToRemove = Index;
        }
        
        ImGui::SameLine();

        if (FCogWindowWidgets::KeyBind(KeyBind))
        {
            PlayerInput->SaveConfig();
        }

        ImGui::PopID();
        Index++;
    }

    if (IndexToRemove != INDEX_NONE)
    {
        PlayerInput->DebugExecBindings.RemoveAt(IndexToRemove);
        PlayerInput->SaveConfig();
    }
}
