#pragma once

#include "CoreMinimal.h"
#include "CogImguiInteroperability.h"
#include "CogImguiContext.h"
#include "CogImguiInputState.h"

class FCogImguiContext;
DECLARE_LOG_CATEGORY_EXTERN(LogCogImGuiInputHandler, Warning, All);

/**
 * Handles input and sends it to the input state,
 * which is copied to the ImGui IO at the beginning of the frame
 */
struct COGIMGUI_API FCogImguiInputHandler
{
public:

	void Initialize(FCogImguiContext* InContext);

	/*
	 * Called to handle character events
	 * @returns Response whether the event was handled
	 */
	FReply OnKeyChar(const struct FCharacterEvent& CharacterEvent) const;

	/*
	 * Called to handle key down events.
	 * @returns Response whether the event was handled
	 */
	FReply OnKeyDown(const FKeyEvent& KeyEvent) const;

	/*
	 * Called to handle analog value change events.
	 * @returns Response whether the event was handled
	 */
	FReply OnKeyUp(const FKeyEvent& KeyEvent) const;

	/*
	 * Called to handle analog value change events.
	 * @returns Response whether the event was handled
	 */
	FReply OnAnalogValueChanged(const FAnalogInputEvent& AnalogInputEvent) const;

	/**
	 * Called to handle Mouse Down events
	 * @param MouseEvent - the Mouse Event
	 * @return Response whether the event was handled
	 */
	FReply OnMouseButtonDown(const FPointerEvent& MouseEvent) const;
	
	/*
	 * Called to handle mouse button double-click events.
	 * @returns Response whether the event was handled
	 */
	FReply OnMouseButtonDoubleClick(const FPointerEvent& MouseEvent) const;

	/*
	 * Called to handle mouse button up events.
	 * @returns Response whether the event was handled.
	 */
	FReply OnMouseButtonUp(const FPointerEvent& MouseEvent) const;

	/**
	 * Called to handle mouse wheel events
	 * @param MouseEvent - the mouse wheel event
	 * @return Response whether the event was handled
	 */
	FReply OnMouseWheel(const FPointerEvent& MouseEvent) const;

	/**
	 * Called to handle mouse move events.
	 * @param MousePosition - the Mouse Position in ImGui space
	 * @param MouseEvent - Optional Mouse event passed from Slate
	 * @return Response whether the event was handled
	 */
	FReply OnMouseMove(const FVector2D& MousePosition, const FPointerEvent& MouseEvent) const;
	FReply OnMouseMove(const FVector2D& MousePosition) const;
	
	void OnKeyboardInputEnabled();
	void OnKeyboardInputDisabled();
	
	void OnMouseInputEnabled();
	void OnMouseInputDisabled();

protected:
	void UpdateInputStatePointer() const;

	// Copy state of modifier keys to input state
	void CopyModifierKeys(const FInputEvent& InputEvent) const;

	FReply HandleKeyEvent(const FKeyEvent& KeyEvent, bool Down) const;

	FReply HandleMouseButtonEvent(const FPointerEvent& MouseEvent, bool Down) const;

	FCogImguiInputState& GetMutableInputState() const;

private:
	bool bMouseInputEnabled = true;
	bool bKeyboardInputEnabled = true;

	FCogImguiContext* Context = nullptr;
};
