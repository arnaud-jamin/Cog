#include "CogImguiInputHandler.h"

#include "CogImguiContext.h"
#include "CogImguiInputHelper.h"
#include "CogImguiInputState.h"


DEFINE_LOG_CATEGORY(LogCogImGuiInputHandler);

namespace
{
	bool bIsKeyboardInputShared = true;
	bool bIsGamepadInputShared = true;
	FReply ToReply(bool bConsume)
	{
		return bConsume ? FReply::Handled() : FReply::Unhandled();
	}
}

void FCogImguiInputHandler::Initialize(FCogImguiContext* InContext)
{
	Context = InContext;
}

FReply FCogImguiInputHandler::OnKeyChar(const FCharacterEvent& CharacterEvent) const
{
	GetMutableInputState().AddCharacter(CharacterEvent.GetCharacter());
	return ToReply(!Context->GetShareKeyboard());
}

FReply FCogImguiInputHandler::OnKeyDown(const FKeyEvent& KeyEvent) const
{
	return HandleKeyEvent(KeyEvent, true);
}

FReply FCogImguiInputHandler::OnKeyUp(const FKeyEvent& KeyEvent) const
{
	return HandleKeyEvent(KeyEvent, false);
}

FReply FCogImguiInputHandler::HandleKeyEvent(const FKeyEvent& KeyEvent, bool Down) const
{	
	if (Context->GetEnableInput() == false)
	{
		return FReply::Unhandled();
	}

	if (KeyEvent.GetKey().IsGamepadKey())
	{
		// Not handling game pad events for now
		/*
		if (KeyEvent.GetKey().IsGamepadKey())
		{
			bool bConsume = false;
			if (GetMutableInputState().IsGamepadNavigationEnabled())
			{
				GetMutableInputState().SetGamepadNavigationKey(KeyEvent, true);
				bConsume = !bIsGamepadInputShared;
			}

			return ToReply(bConsume);
		}
		*/
		return FReply::Unhandled();
	}

	if (FCogImguiInputHelper::IsKeyEventHandled(Context->GetGameViewport()->GetWorld(), KeyEvent) == false)
	{
		CopyModifierKeys(KeyEvent);
		return FReply::Unhandled();
	}
	
	{
		const bool bConsume = !Context->GetShareKeyboard();

		GetMutableInputState().SetKeyDown(KeyEvent, true);
		GetMutableInputState().AddKeyEvent(KeyEvent, true);
		CopyModifierKeys(KeyEvent);

		return ToReply(bConsume);
	}
}

FReply FCogImguiInputHandler::OnAnalogValueChanged(const FAnalogInputEvent& AnalogInputEvent) const
{
	// Not handling game pad events for now
	if (AnalogInputEvent.GetKey().IsGamepadKey())
	{
		return FReply::Unhandled();
	}

	return FReply::Handled();
}

FReply FCogImguiInputHandler::OnMouseButtonDown(const FPointerEvent& MouseEvent) const
{
    return HandleMouseButtonEvent(MouseEvent, true);
}

FReply FCogImguiInputHandler::OnMouseButtonDoubleClick(const FPointerEvent& MouseEvent) const
{
    return HandleMouseButtonEvent(MouseEvent, true);
}

FReply FCogImguiInputHandler::OnMouseButtonUp(const FPointerEvent& MouseEvent) const
{
    return HandleMouseButtonEvent(MouseEvent, false);
}

FReply FCogImguiInputHandler::HandleMouseButtonEvent(const FPointerEvent& MouseEvent, bool Down) const
{
	if (Context->GetEnableInput() == false)
	{
		UE_LOG(LogCogImGuiInputHandler, VeryVerbose,
			TEXT("FCogImguiInputHandler::HandleMouseButtonEvent | Unhandled | EnableInput == false | Down:%d"), Down);
		return FReply::Unhandled();
	}
	const uint32 MouseButton = FCogImguiInputHelper::ToImGuiMouseButton(MouseEvent.GetEffectingButton());

	GetMutableInputState().SetMouseDown(MouseEvent, Down);
	GetMutableInputState().AddMouseButtonEvent(MouseEvent, Down);
	return ToReply(true);
}

FCogImguiInputState& FCogImguiInputHandler::GetMutableInputState() const
{
	return Context->GetInputState();
}

FReply FCogImguiInputHandler::OnMouseWheel(const FPointerEvent& MouseEvent) const
{
	if (Context->GetEnableInput() == false)
	{
		return FReply::Unhandled();
	}
	GetMutableInputState().AddMouseWheelDelta(MouseEvent.GetWheelDelta());
	return ToReply(true);
}

FReply FCogImguiInputHandler::OnMouseMove(const FVector2D& MousePosition, const FPointerEvent& MouseEvent) const
{
	return OnMouseMove(MousePosition);
}

FReply FCogImguiInputHandler::OnMouseMove(const FVector2D& MousePosition) const
{
	if (Context->GetEnableInput() == false)
	{
		//UE_LOG(LogCogImGui, VeryVerbose, TEXT("SCogImguiWidget::OnMouseMove | Window:%s | Unhandled | EnableInput == false"), Window.IsValid() ? *Window->GetTitle().ToString() : *FString("None"));
		return FReply::Unhandled();
	}
	GetMutableInputState().SetMousePosition(MousePosition);
	return ToReply(true);
}

void FCogImguiInputHandler::OnKeyboardInputEnabled()
{
	bKeyboardInputEnabled = true;
}

void FCogImguiInputHandler::OnKeyboardInputDisabled()
{
	if (bKeyboardInputEnabled)
	{
		bKeyboardInputEnabled = false;
		GetMutableInputState().ResetKeyboard();
	}
}

void FCogImguiInputHandler::OnMouseInputEnabled()
{
	if (!bMouseInputEnabled)
	{
		bMouseInputEnabled = true;
		UpdateInputStatePointer();
	}
}

void FCogImguiInputHandler::OnMouseInputDisabled()
{
	if (bMouseInputEnabled)
	{
		bMouseInputEnabled = false;
		GetMutableInputState().ResetMouse();
		UpdateInputStatePointer();
	}
}

void FCogImguiInputHandler::UpdateInputStatePointer() const
{
	GetMutableInputState().SetMousePointer(bMouseInputEnabled && false);
}

void FCogImguiInputHandler::CopyModifierKeys(const FInputEvent& InputEvent) const
{
	GetMutableInputState().SetControlDown(InputEvent.IsControlDown());
	GetMutableInputState().SetShiftDown(InputEvent.IsShiftDown());
	GetMutableInputState().SetAltDown(InputEvent.IsAltDown());
}
