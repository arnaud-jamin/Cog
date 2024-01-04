#include "CogImGuiInputProcessor.h"

#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "CogImguiWidget.h"
#include "CogImguiContext.h"
#include "imgui.h"
#include "Framework/Application/SlateApplication.h"

//--------------------------------------------------------------------------------------------------------------------------
FCogImGuiInputProcessor::FCogImGuiInputProcessor(UPlayerInput* InPlayerInput, FCogImguiContext* InContext, SCogImguiWidget* InMainWidget)
{
    PlayerInput = InPlayerInput;
    Context = InContext;
    MainWidget = InMainWidget;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImGuiInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> SlateCursor)
{
    ImGuiIO& IO = ImGui::GetIO();

    const bool bHasGamepad = (IO.BackendFlags & ImGuiBackendFlags_HasGamepad);
    if (bHasGamepad != SlateApp.IsGamepadAttached())
    {
        IO.BackendFlags ^= ImGuiBackendFlags_HasGamepad;
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("FCogImGuiInputProcessor::Tick | HasGamePad Changed"));
    }

    AddMousePosEvent(SlateApp.GetCursorPos());

    const bool bHasMouse = (IO.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0;
    const bool bUpdateMouseMouseCursor = (IO.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0;
    if (bHasMouse && bUpdateMouseMouseCursor)
    {
        SlateCursor->SetType(FCogImguiInputHelper::ToSlateMouseCursor(ImGui::GetMouseCursor()));
    }

    if (IO.WantSetMousePos)
    {
        SlateApp.SetCursorPos(FCogImguiHelper::ToFVector2D(IO.MousePos));
        //UE_LOG(LogCogImGui, VeryVerbose, TEXT("FCogImGuiInputProcessor::Tick | SetCursorPos"));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImGuiInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& Event)
{
    return HandleKeyEvent(SlateApp, Event, true);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImGuiInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& Event)
{
    return HandleKeyEvent(SlateApp, Event, false);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImGuiInputProcessor::HandleKeyEvent(FSlateApplication& SlateApp, const FKeyEvent& Event, bool IsKeyDown)
{
    //------------------------------------------------------------------------------------------------
    // We want the user to be able to open the console command when imgui has the input.
    //------------------------------------------------------------------------------------------------
    if (FCogImguiInputHelper::IsConsoleEvent(Event))
    {
        const bool Result = ForwardEvent;
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("FCogImGuiInputProcessor::HandleKeyEvent | Key:%s | IsKeyDown:%d | TerminateEvent:%d | ConsoleEvent"), *Event.GetKey().ToString(), IsKeyDown, Result);
        return Result;
    }

    //------------------------------------------------------------------------------------------------
    // We want the user to be able to stop its session by pressing Esc, even when imgui has the input
    //------------------------------------------------------------------------------------------------
    if (FCogImguiInputHelper::IsStopPlaySessionEvent(Event))
    {
        const bool Result = ForwardEvent;
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("FCogImGuiInputProcessor::HandleKeyEvent | Key:%s | IsKeyDown:%d | TerminateEvent:%d | StopPlaySessionEvent"), *Event.GetKey().ToString(), IsKeyDown, Result);
        return Result;
    }

    //------------------------------------------------------------------------------------------------
    // We want the user to be able to use command bingings, even when imgui has the input. 
    // We actually use a console command to toggle the input from the game to imgui, and other
    // windows command such as LoadLayout.
    //------------------------------------------------------------------------------------------------
    if (FCogImguiInputHelper::IsKeyBoundToCommand(PlayerInput, Event))
    {
        const bool Result = ForwardEvent;
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("FCogImGuiInputProcessor::HandleKeyEvent | Key:%s | IsKeyDown:%d | TerminateEvent:%d | KeyBoundToCommand"), *Event.GetKey().ToString(), IsKeyDown, Result);
        return Result;
    }

    ImGuiIO& IO = ImGui::GetIO();

    IO.AddKeyEvent(FCogImguiInputHelper::ToImKey(Event.GetKey()), IsKeyDown);

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
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("FCogImGuiInputProcessor::HandleKeyEvent | Key:%s | IsKeyDown:%d | TerminateEvent:%d | IsModifierKey"), *Event.GetKey().ToString(), IsKeyDown, Result);
        return Result;
    }

    if (Event.GetKey().IsGamepadKey())
    {
        if (IO.WantCaptureKeyboard && (IO.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad))
        {
            const bool Result = TerminateEvent;
            UE_LOG(LogCogImGui, VeryVerbose, TEXT("FCogImGuiInputProcessor::HandleKeyEvent | Key:%s | IsKeyDown:%d | TerminateEvent:%d | NavEnableGamepad"), *Event.GetKey().ToString(), IsKeyDown, Result);
            return Result;
        }
    }

    const bool Result = IO.WantCaptureKeyboard ? TerminateEvent : ForwardEvent;
    UE_LOG(LogCogImGui, VeryVerbose, TEXT("FCogImGuiInputProcessor::HandleKeyEvent | Key:%s | IsKeyDown:%d | WantCaptureKeyboard:%d | TerminateEvent:%d"), *Event.GetKey().ToString(), IsKeyDown, IO.WantCaptureKeyboard, Result);
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImGuiInputProcessor::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& Event)
{
    const float Value = Event.GetAnalogValue();

    ImGuiIO& IO = ImGui::GetIO();
    IO.AddKeyAnalogEvent(FCogImguiInputHelper::ToImKey(Event.GetKey()), FMath::Abs(Value) > 0.0f, Value);

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
bool FCogImGuiInputProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& Event)
{
    AddMousePosEvent(Event.GetScreenSpacePosition());

    if (Context->GetEnableInput() && Context->GetShareMouse() == false && FCogImguiInputHelper::IsMouseInsideMainViewport())
    {
        return TerminateEvent;
    }

    ImGuiIO& IO = ImGui::GetIO();
    const bool Result = IO.WantCaptureMouse ? TerminateEvent : ForwardEvent;
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImGuiInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& Event)
{
    return HandleMouseButtonEvent(SlateApp, Event, true);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImGuiInputProcessor::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& Event)
{
    return HandleMouseButtonEvent(SlateApp, Event, false);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImGuiInputProcessor::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& Event)
{
    return HandleMouseButtonEvent(SlateApp, Event, true);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImGuiInputProcessor::HandleMouseButtonEvent(FSlateApplication& SlateApp, const FPointerEvent& Event, bool IsButtonDown)
{
    ImGuiIO& IO = ImGui::GetIO();

    const uint32 Button = FCogImguiInputHelper::ToImGuiMouseButton(Event.GetEffectingButton());
    IO.AddMouseButtonEvent(Button, IsButtonDown);

    if (Context->GetEnableInput() && Context->GetShareMouse() == false && FCogImguiInputHelper::IsMouseInsideMainViewport())
    {
        const bool Result = TerminateEvent;
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("FCogImGuiInputProcessor::HandleMouseButtonEvent | Button:%d | IsButtonDown:%d | WantCaptureMouse:%d | TerminateEvent:%d | ShareMouse == false"), Button, IsButtonDown, IO.WantCaptureMouse, Result);
        return Result;
    }

    const bool Result = IO.WantCaptureMouse ? TerminateEvent : ForwardEvent;
    UE_LOG(LogCogImGui, VeryVerbose, TEXT("FCogImGuiInputProcessor::HandleMouseButtonEvent | Button:%d | IsButtonDown:%d | WantCaptureMouse:%d | TerminateEvent:%d"), Button, IsButtonDown, IO.WantCaptureMouse, Result);
    return Result;
}
//--------------------------------------------------------------------------------------------------------------------------
void FCogImGuiInputProcessor::AddMousePosEvent(const FVector2D& MousePosition) const
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
bool FCogImGuiInputProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& Event, const FPointerEvent* GestureEvent)
{
    ImGuiIO& IO = ImGui::GetIO();

    IO.AddMouseWheelEvent(0.0f, Event.GetWheelDelta());

    return IO.WantCaptureMouse;
}
