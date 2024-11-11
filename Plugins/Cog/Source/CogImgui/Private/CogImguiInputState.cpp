#include "CogImguiInputState.h"

#include "CogImguiInputHelper.h"

FCogImguiInputState::FCogImguiInputState()
{
	Reset();
}

void FCogImguiInputState::AddCharacter(TCHAR Char)
{
	InputCharacters.Add(Char);
}

void FCogImguiInputState::SetKeyDown(uint32 KeyIndex, bool bIsDown)
{
	if (KeyIndex < Utilities::GetArraySize(KeysDown))
	{
		if (KeysDown[KeyIndex].Down != bIsDown)
		{
			KeysDown[KeyIndex].Down = bIsDown;
			KeysUpdateRange.AddPosition(KeyIndex);
		}
	}
}

void FCogImguiInputState::AddKeyEvent(const FKeyEvent& KeyEvent, bool bIsDown)
{
	ImGuiKey Key = FCogImguiInputHelper::ToImKey(KeyEvent.GetKey());
	KeyEvents.Emplace(Key, bIsDown);
}

void FCogImguiInputState::SetMouseDown(uint32 MouseIndex, bool bIsDown)
{
	if (MouseIndex < Utilities::GetArraySize(MouseButtonsDown))
	{
		if (MouseButtonsDown[MouseIndex] != bIsDown)
		{
			MouseButtonsDown[MouseIndex] = bIsDown;
			MouseButtonsUpdateRange.AddPosition(MouseIndex);
		}
	}
}

void FCogImguiInputState::AddMouseButtonEvent(const FPointerEvent& MouseEvent, bool bIsDown)
{
	const uint32 MouseButton = FCogImguiInputHelper::ToImGuiMouseButton(MouseEvent.GetEffectingButton());
	MouseEvents.Emplace(MouseButton, bIsDown);
}

void FCogImguiInputState::ClearUpdateState()
{
	ClearCharacters();

	KeysUpdateRange.SetEmpty();
	KeyEvents.Reset();

	MouseButtonsUpdateRange.SetEmpty();
	MouseEvents.Reset();

	MouseWheelDelta = 0.0f;
}

void FCogImguiInputState::ClearCharacters()
{
	InputCharacters.Empty();
}

void FCogImguiInputState::ClearKeys()
{
	using std::fill;
	fill(KeysDown, &KeysDown[Utilities::GetArraySize(KeysDown)], ImGuiKeyData());

	// Mark the whole array as dirty because potentially each entry could be affected.
	KeysUpdateRange.SetFull();

	KeyEvents.Reset();
}

void FCogImguiInputState::ClearMouseButtons()
{
	using std::fill;
	fill(MouseButtonsDown, &MouseButtonsDown[Utilities::GetArraySize(MouseButtonsDown)], false);

	// Mark the whole array as dirty because potentially each entry could be affected
	MouseButtonsUpdateRange.SetFull();

	MouseEvents.Reset();
}

void FCogImguiInputState::ClearMouseAnalogue()
{
	MousePosition = FVector2D::ZeroVector;
	MouseWheelDelta = 0.f;
}

void FCogImguiInputState::ClearModifierKeys()
{
	bIsControlDown = false;
	bIsShiftDown = false;
	bIsAltDown = false;
}

// void FCogImguiInputState::ClearNavigationInputs()
// { std::fill(NavigationInputs, &NavigationInputs[Utilities::GetArraySize(NavigationInputs)], 0.0f); }

