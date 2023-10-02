#pragma once

#include "CoreMinimal.h"
#include "CogImguiInputHelper.h"
#include "imgui.h"

struct FCogImGuiKeyInfo;

class COGIMGUI_API FCogImguiInputHelper
{
public:

    static APlayerController* GetFirstLocalPlayerController(UWorld& World);

    static bool IsKeyEventHandled(const FKeyEvent& KeyEvent);

    static bool WasKeyInfoJustPressed(APlayerController& PlayerController, const FCogImGuiKeyInfo& KeyInfo);

    static bool IsCheckBoxStateMatchingValue(ECheckBoxState CheckBoxState, bool bValue);

    static bool IsKeyEventMatchingKeyInfo(const FKeyEvent& KeyEvent, const FCogImGuiKeyInfo& InputChord);

    static bool IsConsoleEvent(const FKeyEvent& KeyEvent);

    static bool IsImGuiToggleInputEvent(const FKeyEvent& KeyEvent);

    static bool IsStopPlaySessionEvent(const FKeyEvent& KeyEvent);

    static ImGuiKey KeyEventToImGuiKey(const FKeyEvent& KeyEvent);

    static uint32 MouseButtonToImGuiMouseButton(const FKey& MouseButton);

    static EMouseCursor::Type ToSlateMouseCursor(ImGuiMouseCursor MouseCursor);

    template<typename T, std::enable_if_t<(sizeof(T) <= sizeof(ImWchar)), T>* = nullptr>
    static ImWchar CastInputChar(T Char)
    {
        return static_cast<ImWchar>(Char);
    }

    static void InitializeKeyMap();

    static TMap<FKey, ImGuiKey> KeyMap;
};
