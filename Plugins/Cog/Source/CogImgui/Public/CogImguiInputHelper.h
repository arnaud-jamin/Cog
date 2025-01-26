#pragma once

#include "CoreMinimal.h"
#include "imgui.h"
#include "GenericPlatform/ICursor.h"

class APlayerController;
class UPlayerInput;
class UWorld;
enum class ECheckBoxState : uint8;
struct FCogImGuiKeyInfo;
struct FCogImGuiKeyInfo;
struct FKey;
struct FKeyBind;
struct FKeyEvent;

class COGIMGUI_API FCogImguiInputHelper
{
public:

    static APlayerController* GetFirstLocalPlayerController(const UWorld& World);

    static UPlayerInput* GetPlayerInput(const UWorld& World);

    static bool IsTopPriorityKey(UWorld* InWorld, const FKey& InKey);

    static bool IsTopPriorityKeyEvent(UWorld* InWorld, const FKeyEvent& InKeyEvent);

    static bool WasKeyInfoJustPressed(APlayerController& PlayerController, const FCogImGuiKeyInfo& KeyInfo);

    static bool IsCheckBoxStateMatchingValue(ECheckBoxState CheckBoxState, bool bValue);

    static bool IsCheckBoxStateMatchingKeyBindModifier(ECheckBoxState InCheckBoxState, bool InRequireModifier, bool InIgnoreModifier);

    static bool IsKeyEventMatchingKeyInfo(const FKeyEvent& InKeyEvent, const FCogImGuiKeyInfo& InKeyInfo);

    static bool IsKeyBindMatchingKeyInfo(const FKeyBind& InKeyBind, const FCogImGuiKeyInfo& InKeyInfo);

    static bool IsKeyEventMatchingKeyBind(const FKeyEvent& KeyEvent, const FKeyBind& KeyBind);

    static bool IsKeyInfoPressed(const UPlayerInput* PlayerInput, const FCogImGuiKeyInfo& InKeyInfo);

    static ECheckBoxState MakeCheckBoxState(uint8 RequireValue, uint8 IgnoreValue);

    static void KeyBindToKeyInfo(const FKeyBind& KeyBind, FCogImGuiKeyInfo& KeyInfo);

    static void KeyInfoToKeyBind(const FCogImGuiKeyInfo& KeyInfo, FKeyBind& KeyBind);

    static bool IsConsoleEvent(const FKeyEvent& KeyEvent);

    static bool IsKeyBoundToCommand(UWorld* World, const FKeyEvent& KeyEvent);

    static bool IsStopPlaySessionEvent(const FKeyEvent& KeyEvent);

    static uint32 ToImGuiMouseButton(const FKey& MouseButton);

    static ImGuiKey ToImKey(const FKey& Key);

    static EMouseCursor::Type ToSlateMouseCursor(ImGuiMouseCursor MouseCursor);

    static FString CommandToString(const UWorld& World, const FString& Command);

    static FString CommandToString(const UPlayerInput* PlayerInput, const FString& Command);

    static FString KeyBindToString(const FKeyBind& InKeyBind);

    static FString KeyInfoToString(const FCogImGuiKeyInfo& InKeyInfo);

    static bool IsMouseInsideMainViewport();

    static bool IsKeyBoundToCommand(const UPlayerInput* InPlayerInput, const FKeyEvent& KeyEvent);

    static void SetShortcuts(const UWorld& World, const TArray<FCogImGuiKeyInfo>& InShortcuts, bool InDisableCommandsConflictingWithShortcuts);

    template<typename T, std::enable_if_t<(sizeof(T) <= sizeof(ImWchar)), T>* = nullptr>
    static ImWchar CastInputChar(T Char)
    {
        return static_cast<ImWchar>(Char);
    }

private:
    static TArray<FCogImGuiKeyInfo> CogShortcuts; 
};
