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
TMap<FKey, ImGuiKey> FCogImguiInputHelper::KeyMap;

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
    // Do not handle the input if imgui input is disabled
    //------------------------------------------------------------------------------------------------
    if (FCogImguiModule::Get().GetEnableInput() == false)
    {
        return false;
    }

    //------------------------------------------------------------------------------------------------
    // We want the user to control the character with its gamepad even when imgui has the input.
    //------------------------------------------------------------------------------------------------
    if (KeyEvent.GetKey().IsGamepadKey())
    {
        return false;
    }
    
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
ImGuiKey FCogImguiInputHelper::KeyEventToImGuiKey(const FKeyEvent& KeyEvent)
{
    if (KeyMap.IsEmpty())
    {
        InitializeKeyMap();
    }

    if (const ImGuiKey* Key = KeyMap.Find(KeyEvent.GetKey()))
    {
        return *Key;
    }

    return ImGuiKey_None;
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
void FCogImguiInputHelper::InitializeKeyMap()
{
    KeyMap.Add(EKeys::LeftMouseButton,      ImGuiKey_MouseLeft);
    KeyMap.Add(EKeys::RightMouseButton,     ImGuiKey_MouseRight);
    KeyMap.Add(EKeys::MiddleMouseButton,    ImGuiKey_MouseMiddle);
    KeyMap.Add(EKeys::ThumbMouseButton,     ImGuiKey_MouseX1);
    KeyMap.Add(EKeys::ThumbMouseButton2,    ImGuiKey_MouseX2);

    KeyMap.Add(EKeys::BackSpace,            ImGuiKey_Backspace);
    KeyMap.Add(EKeys::Tab,                  ImGuiKey_Tab);
    KeyMap.Add(EKeys::Enter,                ImGuiKey_Enter);
    KeyMap.Add(EKeys::Pause,                ImGuiKey_Pause);

    KeyMap.Add(EKeys::CapsLock,             ImGuiKey_CapsLock);
    KeyMap.Add(EKeys::Escape,               ImGuiKey_Escape);
    KeyMap.Add(EKeys::SpaceBar,             ImGuiKey_Space);
    KeyMap.Add(EKeys::PageUp,               ImGuiKey_PageUp);
    KeyMap.Add(EKeys::PageDown,             ImGuiKey_PageDown);
    KeyMap.Add(EKeys::End,                  ImGuiKey_End);
    KeyMap.Add(EKeys::Home,                 ImGuiKey_Home);

    KeyMap.Add(EKeys::Left,                 ImGuiKey_LeftArrow);
    KeyMap.Add(EKeys::Up,                   ImGuiKey_UpArrow);
    KeyMap.Add(EKeys::Right,                ImGuiKey_RightArrow);
    KeyMap.Add(EKeys::Down,                 ImGuiKey_DownArrow);

    KeyMap.Add(EKeys::Insert,               ImGuiKey_Insert);
    KeyMap.Add(EKeys::Delete,               ImGuiKey_Delete);

    KeyMap.Add(EKeys::Zero,                 ImGuiKey_0);
    KeyMap.Add(EKeys::One,                  ImGuiKey_1);
    KeyMap.Add(EKeys::Two,                  ImGuiKey_2);
    KeyMap.Add(EKeys::Three,                ImGuiKey_3);
    KeyMap.Add(EKeys::Four,                 ImGuiKey_4);
    KeyMap.Add(EKeys::Five,                 ImGuiKey_5);
    KeyMap.Add(EKeys::Six,                  ImGuiKey_6);
    KeyMap.Add(EKeys::Seven,                ImGuiKey_7);
    KeyMap.Add(EKeys::Eight,                ImGuiKey_8);
    KeyMap.Add(EKeys::Nine,                 ImGuiKey_9);

    KeyMap.Add(EKeys::A,                    ImGuiKey_A);
    KeyMap.Add(EKeys::B,                    ImGuiKey_B);
    KeyMap.Add(EKeys::C,                    ImGuiKey_C);
    KeyMap.Add(EKeys::D,                    ImGuiKey_D);
    KeyMap.Add(EKeys::E,                    ImGuiKey_E);
    KeyMap.Add(EKeys::F,                    ImGuiKey_F);
    KeyMap.Add(EKeys::G,                    ImGuiKey_G);
    KeyMap.Add(EKeys::H,                    ImGuiKey_H);
    KeyMap.Add(EKeys::I,                    ImGuiKey_I);
    KeyMap.Add(EKeys::J,                    ImGuiKey_J);
    KeyMap.Add(EKeys::K,                    ImGuiKey_K);
    KeyMap.Add(EKeys::L,                    ImGuiKey_L);
    KeyMap.Add(EKeys::M,                    ImGuiKey_M);
    KeyMap.Add(EKeys::N,                    ImGuiKey_N);
    KeyMap.Add(EKeys::O,                    ImGuiKey_O);
    KeyMap.Add(EKeys::P,                    ImGuiKey_P);
    KeyMap.Add(EKeys::Q,                    ImGuiKey_Q);
    KeyMap.Add(EKeys::R,                    ImGuiKey_R);
    KeyMap.Add(EKeys::S,                    ImGuiKey_S);
    KeyMap.Add(EKeys::T,                    ImGuiKey_T);
    KeyMap.Add(EKeys::U,                    ImGuiKey_U);
    KeyMap.Add(EKeys::V,                    ImGuiKey_V);
    KeyMap.Add(EKeys::W,                    ImGuiKey_W);
    KeyMap.Add(EKeys::X,                    ImGuiKey_X);
    KeyMap.Add(EKeys::Y,                    ImGuiKey_Y);
    KeyMap.Add(EKeys::Z,                    ImGuiKey_Z);

    KeyMap.Add(EKeys::NumPadZero,           ImGuiKey_Keypad0);
    KeyMap.Add(EKeys::NumPadOne,            ImGuiKey_Keypad1);
    KeyMap.Add(EKeys::NumPadTwo,            ImGuiKey_Keypad2);
    KeyMap.Add(EKeys::NumPadThree,          ImGuiKey_Keypad3);
    KeyMap.Add(EKeys::NumPadFour,           ImGuiKey_Keypad4);
    KeyMap.Add(EKeys::NumPadFive,           ImGuiKey_Keypad5);
    KeyMap.Add(EKeys::NumPadSix,            ImGuiKey_Keypad6);
    KeyMap.Add(EKeys::NumPadSeven,          ImGuiKey_Keypad7);
    KeyMap.Add(EKeys::NumPadEight,          ImGuiKey_Keypad8);
    KeyMap.Add(EKeys::NumPadNine,           ImGuiKey_Keypad9);

    KeyMap.Add(EKeys::Multiply,             ImGuiKey_KeypadMultiply);
    KeyMap.Add(EKeys::Add,                  ImGuiKey_KeypadAdd);
    KeyMap.Add(EKeys::Subtract,             ImGuiKey_KeypadSubtract);
    KeyMap.Add(EKeys::Decimal,              ImGuiKey_KeypadDecimal);
    KeyMap.Add(EKeys::Divide,               ImGuiKey_KeypadDivide);

    KeyMap.Add(EKeys::F1,                   ImGuiKey_F1);
    KeyMap.Add(EKeys::F2,                   ImGuiKey_F2);
    KeyMap.Add(EKeys::F3,                   ImGuiKey_F3);
    KeyMap.Add(EKeys::F4,                   ImGuiKey_F4);
    KeyMap.Add(EKeys::F5,                   ImGuiKey_F5);
    KeyMap.Add(EKeys::F6,                   ImGuiKey_F6);
    KeyMap.Add(EKeys::F7,                   ImGuiKey_F7);
    KeyMap.Add(EKeys::F8,                   ImGuiKey_F8);
    KeyMap.Add(EKeys::F9,                   ImGuiKey_F9);
    KeyMap.Add(EKeys::F10,                  ImGuiKey_F10);
    KeyMap.Add(EKeys::F11,                  ImGuiKey_F11);
    KeyMap.Add(EKeys::F12,                  ImGuiKey_F12);

    KeyMap.Add(EKeys::NumLock,              ImGuiKey_NumLock);

    KeyMap.Add(EKeys::ScrollLock,           ImGuiKey_ScrollLock);

    KeyMap.Add(EKeys::LeftShift,            ImGuiKey_LeftShift);
    KeyMap.Add(EKeys::RightShift,           ImGuiKey_RightShift);
    KeyMap.Add(EKeys::LeftControl,          ImGuiKey_LeftCtrl);
    KeyMap.Add(EKeys::RightControl,         ImGuiKey_RightCtrl);
    KeyMap.Add(EKeys::LeftAlt,              ImGuiKey_LeftAlt);
    KeyMap.Add(EKeys::RightAlt,             ImGuiKey_RightAlt);
    KeyMap.Add(EKeys::LeftCommand,          ImGuiKey_LeftSuper);
    KeyMap.Add(EKeys::RightCommand,         ImGuiKey_RightSuper);

    KeyMap.Add(EKeys::Semicolon,            ImGuiKey_Semicolon);
    KeyMap.Add(EKeys::Equals,               ImGuiKey_Equal);
    KeyMap.Add(EKeys::Comma,                ImGuiKey_Comma);
    KeyMap.Add(EKeys::Hyphen,               ImGuiKey_Minus);
    KeyMap.Add(EKeys::Period,               ImGuiKey_Period);
    KeyMap.Add(EKeys::Slash,                ImGuiKey_Slash);

    KeyMap.Add(EKeys::LeftBracket,          ImGuiKey_LeftBracket);
    KeyMap.Add(EKeys::Backslash,            ImGuiKey_Backslash);
    KeyMap.Add(EKeys::RightBracket,         ImGuiKey_RightBracket);
    KeyMap.Add(EKeys::Apostrophe,           ImGuiKey_Apostrophe);

    //KeyMap.Add(EKeys::MouseX,            ImGuiKey_None;
    //KeyMap.Add(EKeys::MouseY,            ImGuiKey_None;
    //KeyMap.Add(EKeys::Mouse2D,           ImGuiKey_None;
    //KeyMap.Add(EKeys::MouseScrollUp,     ImGuiKey_None;
    //KeyMap.Add(EKeys::MouseScrollDown,   ImGuiKey_None;
    //KeyMap.Add(EKeys::MouseWheelAxis,    ImGuiKey_None;
    //KeyMap.Add(EKeys::Underscore,        ImGuiKey_None;
    //KeyMap.Add(EKeys::Tilde,             ImGuiKey_None;
    //KeyMap.Add(EKeys::Ampersand,         ImGuiKey_None;
    //KeyMap.Add(EKeys::Asterix,           ImGuiKey_None;
    //KeyMap.Add(EKeys::Caret,             ImGuiKey_None;
    //KeyMap.Add(EKeys::Colon,             ImGuiKey_None;
    //KeyMap.Add(EKeys::Dollar,            ImGuiKey_None;
    //KeyMap.Add(EKeys::Exclamation,       ImGuiKey_None;
    //KeyMap.Add(EKeys::LeftParantheses,   ImGuiKey_None;
    //KeyMap.Add(EKeys::RightParantheses,  ImGuiKey_None;
    //KeyMap.Add(EKeys::Quote,             ImGuiKey_None;
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
