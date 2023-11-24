#include "CogImguiInputHelper.h"

#include "CogImguiModule.h"
#include "Framework/Commands/UICommandInfo.h"
#include "GameFramework/GameUserSettings.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "imgui_internal.h"
#include "InputCoreTypes.h"

#if WITH_EDITOR
#include "Kismet2/DebuggerCommands.h"
#endif //WITH_EDITOR

//--------------------------------------------------------------------------------------------------------------------------
APlayerController* FCogImguiInputHelper::GetFirstLocalPlayerController(const UWorld& World)
{
    for (FConstPlayerControllerIterator Iterator = World.GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PlayerController = Iterator->Get();
        if (PlayerController->IsLocalController())
        {
            return PlayerController;
        }
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
UPlayerInput* FCogImguiInputHelper::GetPlayerInput(const UWorld& World)
{
    APlayerController* PlayerController = GetFirstLocalPlayerController(World);
    if (PlayerController == nullptr)
    {
        return nullptr;
    }

    UPlayerInput* PlayerInput = PlayerController->PlayerInput;
    return PlayerInput;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsKeyEventHandled(UWorld* World, const FKeyEvent& KeyEvent)
{
    //------------------------------------------------------------------------------------------------
    // We want the user to be able to open the console command when imgui has the input.
    //------------------------------------------------------------------------------------------------
    if (IsConsoleEvent(KeyEvent))
    {
        return false;
    }

    //------------------------------------------------------------------------------------------------
    // We want the user to be able to stop its session by pressing Esc, even when imgui has the input
    //------------------------------------------------------------------------------------------------
    if (IsStopPlaySessionEvent(KeyEvent))
    {
        return false;
    }

    //------------------------------------------------------------------------------------------------
    // If we receive a key modifier, we want to let others systems know about it. 
    // Otherwise, the console command bindings that are bound to something like CTRL+Key 
    // won't work, even if we let the KeyEvent pass with 'IsKeyBoundToCommand' below. 
    // It seems the command binings system needs to know about the modifier key press event itself, 
    // and not the Key+Modifier event.
    // We update ImGui modifier keys in SCogImguiWidget::TickKeyModifiers().
    //------------------------------------------------------------------------------------------------
    if (KeyEvent.GetKey().IsModifierKey())
    {
        return false;
    }

    //------------------------------------------------------------------------------------------------
    // We want the user to be able to use command bingings, even when imgui has the input. 
    // We actually use a console command to toggle the input from the game to imgui, and other
    // windows command such as LoadLayout.
    //------------------------------------------------------------------------------------------------
    if (IsKeyBoundToCommand(World, KeyEvent))
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsCheckBoxStateMatchingValue(ECheckBoxState CheckBoxState, bool bValue)
{
    const bool Result = (CheckBoxState == ECheckBoxState::Undetermined) || ((CheckBoxState == ECheckBoxState::Checked) == bValue);
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsKeyEventMatchingKeyInfo(const FKeyEvent& KeyEvent, const FCogImGuiKeyInfo& KeyInfo)
{
    const bool Result = (KeyInfo.Key == KeyEvent.GetKey())
        && IsCheckBoxStateMatchingValue(KeyInfo.Shift, KeyEvent.IsShiftDown())
        && IsCheckBoxStateMatchingValue(KeyInfo.Ctrl, KeyEvent.IsControlDown())
        && IsCheckBoxStateMatchingValue(KeyInfo.Alt, KeyEvent.IsAltDown())
        && IsCheckBoxStateMatchingValue(KeyInfo.Cmd, KeyEvent.IsCommandDown());

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
#define BREAK_CHECKBOX_STATE(CheckBoxState, RequireValue, IgnoreValue)  \
{                                                                       \
    if (CheckBoxState == ECheckBoxState::Checked)                       \
    {                                                                   \
        RequireValue = true;                                            \
        IgnoreValue = false;                                            \
    }                                                                   \
    else if (CheckBoxState == ECheckBoxState::Unchecked)                \
    {                                                                   \
        RequireValue = false;                                           \
        IgnoreValue = true;                                             \
    }                                                                   \
    else if (CheckBoxState == ECheckBoxState::Undetermined)             \
    {                                                                   \
        RequireValue = false;                                           \
        IgnoreValue = false;                                            \
    }                                                                   \
}                                                                       \


//--------------------------------------------------------------------------------------------------------------------------
ECheckBoxState FCogImguiInputHelper::MakeCheckBoxState(uint8 RequireValue, uint8 IgnoreValue)
{
    if (RequireValue)
    {
        return ECheckBoxState::Checked;
    }

    if (IgnoreValue)
    {
        return ECheckBoxState::Unchecked;
    }

    return ECheckBoxState::Undetermined;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiInputHelper::KeyBindToKeyInfo(const FKeyBind& KeyBind, FCogImGuiKeyInfo& KeyInfo)
{
    KeyInfo.Key = KeyBind.Key;
    KeyInfo.Shift = MakeCheckBoxState(KeyBind.Shift, KeyBind.bIgnoreShift);
    KeyInfo.Ctrl = MakeCheckBoxState(KeyBind.Control, KeyBind.bIgnoreCtrl);
    KeyInfo.Alt = MakeCheckBoxState(KeyBind.Alt, KeyBind.bIgnoreAlt);
    KeyInfo.Alt = MakeCheckBoxState(KeyBind.Cmd, KeyBind.bIgnoreCmd);
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiInputHelper::KeyInfoToKeyBind(const FCogImGuiKeyInfo& KeyInfo, FKeyBind& KeyBind)
{
    KeyBind.Key = KeyInfo.Key;
    BREAK_CHECKBOX_STATE(KeyInfo.Shift, KeyBind.Shift, KeyBind.bIgnoreShift);
    BREAK_CHECKBOX_STATE(KeyInfo.Ctrl, KeyBind.Control, KeyBind.bIgnoreCtrl);
    BREAK_CHECKBOX_STATE(KeyInfo.Alt, KeyBind.Alt, KeyBind.bIgnoreAlt);
    BREAK_CHECKBOX_STATE(KeyInfo.Cmd, KeyBind.Cmd, KeyBind.bIgnoreCmd);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsKeyEventMatchingKeyBind(const FKeyEvent& KeyEvent, const FKeyBind& KeyBind)
{
    FCogImGuiKeyInfo KeyInfo;
    KeyBindToKeyInfo(KeyBind, KeyInfo);
    const bool Result = IsKeyEventMatchingKeyInfo(KeyEvent, KeyInfo);
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::WasKeyInfoJustPressed(APlayerController& PlayerController, const FCogImGuiKeyInfo& KeyInfo)
{
    if (PlayerController.WasInputKeyJustPressed(KeyInfo.Key))
    {
        const FModifierKeysState& ModifierKeys = FSlateApplication::Get().GetModifierKeys();

        const bool MatchCtrl    = IsCheckBoxStateMatchingValue(KeyInfo.Ctrl,    ModifierKeys.IsControlDown());
        const bool MatchAlt     = IsCheckBoxStateMatchingValue(KeyInfo.Alt,     ModifierKeys.IsAltDown());
        const bool MatchShift   = IsCheckBoxStateMatchingValue(KeyInfo.Shift,   ModifierKeys.IsShiftDown());
        const bool MatchCmd     = IsCheckBoxStateMatchingValue(KeyInfo.Cmd,     ModifierKeys.IsCommandDown());

        const bool Result = MatchCtrl && MatchAlt && MatchShift && MatchCmd;
        return Result;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsKeyBoundToCommand(UWorld* World, const FKeyEvent& KeyEvent)
{
    if (World == nullptr)
    {
        return false;
    }

    const UPlayerInput* PlayerInput = GetPlayerInput(*World);
    if (PlayerInput == nullptr)
    {
        return false;
    }

    for (const FKeyBind& KeyBind : PlayerInput->DebugExecBindings)
    {
        if (IsKeyEventMatchingKeyBind(KeyEvent, KeyBind))
        {
            return true;
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsConsoleEvent(const FKeyEvent& KeyEvent)
{
    const bool bModifierDown = KeyEvent.IsControlDown() || KeyEvent.IsShiftDown() || KeyEvent.IsAltDown() || KeyEvent.IsCommandDown();
    const bool Result = !bModifierDown && GetDefault<UInputSettings>()->ConsoleKeys.Contains(KeyEvent.GetKey());
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsStopPlaySessionEvent(const FKeyEvent& KeyEvent)
{
#if WITH_EDITOR
    static TSharedPtr<FUICommandInfo> StopPlaySessionCommandInfo = FInputBindingManager::Get().FindCommandInContext("PlayWorld", "StopPlaySession");

    if (StopPlaySessionCommandInfo.IsValid())
    {
        const FInputChord InputChord(KeyEvent.GetKey(), KeyEvent.IsShiftDown(), KeyEvent.IsControlDown(), KeyEvent.IsAltDown(), KeyEvent.IsCommandDown());
        const bool bHasActiveChord = StopPlaySessionCommandInfo->HasActiveChord(InputChord);
        return bHasActiveChord && FPlayWorldCommands::GlobalPlayWorldActions->CanExecuteAction(StopPlaySessionCommandInfo.ToSharedRef());
    }
#endif // WITH_EDITOR

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
uint32 FCogImguiInputHelper::MouseButtonToImGuiMouseButton(const FKey& MouseButton)
{
    if (MouseButton == EKeys::LeftMouseButton)      { return 0; }
    if (MouseButton == EKeys::RightMouseButton)     { return 1; }
    if (MouseButton == EKeys::MiddleMouseButton)    { return 2; }
    if (MouseButton == EKeys::ThumbMouseButton)     { return 3; }
    if (MouseButton == EKeys::ThumbMouseButton2)    { return 4; }

    return -1;
}

//--------------------------------------------------------------------------------------------------------------------------
EMouseCursor::Type FCogImguiInputHelper::ToSlateMouseCursor(ImGuiMouseCursor MouseCursor)
{
    switch (MouseCursor)
    {
    case ImGuiMouseCursor_Arrow:        return EMouseCursor::Default;
    case ImGuiMouseCursor_TextInput:    return EMouseCursor::TextEditBeam;
    case ImGuiMouseCursor_ResizeAll:    return EMouseCursor::CardinalCross;
    case ImGuiMouseCursor_ResizeNS:     return  EMouseCursor::ResizeUpDown;
    case ImGuiMouseCursor_ResizeEW:     return  EMouseCursor::ResizeLeftRight;
    case ImGuiMouseCursor_ResizeNESW:   return  EMouseCursor::ResizeSouthWest;
    case ImGuiMouseCursor_ResizeNWSE:   return  EMouseCursor::ResizeSouthEast;

    case ImGuiMouseCursor_None:
    default:
        return EMouseCursor::None;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogImguiInputHelper::CommandToString(const UWorld& World, const FString& Command)
{
    const UPlayerInput* PlayerInput = GetPlayerInput(World);
    if (PlayerInput == nullptr)
    {
        return FString();
    }

    const FKeyBind* Result = PlayerInput->DebugExecBindings.FindByPredicate([&](const FKeyBind& KeyBind) { return KeyBind.Command == Command; });
    if (Result == nullptr)
    {
        return FString();
    }

    return KeyBindToString(*Result);
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogImguiInputHelper::CommandToString(const UPlayerInput* PlayerInput, const FString& Command)
{
    if (PlayerInput == nullptr)
    {
        return FString();
    }

    const FKeyBind* Result = PlayerInput->DebugExecBindings.FindByPredicate([&](const FKeyBind& KeyBind) { return KeyBind.Command == Command; });
    if (Result == nullptr)
    {
        return FString();
    }

    return KeyBindToString(*Result);
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogImguiInputHelper::KeyBindToString(const FKeyBind& KeyBind)
{
    FString Result;
    if (KeyBind.Alt)
    {
        Result = Result.Append("Alt ");
    }

    if (KeyBind.Shift)
    {
        Result = Result.Append("Shift ");
    }

    if (KeyBind.Control)
    {
        Result = Result.Append("Ctrl ");
    }

    if (KeyBind.Cmd)
    {
        Result = Result.Append("Cmd ");
    }

    Result = Result.Printf(TEXT("[%s]"), *KeyBind.Key.ToString());

    return Result;
}
