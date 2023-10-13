#pragma once

#include "CoreMinimal.h"
#include "imgui.h"
#include "UObject/ReflectedTypeAccessors.h"

class UEnum;
class FEnumProperty;

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

    static void MenuSearchBar(ImGuiTextFilter& Filter);

    static void PushBackColor(const ImVec4& Color);

    static void PopBackColor();

    static void SetNextItemToShortWidth();

    static float GetFontWidth();

    template<typename EnumType>
    static bool ComboboxEnum(const char* Label, const EnumType CurrentValue, EnumType& NewValue);

    template<typename EnumType>
    static bool ComboboxEnum(const char* Label, EnumType& Value);

    static bool ComboboxEnum(const char* Label, UEnum* Enum, int64 CurrentValue, int64& NewValue);
    
    static bool ComboboxEnum(const char* Label, UObject* Object, const char* FieldName, uint8* PointerToEnumValue);
    
    static bool ComboboxEnum(const char* Label, const FEnumProperty* EnumProperty, uint8* PointerToEnumValue);

};

template<typename EnumType>
bool FCogWindowWidgets::ComboboxEnum(const char* Label, const EnumType CurrentValue, EnumType& NewValue)
{
    int64 NewValueInt;
    if (ComboboxEnum(Label, StaticEnum<EnumType>(), (int64)CurrentValue, NewValueInt))
    {
        NewValue = (EnumType)NewValueInt;
        return true;
    }

    return false;
}

template<typename EnumType>
bool FCogWindowWidgets::ComboboxEnum(const char* Label, EnumType& Value)
{
    return ComboboxEnum(Label, Value, Value);
}