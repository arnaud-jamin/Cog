#pragma once

#include "CoreMinimal.h"
#include "imgui.h"
#include "GenericPlatform/ICursor.h"

class APlayerController;
class UPlayerInput;
class UWorld;
enum class ECheckBoxState : uint8;
struct FKey;
struct FKeyBind;
struct FKeyEvent;

//--------------------------------------------------------------------------------------------------------------------------
#define BREAK_CHECKBOX_STATE(CheckBoxState, RequireValue, IgnoreValue)  \
{                                                                       \
    if (CheckBoxState == ECheckBoxState::Checked)                       \
    {                                                                   \
        RequireValue = true;                                            \
        IgnoreValue = false;                                            \
    }                                                                   \
    else if (CheckBoxState == ECheckBoxState::Unchecked)                \
    {                                                                   \
        RequireValue = false;                                           \
        IgnoreValue = true;                                             \
    }                                                                   \
    else if (CheckBoxState == ECheckBoxState::Undetermined)             \
    {                                                                   \
        RequireValue = false;                                           \
        IgnoreValue = false;                                            \
    }                                                                   \
}                                                                       \

class COGIMGUI_API FCogImguiInputHelper
{
public:

    static APlayerController* GetFirstLocalPlayerController(const UWorld& World);

    static UPlayerInput* GetPlayerInput(const UWorld& World);

    static bool IsTopPriorityKey(const UPlayerInput& PlayerInput, const FKey& InKey);

    static bool IsTopPriorityKeyEvent(const UPlayerInput& PlayerInput, const FKeyEvent& InKeyEvent);

    static bool IsCheckBoxStateMatchingValue(ECheckBoxState CheckBoxState, bool bValue);

    static bool IsCheckBoxStateMatchingKeyBindModifier(ECheckBoxState InCheckBoxState, bool InRequireModifier, bool InIgnoreModifier);

    static bool IsKeyEventMatchingKeyBind(const FKeyEvent& KeyEvent, const FKeyBind& KeyBind);

    static ECheckBoxState MakeCheckBoxState(uint8 RequireValue, uint8 IgnoreValue);

    static bool IsInputChordMatchingKeyInfo(const FKeyEvent& InKeyEvent, const FInputChord& InInputChord);

    static bool IsKeyBindMatchingInputChord(const FKeyBind& InKeyBind, const FInputChord& InInputChord);

    static bool IsConsoleEvent(const FKeyEvent& KeyEvent);

    static bool IsKeyBoundToCommand(const UPlayerInput& PlayerInput, const FKeyEvent& KeyEvent);

    static bool IsStopPlaySessionEvent(const FKeyEvent& KeyEvent);

    static uint32 ToImGuiMouseButton(const FKey& MouseButton);

    static ImGuiKey ToImKey(const FKey& Key);

    static EMouseCursor::Type ToSlateMouseCursor(ImGuiMouseCursor MouseCursor);

    static FString InputChordToString(const FInputChord& InInputChord);

    static bool IsMouseInsideMainViewport();

    static bool IsKeyBoundToCommand(const UPlayerInput* InPlayerInput, const FKeyEvent& KeyEvent);

    static TArray<FInputChord>& GetPrioritizedShortcuts() { return CogPrioritizedShortcuts; }

    static bool DisableCommandsConflictingWithShortcuts(UPlayerInput& PlayerInput);

    template<typename T, std::enable_if_t<(sizeof(T) <= sizeof(ImWchar)), T>* = nullptr>
    static ImWchar CastInputChar(T Char)
    {
        return static_cast<ImWchar>(Char);
    }

private:
    static TArray<FInputChord> CogPrioritizedShortcuts; 
};
