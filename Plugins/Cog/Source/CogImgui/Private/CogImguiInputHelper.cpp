#include "CogImguiInputHelper.h"

#include "CogInputChord.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/UICommandInfo.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "GenericPlatform/GenericApplication.h"
#include "imgui_internal.h"
#include "Input/Events.h"
#include "InputCoreTypes.h"

#if WITH_EDITOR
#include "Kismet2/DebuggerCommands.h"
#endif //WITH_EDITOR

//--------------------------------------------------------------------------------------------------------------------------
 TArray<FCogInputChord> FCogImguiInputHelper::CogPrioritizedShortcuts;

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
bool FCogImguiInputHelper::IsTopPriorityKey(const UPlayerInput& PlayerInput, const FKey& InKey)
{
    FKeyEvent KeyEvent(InKey, FModifierKeysState(), 0, false, 0, 0);
    return IsTopPriorityKeyEvent(PlayerInput, KeyEvent);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsTopPriorityKeyEvent(const UPlayerInput& PlayerInput, const FKeyEvent& InKeyEvent)
{
    //------------------------------------------------------------------------------------------------
    // We want the user to be able to use Cog shortcuts when imgui has the input.
	//------------------------------------------------------------------------------------------------
    for (const FCogInputChord& InputChord : CogPrioritizedShortcuts)
    {
        if (IsInputChordMatchingKeyInfo(InKeyEvent, InputChord))
        { return true; }
    }
        
    //------------------------------------------------------------------------------------------------
    // We want the user to be able to open the console command when imgui has the input.
    //------------------------------------------------------------------------------------------------
    if (IsConsoleEvent(InKeyEvent))
    { return true; }

    //------------------------------------------------------------------------------------------------
    // We want the user to be able to stop its session by pressing Esc, even when imgui has the input
    //------------------------------------------------------------------------------------------------
    if (IsStopPlaySessionEvent(InKeyEvent))
    { return true; }

    //------------------------------------------------------------------------------------------------
    // If we receive a key modifier, we want to let others systems know about it. 
    // Otherwise, the console command bindings that are bound to something like CTRL+Key 
    // won't work, even if we let the KeyEvent pass with 'IsKeyBoundToCommand' below. 
    // It seems the command bindings system needs to know about the modifier key press event itself, 
    // and not the Key+Modifier event.
    // We update ImGui modifier keys in SCogImguiWidget::TickKeyModifiers().
    //------------------------------------------------------------------------------------------------
    if (InKeyEvent.GetKey().IsModifierKey())
    { return true; }

    //------------------------------------------------------------------------------------------------
    // We want the user to be able to use command bindings, even when imgui has the input. 
    //------------------------------------------------------------------------------------------------
    if (IsKeyBoundToCommand(PlayerInput, InKeyEvent))
    { return true; }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsCheckBoxStateMatchingValue(ECheckBoxState CheckBoxState, bool bValue)
{
    const bool Result = (CheckBoxState == ECheckBoxState::Undetermined) || ((CheckBoxState == ECheckBoxState::Checked) == bValue);
    return Result;
}
    
//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsCheckBoxStateMatchingKeyBindModifier(ECheckBoxState InCheckBoxState, bool InRequireModifier, bool InIgnoreModifier)
{
    switch (InCheckBoxState)
    {
        case ECheckBoxState::Undetermined: return true;
        case ECheckBoxState::Checked: return InRequireModifier && InIgnoreModifier == false;
        case ECheckBoxState::Unchecked: return InRequireModifier == false && InIgnoreModifier;
    }
    return false;
}

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
bool FCogImguiInputHelper::IsInputChordMatchingKeyInfo(const FKeyEvent& InKeyEvent, const FInputChord& InInputChord)
{
    const bool Result = (InInputChord.Key == InKeyEvent.GetKey())
        && (InInputChord.bShift == InKeyEvent.IsShiftDown())
        && (InInputChord.bCtrl == InKeyEvent.IsControlDown())
        && (InInputChord.bAlt == InKeyEvent.IsAltDown())
        && (InInputChord.bCmd == InKeyEvent.IsCommandDown());

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsKeyBindMatchingInputChord(const FKeyBind& InKeyBind, const FInputChord& InInputChord)
{
    const bool Result =
        InKeyBind.bDisabled == false
        && (InInputChord.Key == InKeyBind.Key)
        && (InInputChord.bShift == InKeyBind.Shift)
        && (InInputChord.bCtrl == InKeyBind.Control)
        && (InInputChord.bAlt == InKeyBind.Alt)
        && (InInputChord.bCmd == InKeyBind.Cmd);

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsKeyBoundToCommand(const UPlayerInput& PlayerInput, const FKeyEvent& KeyEvent)
{
    for (const FKeyBind& KeyBind : PlayerInput.DebugExecBindings)
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
uint32 FCogImguiInputHelper::ToImGuiMouseButton(const FKey& MouseButton)
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
        case ImGuiMouseCursor_Hand:         return  EMouseCursor::Hand;
        case ImGuiMouseCursor_NotAllowed:   return  EMouseCursor::SlashedCircle;

        case ImGuiMouseCursor_None:
        default:
            return EMouseCursor::None;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogImguiInputHelper::InputChordToString(const FInputChord& InInputChord)
{
    if (InInputChord.Key == FKey())
    {
        return FString("");
    }
    
    FString Result = "[";
    if (InInputChord.bAlt)
    {
        Result += FString("Alt ");
    }

    if (InInputChord.bShift)
    {
        Result += FString("Shift ");
    }

    if (InInputChord.bCtrl)
    {
        Result += FString("Ctrl ");
    }

    if (InInputChord.bCmd)
    {
        Result += FString("Cmd ");
    }

    Result += InInputChord.Key.ToString();
    Result += FString("]");
    
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsKeyEventMatchingKeyBind(const FKeyEvent& KeyEvent, const FKeyBind& KeyBind)
{
    if (KeyBind.bDisabled)
    {
        return false;
    }

    if (KeyBind.Key != KeyEvent.GetKey())
    {
        return false;
    }

    const bool bControlPressed = KeyEvent.IsControlDown();
    const bool bShiftPressed = KeyEvent.IsShiftDown();
    const bool bAltPressed = KeyEvent.IsAltDown();
    const bool bCmdPressed = KeyEvent.IsCommandDown();

    if ((!KeyBind.Control || bControlPressed)
        && (!KeyBind.Shift || bShiftPressed)
        && (!KeyBind.Alt || bAltPressed)
        && (!KeyBind.Cmd || bCmdPressed)
        && (!KeyBind.bIgnoreCtrl || !bControlPressed)
        && (!KeyBind.bIgnoreShift || !bShiftPressed)
        && (!KeyBind.bIgnoreAlt || !bAltPressed)
        && (!KeyBind.bIgnoreCmd || !bCmdPressed))
    {
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsKeyBoundToCommand(const UPlayerInput* InPlayerInput, const FKeyEvent& KeyEvent)
{
    if (InPlayerInput == nullptr)
    {
        return false;
    }

    for (const FKeyBind& KeyBind : InPlayerInput->DebugExecBindings)
    {
        if (IsKeyEventMatchingKeyBind(KeyEvent, KeyBind))
        {
            return true;
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::IsMouseInsideMainViewport()
{
    if (ImGuiViewportP* Viewport = static_cast<ImGuiViewportP*>(ImGui::GetMainViewport()))
    {
        ImGuiIO& IO = ImGui::GetIO();
        const bool Result = Viewport->GetMainRect().Contains(IO.MousePos);
        return Result;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiInputHelper::DisableCommandsConflictingWithShortcuts(UPlayerInput& PlayerInput)
{
    bool HasDisabled = false;
    
    for (const FCogInputChord& Shortcut : CogPrioritizedShortcuts)
    {
        for (FKeyBind& KeyBind : PlayerInput.DebugExecBindings)
        {
            if (IsKeyBindMatchingInputChord(KeyBind, Shortcut))
            {
                KeyBind.bDisabled = true;
                HasDisabled = false;
            }
        }
    }

    if (HasDisabled)
    {
        PlayerInput.SaveConfig();
    }
    
    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
ImGuiKey FCogImguiInputHelper::ToImKey(const FKey& Key)
{
    static const TMap<FKey, ImGuiKey> LookupMap = {
        { EKeys::Tab, ImGuiKey_Tab },

        { EKeys::Left, ImGuiKey_LeftArrow },
        { EKeys::Right, ImGuiKey_RightArrow },
        { EKeys::Up, ImGuiKey_UpArrow },
        { EKeys::Down, ImGuiKey_DownArrow },

        { EKeys::PageUp, ImGuiKey_PageUp },
        { EKeys::PageDown, ImGuiKey_PageDown },
        { EKeys::Home, ImGuiKey_Home },
        { EKeys::End, ImGuiKey_End },
        { EKeys::Insert, ImGuiKey_Insert },
        { EKeys::Delete, ImGuiKey_Delete },

        { EKeys::BackSpace, ImGuiKey_Backspace },
        { EKeys::SpaceBar, ImGuiKey_Space },
        { EKeys::Enter, ImGuiKey_Enter },
        { EKeys::Escape, ImGuiKey_Escape },

        { EKeys::LeftControl, ImGuiKey_LeftCtrl },
        { EKeys::LeftShift, ImGuiKey_LeftShift },
        { EKeys::LeftAlt, ImGuiKey_LeftAlt },
        { EKeys::LeftCommand, ImGuiKey_LeftSuper },
        { EKeys::RightControl, ImGuiKey_RightCtrl },
        { EKeys::RightShift, ImGuiKey_RightShift },
        { EKeys::RightAlt, ImGuiKey_RightAlt },
        { EKeys::RightCommand, ImGuiKey_RightSuper },

        { EKeys::Zero, ImGuiKey_0 },
        { EKeys::One, ImGuiKey_1 },
        { EKeys::Two, ImGuiKey_2 },
        { EKeys::Three, ImGuiKey_3 },
        { EKeys::Four, ImGuiKey_4 },
        { EKeys::Five, ImGuiKey_5 },
        { EKeys::Six, ImGuiKey_6 },
        { EKeys::Seven, ImGuiKey_7 },
        { EKeys::Eight, ImGuiKey_8 },
        { EKeys::Nine, ImGuiKey_9 },

        { EKeys::A, ImGuiKey_A },
        { EKeys::B, ImGuiKey_B },
        { EKeys::C, ImGuiKey_C },
        { EKeys::D, ImGuiKey_D },
        { EKeys::E, ImGuiKey_E },
        { EKeys::F, ImGuiKey_F },
        { EKeys::G, ImGuiKey_G },
        { EKeys::H, ImGuiKey_H },
        { EKeys::I, ImGuiKey_I },
        { EKeys::J, ImGuiKey_J },
        { EKeys::K, ImGuiKey_K },
        { EKeys::L, ImGuiKey_L },
        { EKeys::M, ImGuiKey_M },
        { EKeys::N, ImGuiKey_N },
        { EKeys::O, ImGuiKey_O },
        { EKeys::P, ImGuiKey_P },
        { EKeys::Q, ImGuiKey_Q },
        { EKeys::R, ImGuiKey_R },
        { EKeys::S, ImGuiKey_S },
        { EKeys::T, ImGuiKey_T },
        { EKeys::U, ImGuiKey_U },
        { EKeys::V, ImGuiKey_V },
        { EKeys::W, ImGuiKey_W },
        { EKeys::X, ImGuiKey_X },
        { EKeys::Y, ImGuiKey_Y },
        { EKeys::Z, ImGuiKey_Z },

        { EKeys::F1, ImGuiKey_F1 },
        { EKeys::F2, ImGuiKey_F2 },
        { EKeys::F3, ImGuiKey_F3 },
        { EKeys::F4, ImGuiKey_F4 },
        { EKeys::F5, ImGuiKey_F5 },
        { EKeys::F6, ImGuiKey_F6 },
        { EKeys::F7, ImGuiKey_F7 },
        { EKeys::F8, ImGuiKey_F8 },
        { EKeys::F9, ImGuiKey_F9 },
        { EKeys::F10, ImGuiKey_F10 },
        { EKeys::F11, ImGuiKey_F11 },
        { EKeys::F12, ImGuiKey_F12 },

        { EKeys::Apostrophe, ImGuiKey_Apostrophe },
        { EKeys::Comma, ImGuiKey_Comma },
        { EKeys::Period, ImGuiKey_Period },
        { EKeys::Slash, ImGuiKey_Slash },
        { EKeys::Semicolon, ImGuiKey_Semicolon },
        { EKeys::LeftBracket, ImGuiKey_LeftBracket },
        { EKeys::Backslash, ImGuiKey_Backslash },
        { EKeys::RightBracket, ImGuiKey_RightBracket },

        { EKeys::CapsLock, ImGuiKey_CapsLock },
        { EKeys::ScrollLock, ImGuiKey_ScrollLock },
        { EKeys::NumLock, ImGuiKey_NumLock },
        { EKeys::Pause, ImGuiKey_Pause },

        { EKeys::NumPadZero, ImGuiKey_Keypad0 },
        { EKeys::NumPadOne, ImGuiKey_Keypad1 },
        { EKeys::NumPadTwo, ImGuiKey_Keypad2 },
        { EKeys::NumPadThree, ImGuiKey_Keypad3 },
        { EKeys::NumPadFour, ImGuiKey_Keypad4 },
        { EKeys::NumPadFive, ImGuiKey_Keypad5 },
        { EKeys::NumPadSix, ImGuiKey_Keypad6 },
        { EKeys::NumPadSeven, ImGuiKey_Keypad7 },
        { EKeys::NumPadEight, ImGuiKey_Keypad8 },
        { EKeys::NumPadNine, ImGuiKey_Keypad9 },

        { EKeys::Decimal, ImGuiKey_KeypadDecimal },
        { EKeys::Divide, ImGuiKey_KeypadDivide },
        { EKeys::Multiply, ImGuiKey_KeypadMultiply },
        { EKeys::Subtract, ImGuiKey_KeypadSubtract },
        { EKeys::Add, ImGuiKey_KeypadAdd },
        { EKeys::Equals, ImGuiKey_KeypadEqual },

        { EKeys::Gamepad_Special_Right, ImGuiKey_GamepadStart },
        { EKeys::Gamepad_Special_Left, ImGuiKey_GamepadBack },
        { EKeys::Gamepad_FaceButton_Left, ImGuiKey_GamepadFaceLeft },
        { EKeys::Gamepad_FaceButton_Right, ImGuiKey_GamepadFaceRight },
        { EKeys::Gamepad_FaceButton_Top, ImGuiKey_GamepadFaceUp },
        { EKeys::Gamepad_FaceButton_Bottom, ImGuiKey_GamepadFaceDown },
        { EKeys::Gamepad_DPad_Left, ImGuiKey_GamepadDpadLeft },
        { EKeys::Gamepad_DPad_Right, ImGuiKey_GamepadDpadRight },
        { EKeys::Gamepad_DPad_Up, ImGuiKey_GamepadDpadUp },
        { EKeys::Gamepad_DPad_Down, ImGuiKey_GamepadDpadDown },
        { EKeys::Gamepad_LeftShoulder, ImGuiKey_GamepadL1 },
        { EKeys::Gamepad_RightShoulder, ImGuiKey_GamepadR1 },
        { EKeys::Gamepad_LeftTrigger, ImGuiKey_GamepadL2 },
        { EKeys::Gamepad_RightTrigger, ImGuiKey_GamepadR2 },
        { EKeys::Gamepad_LeftThumbstick, ImGuiKey_GamepadL3 },
        { EKeys::Gamepad_RightThumbstick, ImGuiKey_GamepadR3 },
        { EKeys::Gamepad_LeftStick_Left, ImGuiKey_GamepadLStickLeft },
        { EKeys::Gamepad_LeftStick_Right, ImGuiKey_GamepadLStickRight },
        { EKeys::Gamepad_LeftStick_Up, ImGuiKey_GamepadLStickUp },
        { EKeys::Gamepad_LeftStick_Down, ImGuiKey_GamepadLStickDown },
        { EKeys::Gamepad_RightStick_Left, ImGuiKey_GamepadRStickLeft },
        { EKeys::Gamepad_RightStick_Right, ImGuiKey_GamepadRStickRight },
        { EKeys::Gamepad_RightStick_Up, ImGuiKey_GamepadRStickUp },
        { EKeys::Gamepad_RightStick_Down, ImGuiKey_GamepadRStickDown }
    };

    const ImGuiKey* Result = LookupMap.Find(Key);
    return (Result != nullptr) ? *Result : ImGuiKey_None;
}

