#include "CogImguiInputProcessor.h"

#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "CogImguiWidget.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"
#include "imgui.h"
#include "Slate/SGameLayerManager.h"

#if WITH_EDITOR
#include "Kismet2/DebuggerCommands.h"
#endif //WITH_EDITOR

constexpr bool ForwardEvent = false;
constexpr bool TerminateEvent = true;

//--------------------------------------------------------------------------------------------------------------------------
FImGuiInputProcessor::FImGuiInputProcessor(UPlayerInput* InPlayerInput, SCogImguiWidget* InMainWidget)
{
    PlayerInput = InPlayerInput;
    MainWidget = InMainWidget;
}

//--------------------------------------------------------------------------------------------------------------------------
static FVector2D TransformScreenPointToImGui(const FGeometry& MyGeometry, const FVector2D& Point)
{
    const FSlateRenderTransform ImGuiToScreen = MyGeometry.GetAccumulatedRenderTransform();
    return ImGuiToScreen.Inverse().TransformPoint(Point);
}

//--------------------------------------------------------------------------------------------------------------------------
void FImGuiInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> SlateCursor)
{
    ImGuiIO& IO = ImGui::GetIO();

    const bool bHasGamepad = (IO.BackendFlags & ImGuiBackendFlags_HasGamepad);
    if (bHasGamepad != SlateApp.IsGamepadAttached())
    {
        IO.BackendFlags ^= ImGuiBackendFlags_HasGamepad;
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("FImGuiInputProcessor::Tick | HasGamePad Changed"));
    }

    AddMousePosEvent(SlateApp.GetCursorPos());

    if ((IO.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0)
    {
        SlateCursor->SetType(FCogImguiInputHelper::ToSlateMouseCursor(ImGui::GetMouseCursor()));
    }

    if (IO.WantSetMousePos)
    {
        SlateApp.SetCursorPos(FCogImguiHelper::ToVector2D(IO.MousePos));
        //UE_LOG(LogCogImGui, VeryVerbose, TEXT("FImGuiInputProcessor::Tick | SetCursorPos"));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FImGuiInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& Event)
{
    return HandleKeyEvent(SlateApp, Event, true);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FImGuiInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& Event)
{
    return HandleKeyEvent(SlateApp, Event, false);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FImGuiInputProcessor::HandleKeyEvent(FSlateApplication& SlateApp, const FKeyEvent& Event, bool IsKeyDown)
{
    //------------------------------------------------------------------------------------------------
    // We want the user to be able to open the console command when imgui has the input.
    //------------------------------------------------------------------------------------------------
    if (IsConsoleEvent(Event))
    {
        const bool Result = ForwardEvent;
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("FImGuiInputProcessor::HandleKeyEvent | Key:%s | IsKeyDown:%d | TerminateEvent:%d | ConsoleEvent"), *Event.GetKey().ToString(), IsKeyDown, Result);
        return Result;
    }

    //------------------------------------------------------------------------------------------------
    // We want the user to be able to stop its session by pressing Esc, even when imgui has the input
    //------------------------------------------------------------------------------------------------
    if (IsStopPlaySessionEvent(Event))
    {
        const bool Result = ForwardEvent;
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("FImGuiInputProcessor::HandleKeyEvent | Key:%s | IsKeyDown:%d | TerminateEvent:%d | StopPlaySessionEvent"), *Event.GetKey().ToString(), IsKeyDown, Result);
        return Result;
    }

    //------------------------------------------------------------------------------------------------
    // We want the user to be able to use command bingings, even when imgui has the input. 
    // We actually use a console command to toggle the input from the game to imgui, and other
    // windows command such as LoadLayout.
    //------------------------------------------------------------------------------------------------
    if (IsKeyBoundToCommand(Event))
    {
        const bool Result = ForwardEvent;
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("FImGuiInputProcessor::HandleKeyEvent | Key:%s | IsKeyDown:%d | TerminateEvent:%d | KeyBoundToCommand"), *Event.GetKey().ToString(), IsKeyDown, Result);
        return Result;
    }

    ImGuiIO& IO = ImGui::GetIO();

    IO.AddKeyEvent(ToImKey(Event.GetKey()), IsKeyDown);

    const FModifierKeysState& ModifierKeys = Event.GetModifierKeys();
    IO.AddKeyEvent(ImGuiMod_Ctrl, ModifierKeys.IsControlDown());
    IO.AddKeyEvent(ImGuiMod_Shift, ModifierKeys.IsShiftDown());
    IO.AddKeyEvent(ImGuiMod_Alt, ModifierKeys.IsAltDown());
    IO.AddKeyEvent(ImGuiMod_Super, ModifierKeys.IsCommandDown());

    //------------------------------------------------------------------------------------------------
    // If we receive a key modifier, we want to let others systems know about it. 
    // Otherwise, the console command bindings that are bound to something like CTRL+Key 
    // won't work, even if we let the KeyEvent pass with 'IsKeyBoundToCommand' below. 
    // It seems the command binings system needs to know about the modifier key press event itself, 
    // and not the Key+Modifier event.
    //------------------------------------------------------------------------------------------------
    const bool IsModifierKey = Event.GetKey().IsModifierKey();
    if (IsModifierKey)
    {
        const bool Result = ForwardEvent;
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("FImGuiInputProcessor::HandleKeyEvent | Key:%s | IsKeyDown:%d | TerminateEvent:%d | IsModifierKey"), *Event.GetKey().ToString(), IsKeyDown, Result);
        return Result;
    }

    if (Event.GetKey().IsGamepadKey())
    {
        if (IO.WantCaptureKeyboard && (IO.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad))
        {
            const bool Result = TerminateEvent;
            UE_LOG(LogCogImGui, VeryVerbose, TEXT("FImGuiInputProcessor::HandleKeyEvent | Key:%s | IsKeyDown:%d | TerminateEvent:%d | NavEnableGamepad"), *Event.GetKey().ToString(), IsKeyDown, Result);
            return Result;
        }
    }

    const bool Result = IO.WantCaptureKeyboard ? TerminateEvent : ForwardEvent;
    UE_LOG(LogCogImGui, VeryVerbose, TEXT("FImGuiInputProcessor::HandleKeyEvent | Key:%s | IsKeyDown:%d | WantCaptureKeyboard:%d | TerminateEvent:%d"), *Event.GetKey().ToString(), IsKeyDown, IO.WantCaptureKeyboard, Result);
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FImGuiInputProcessor::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& Event)
{
    const float Value = Event.GetAnalogValue();

    ImGuiIO& IO = ImGui::GetIO();
    IO.AddKeyAnalogEvent(ToImKey(Event.GetKey()), FMath::Abs(Value) > 0.0f, Value);

    if (Event.GetKey().IsGamepadKey())
    {
        if (IO.WantCaptureKeyboard && (IO.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad))
        {
            return TerminateEvent;
        }

        return ForwardEvent;
    }
    else
    {
        if (IO.WantCaptureKeyboard)
        {
            return TerminateEvent;
        }
        
        return ForwardEvent;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FImGuiInputProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& Event)
{
    AddMousePosEvent(Event.GetScreenSpacePosition());

    if (MainWidget->GetEnableInput() && MainWidget->GetShareMouse() == false)
    {
        return TerminateEvent;
    }

    ImGuiIO& IO = ImGui::GetIO();
    const bool Result = IO.WantCaptureMouse ? TerminateEvent : ForwardEvent;
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FImGuiInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& Event)
{
    return HandleMouseButtonEvent(SlateApp, Event, true);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FImGuiInputProcessor::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& Event)
{
    return HandleMouseButtonEvent(SlateApp, Event, false);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FImGuiInputProcessor::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& Event)
{
    return HandleMouseButtonEvent(SlateApp, Event, true);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FImGuiInputProcessor::HandleMouseButtonEvent(FSlateApplication& SlateApp, const FPointerEvent& Event, bool IsButtonDown)
{
    ImGuiIO& IO = ImGui::GetIO();

    const uint32 Button = ToImGuiMouseButton(Event.GetEffectingButton());
    IO.AddMouseButtonEvent(Button, IsButtonDown);

    if (MainWidget->GetEnableInput() && MainWidget->GetShareMouse() == false)
    {
        const bool Result = TerminateEvent;
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("FImGuiInputProcessor::HandleMouseButtonEvent | Button:%d | IsButtonDown:%d | WantCaptureMouse:%d | TerminateEvent:%d | ShareMouse == false"), Button, IsButtonDown, IO.WantCaptureMouse, Result);
        return Result;
    }

    const bool Result = IO.WantCaptureMouse ? TerminateEvent : ForwardEvent;
    UE_LOG(LogCogImGui, VeryVerbose, TEXT("FImGuiInputProcessor::HandleMouseButtonEvent | Button:%d | IsButtonDown:%d | WantCaptureMouse:%d | TerminateEvent:%d"), Button, IsButtonDown, IO.WantCaptureMouse, Result);
    return Result;
}
//--------------------------------------------------------------------------------------------------------------------------
void FImGuiInputProcessor::AddMousePosEvent(const FVector2D& MousePosition) const
{
    ImGuiIO& IO = ImGui::GetIO();
    if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        IO.AddMousePosEvent(MousePosition.X, MousePosition.Y);
    }
    else
    {
        const FVector2D TransformedMousePosition = MousePosition - MainWidget->GetTickSpaceGeometry().GetAbsolutePosition();
        IO.AddMousePosEvent(TransformedMousePosition.X, TransformedMousePosition.Y);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FImGuiInputProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& Event, const FPointerEvent* GestureEvent)
{
    ImGuiIO& IO = ImGui::GetIO();

    IO.AddMouseWheelEvent(0.0f, Event.GetWheelDelta());

    return IO.WantCaptureMouse;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FImGuiInputProcessor::IsKeyEventMatchingKeyBind(const FKeyEvent& KeyEvent, const FKeyBind& KeyBind)
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
bool FImGuiInputProcessor::IsKeyBoundToCommand(const FKeyEvent& KeyEvent)
{
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
bool FImGuiInputProcessor::IsConsoleEvent(const FKeyEvent& KeyEvent)
{
    const bool bModifierDown = KeyEvent.IsControlDown() || KeyEvent.IsShiftDown() || KeyEvent.IsAltDown() || KeyEvent.IsCommandDown();
    const bool Result = !bModifierDown && GetDefault<UInputSettings>()->ConsoleKeys.Contains(KeyEvent.GetKey());
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FImGuiInputProcessor::IsStopPlaySessionEvent(const FKeyEvent& KeyEvent)
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
uint32 FImGuiInputProcessor::ToImGuiMouseButton(const FKey& MouseButton)
{
    if (MouseButton == EKeys::LeftMouseButton) { return 0; }
    if (MouseButton == EKeys::RightMouseButton) { return 1; }
    if (MouseButton == EKeys::MiddleMouseButton) { return 2; }
    if (MouseButton == EKeys::ThumbMouseButton) { return 3; }
    if (MouseButton == EKeys::ThumbMouseButton2) { return 4; }

    return -1;
}

//--------------------------------------------------------------------------------------------------------------------------
ImGuiKey FImGuiInputProcessor::ToImKey(const FKey& Key)
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
