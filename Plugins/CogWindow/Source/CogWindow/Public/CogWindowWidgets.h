#pragma once

#include "CoreMinimal.h"
#include "imgui.h"

class COGWINDOW_API FCogWindowWidgets
{
public:

    static void BeginTableTooltip();

    static void EndTableTooltip();

    static void ProgressBarCentered(float Fraction, const ImVec2& Size, const char* Overlay);

    static void ToggleButton(bool* Value, const char* TextTrue, const char* TextFalse, const ImVec4& TrueColor, const ImVec4& FalseColor, const ImVec2& Size = ImVec2(0, 0));

    static void SliderWithReset(const char* Name, float* Value, float Min, float Max, const float& ResetValue, const char* Format);

    static void HelpMarker(const char* Text);

    static void PushStyleCompact();
    
    static void PopStyleCompact();

    static void AddTextWithShadow(ImDrawList* DrawList, const ImVec2& Position, ImU32 Color, const char* TextBegin, const char* TextEnd = NULL);

    static float TextBaseWidth;

    static float TextBaseHeight;

    static void MenuSearchBar(ImGuiTextFilter& Filter);

    static void PushBackColor(const ImVec4& Color);

    static void PopBackColor();
};
