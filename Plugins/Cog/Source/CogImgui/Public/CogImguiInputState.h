// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CogImguiInteroperability.h"
#include "CogArrayUtility.h"

struct COGIMGUI_API FCogImguiInputState
{
	// Characters buffer
	using FCharactersBuffer = TArray<TCHAR, TInlineAllocator<8>>;

	// Array for mouse button states
	using FMouseButtonsArray = Cog::ImguiInterops::ImGuiTypes::FMouseButtonsArray;

	// Array for mouse button events
	using FMouseEvents = Cog::ImguiInterops::ImGuiTypes::FMouseEvents;

	// Array for key states.
	using FKeysArray = Cog::ImguiInterops::ImGuiTypes::FKeysArray;

	// Array for key events.
	using FKeyEvents = Cog::ImguiInterops::ImGuiTypes::FKeyEvents;

	// Array for navigation input states
	// @TODO Implement gamepad
	// using FNavInputArray = Cog::ImguiInterps::ImGuiTypes::FNavInputArray;

	// Pair of indices defining range in mouse buttons array.
	using FMouseButtonsIndexRange = Utilities::TArrayIndexRange<FMouseButtonsArray, uint32>;

	// Pair of indices defining range in keys array.
	using FKeysIndexRange = Utilities::TArrayIndexRange<FKeysArray, uint32>;

	// Create empty state with whole range instance with whole update state marked as dirty.
	FCogImguiInputState();

	// Get Reference to input characters buffer.
	const FCharactersBuffer& GetCharacters() const { return InputCharacters; }

	// Add a character to the characters bufer. We can store and send to ImGui up to 16 characters per frame.
	// Any character beyond that limit will be discarded
	// @param Char - Character to add
	void AddCharacter(TCHAR Char);

	// Get reference to the array with key down states.
	const FKeysArray& GetKeys() const { return KeysDown; }

	// Get possibly empty range of indices bounding dirty part of the keys array.
	const FKeysIndexRange& GetKeysUpdateRange() const { return KeysUpdateRange; }

	const FKeyEvents& GetKeyEvents() const { return KeyEvents; }
	
	// Change state of the key in the keys array and expend range bounding dirty part of the array
	// @param KeyEvent - Key event representing the key
	// @param bIsDown - True, if key is down
	void SetKeyDown(const FKeyEvent& KeyEvent, bool bIsDown) { SetKeyDown(Cog::ImguiInterops::GetKeyIndex(KeyEvent), bIsDown); }

	// Change state of the key in the keys array and expand range bounding dirty part of the array.
	// @param Key - Keyboard key
	// @param bIsDown - True, if key is down
	void SetKeyDown(const FKey& Key, bool bIsDown) { SetKeyDown(Cog::ImguiInterops::GetKeyIndex(Key), bIsDown); }

	// Add a Key pressed/released event to the KeyEvents array
	// @param ImGuiKey - Keyboard Key
	// @param bIsDown - True, if key down
	void AddKeyEvent(const FKeyEvent& KeyEvent, bool bIsDown);

	// Get a reference to the array with mouse button states.
	const FMouseButtonsArray& GetMouseButtons() const { return MouseButtonsDown; }

	// Get possibly empty range of indices bounding dirty part of the mouse buttons array
	const FMouseButtonsIndexRange& GetMouseButtonsUpdateRange() const { return MouseButtonsUpdateRange; }

	// Get Mouse Button Events.
	const FMouseEvents& GetMouseButtonEvents() const { return MouseEvents; }

	// Change state of the button in the mouse buttons array and expand range bounding dirty part of the array
	// @param MouseEvent - Mouse event representing mouse button
	// @param bIsDown - True, if button is down
	void SetMouseDown(const FPointerEvent& MouseEvent, bool bIsDown) { SetMouseDown(Cog::ImguiInterops::GetMouseIndex(MouseEvent), bIsDown); }

	// Add a MouseButton pressed/released event to the MouseEvents array
	// @param MouseButton - Mouse button event
	// @param bIsDown - True, if key is down
	void SetMouseDown(const FKey& MouseButton, bool bIsDown) { SetMouseDown(Cog::ImguiInterops::GetMouseIndex(MouseButton), bIsDown); }

	void AddMouseButtonEvent(const FPointerEvent& MouseEvent, bool bIsDown);
	
	// Get Mouse wheel delta accumulated during the last frame.
	float GetMouseWheelDelta() const { return MouseWheelDelta; }

