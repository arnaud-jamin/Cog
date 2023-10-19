#include "CogEngineWindow_CommandBindings.h"

#include "CogWindowWidgets.h"
#include "GameFramework/PlayerInput.h"
#include "imgui.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_CommandBindings::RenderHelp()
{
    ImGui::Text(
        "This window can be used to configure the command bindings. "
        "Bindings are used to trigger console commands from a keyboard shortcuts. "
        );
}

//--------------------------------------------------------------------------------------------------------------------------
UCogEngineWindow_CommandBindings::UCogEngineWindow_CommandBindings()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_CommandBindings::PostInitProperties()
{
    Super::PostInitProperties();

    if (bRegisterDefaultCommands)
    {
        if (RegisterDefaultCommands())
        {
            bRegisterDefaultCommands = false;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_CommandBindings::RenderContent()
{
    Super::RenderContent();

    APlayerController* PlayerController = GetLocalPlayerController();
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
        Sort(PlayerInput);
        PlayerInput->SaveConfig();
    }

    ImGui::SameLine();
    if (FCogWindowWidgets::ButtonWithTooltip(
        "Register Default Commands", 
        "Register the default commands used to control Cog:\n\n"
            "[Tab]  Cog.ToggleInput\n"
            "[F1]   Cog.LoadLayout 1\n"
            "[F2]   Cog.LoadLayout 2\n"
            "[F3]   Cog.LoadLayout 3\n"
            "[F4]   Cog.LoadLayout 4\n"
            "[F5]   Cog.ToggleSelectionMode\n"
    ))
    {
        RegisterDefaultCommands();
    }

    for (FKeyBind& KeyBind : PlayerInput->DebugExecBindings)
    {
        ImGui::PushID(Index);

        if (FCogWindowWidgets::KeyBind(KeyBind))
        {
            PlayerInput->SaveConfig();
        }
        
        ImGui::SameLine();
        if (FCogWindowWidgets::DeleteArrayItemButton())
        {
            IndexToRemove = Index;
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


//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_CommandBindings::Sort(UPlayerInput* PlayerInput)
{
    PlayerInput->DebugExecBindings.Sort([](const FKeyBind& Key1, const FKeyBind& Key2)
    {
        return Key1.Command.Compare(Key2.Command) < 0;
    });
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_CommandBindings::RegisterDefaultCommands()
{
    APlayerController* PlayerController = GetLocalPlayerController();
    if (PlayerController == nullptr)
    {
        return false;
    }

    UPlayerInput* PlayerInput = PlayerController->PlayerInput;
    if (PlayerInput == nullptr)
    {
        return false;
    }

    AddCogCommand(PlayerInput, "Cog.ToggleInput", EKeys::Tab);
    AddCogCommand(PlayerInput, "Cog.LoadLayout 1", EKeys::F1);
    AddCogCommand(PlayerInput, "Cog.LoadLayout 2", EKeys::F2);
    AddCogCommand(PlayerInput, "Cog.LoadLayout 3", EKeys::F3);
    AddCogCommand(PlayerInput, "Cog.LoadLayout 4", EKeys::F4);
    AddCogCommand(PlayerInput, "Cog.ToggleSelectionMode", EKeys::F5);

    Sort(PlayerInput);
    PlayerInput->SaveConfig();

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_CommandBindings::AddCogCommand(UPlayerInput* PlayerInput, const FString& Command, const FKey& Key)
{
    for (FKeyBind& KeyBind : PlayerInput->DebugExecBindings)
    {
        if (KeyBind.Key == Key && KeyBind.Command != Command)
        {
            KeyBind.Control = true;
            KeyBind.bIgnoreCtrl = false;
        }
    }

    FKeyBind* ExistingKeyBind = PlayerInput->DebugExecBindings.FindByPredicate([Command](const FKeyBind& KeyBind){ return KeyBind.Command == Command; });
    if (ExistingKeyBind == nullptr)
    {
        ExistingKeyBind = &PlayerInput->DebugExecBindings.AddDefaulted_GetRef();
    }

    FKeyBind CogKeyBind;
    CogKeyBind.Command = Command;
    CogKeyBind.Control = false;
    CogKeyBind.bIgnoreCtrl = true;
    CogKeyBind.Key = Key;

    *ExistingKeyBind = CogKeyBind;
}