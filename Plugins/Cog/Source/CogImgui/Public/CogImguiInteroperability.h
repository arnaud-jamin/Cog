// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "imgui.h"

struct FCogImguiInputState;
class FCogImguiContext;
struct FCogImguiInputStaste;

namespace Cog::ImguiInterops
{
	namespace ImGuiTypes
	{
		using FMouseButtonsArray = decltype(ImGuiIO::MouseDown);

		struct FCogImguiMouseButtonEvent
		{
			FCogImguiMouseButtonEvent(int32 InButtonId, bool bInDown)
				: ButtonId(InButtonId), Down(bInDown) {}
			int32 ButtonId = 0;
			bool Down = false;
		};
		using FMouseEvents = TArray<FCogImguiMouseButtonEvent>;

		using FKeysArray = decltype(ImGuiIO::KeysData);

		struct FCogImguiKeyEvent
		{
			FCogImguiKeyEvent(const ImGuiKey& InKey, bool bInDown)
				: Key(InKey), Down(bInDown) {}
			ImGuiKey Key = ImGuiKey::ImGuiKey_None;
			bool Down = false;
		};
		using FKeyEvents = TArray<FCogImguiKeyEvent>;

		struct FCogImguiAnalogKeyEvent
		{
			FCogImguiAnalogKeyEvent(const ImGuiKey& InKey, bool bInDown)
				: Key(InKey), Down(bInDown) {}
			ImGuiKey Key = ImGuiKey::ImGuiKey_None;
			bool Down = false;
		};
		using FAnalogKeyEvents = TArray<FCogImguiAnalogKeyEvent>;
	}

	//==============================================================
	// Input Mapping
	//==============================================================

	// Map FKey to index in keys buffer
	uint32 GetKeyIndex(const FKey& Key);

	// Map key event to index in keys buffer
	uint32 GetKeyIndex(const FKeyEvent& KeyEvent);

	// Map mouse FKey to index in mouse buttons buffer
	uint32 GetMouseIndex(const FKey& MouseButton);

	// Map pointer event to index in mouse buttons buffer
	FORCEINLINE uint32 GetMouseIndex(const FPointerEvent& MouseEvent)
	{
		return GetMouseIndex(MouseEvent.GetEffectingButton());
	}

	// Convert from ImGuiMouseCursor type to EMouseCursor
	EMouseCursor::Type ToSlateMouseCursor(ImGuiMouseCursor MouseCursor);

	// TODO: Handle gamepad inputs

	//==================================================================
	// Input State Copying
	//==================================================================

	// Copy input to ImGui IO
	// @param IO - Target ImGui IO
	// @param InputState - InputState to copy
	// @param CogContext - The Cog Context we're operating in
	void CopyInput(ImGuiIO& IO, const FCogImguiInputState& InputState, const FCogImguiContext& CogContext);
}