	// Add mouse wheel delta
	// @param DeltaValue - Mouse wheel delta to add
	void AddMouseWheelDelta(float DeltaValue) { MouseWheelDelta += DeltaValue; }

	// Get the mouse position.
	const FVector2D& GetMousePosition() const { return MousePosition; }

	// Set the mouse position.
	// @param Position - Mouse position.
	void SetMousePosition(const FVector2D& Position) { MousePosition = Position; }

	// Check whether input has active mouse pointer
	bool HasMousePointer() const { return bHasMousePointer; }

	// Set whether input has active mouse pointer
	// @param bHasPointer - True, if input has active mouse pointer
	void SetMousePointer(bool bInHasMousePointer) { bHasMousePointer = bInHasMousePointer; }

	// Get Control down state.
	bool IsControlDown() const { return bIsControlDown; }

	// Set Control down state
	// @param bIsDown - True, if Control is down
	void SetControlDown(bool bIsDown) { bIsControlDown = bIsDown; }

	// Get Shift down state.
	bool IsShiftDown() const { return bIsShiftDown; }

	// Set Shift down state.
	// @param bIsDown - True, if Shift is down
	void SetShiftDown(bool bIsDown) { bIsShiftDown = bIsDown; }

	bool IsAltDown() const { return bIsAltDown; }

	void SetAltDown(bool bIsDown) { bIsAltDown = bIsDown; }

	// const FNavInputArray& GetNavigationInputs() const { return NavigationInputs; }

	// void SetGamepadNavigationKey(const FKeyEvent& KeyEvent, bool bIsDown)
	//	{ Cog::ImGuiInterops::SetGamepadNavigationKey(NavigationInputs, KeyEvent.GetKey(), bIsDown); }

	// void SetGamepadNavigationAxis(const FAnalogInputEvent& AnalogInputEvent, float Value)
	//	{ Cog::ImGuiInterops::SetGamepadNavigationAxis(NavigationInputs, AnalogInputEvent.GetKey(), Value); }

	bool IsKeyboardNavigationEnabled() const { return bKeyboardNavigationEnabled; }

	void SetKeyboardNavigationEnabled(bool bEnabled) { bKeyboardNavigationEnabled = bEnabled; }

	bool IsGamepadNavigationEnabled() const { return bGamepadNavigationEnabled; }

	void SetGamepadNavigationEnabled(bool bEnabled) { bGamepadNavigationEnabled = bEnabled; }

	bool HasGamepad() const { return bHasGamepad; }

	void SetGamepad(bool bInHasGamepad) { bHasGamepad = bInHasGamepad; }

	void Reset()
	{
		ResetKeyboard();
		ResetMouse();
		ResetGamepadNavigation();
	}

	void ResetKeyboard()
	{
		ClearCharacters();
		ClearKeys();
		ClearModifierKeys();
	}

	void ResetMouse()
	{
		ClearMouseButtons();
		ClearMouseAnalogue();
	}

	// Gamepad Disabled for now
	void ResetGamepadNavigation()
	{
		// ClearNavigationInputs();
	}

	// Clear part of the state that is meant to be updated in every frame like: accumulators, buffers, navigation data
	// and information about dirty parts of keys or mouse buttons arrays
	void ClearUpdateState();

private:
	void SetKeyDown(uint32 KeyIndex, bool bIsDown);
	void SetMouseDown(uint32 MouseIndex, bool bIsDown);

	void ClearCharacters();
	void ClearKeys();
	void ClearMouseButtons();
	void ClearMouseAnalogue();
	void ClearModifierKeys();
	// ClearNavigationInputs() // disabled gamepad for now

	FVector2D MousePosition = FVector2D::ZeroVector;
	float MouseWheelDelta = 0.0f;

	FMouseButtonsArray MouseButtonsDown;
	FMouseButtonsIndexRange MouseButtonsUpdateRange;
	FMouseEvents MouseEvents;

	FCharactersBuffer InputCharacters;

	FKeysArray KeysDown;
	FKeysIndexRange KeysUpdateRange;

	FKeyEvents KeyEvents;

	//FNavInputArray NavigationInputs // Disabled gamepad

	bool bHasMousePointer = false;

	bool bIsControlDown = false;
	bool bIsShiftDown = false;
	bool bIsAltDown = false;

	bool bKeyboardNavigationEnabled = false;
	bool bGamepadNavigationEnabled = false;
	bool bHasGamepad = false;
};
