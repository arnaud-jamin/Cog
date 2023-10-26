#include "CogWindowWidgets.h"

#include "CogImguiHelper.h"
#include "CogImGuiKeyInfo.h"
#include "CogImguiInputHelper.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "InputCoreTypes.h"
#include "Kismet/KismetMathLibrary.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::BeginTableTooltip()
{
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(29, 42, 62, 240));
    ImGui::BeginTooltip();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::EndTableTooltip()
{
    ImGui::EndTooltip();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::ProgressBarCentered(float Fraction, const ImVec2& Size, const char* Overlay)
{
    ImGuiWindow* window = FCogImguiHelper::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *ImGui::GetCurrentContext();
    const ImGuiStyle& style = g.Style;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImGui::CalcItemSize(Size, ImGui::CalcItemWidth(), g.FontSize + style.FramePadding.y * 2.0f);
    ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, 0))
        return;

    // Render
    Fraction = ImSaturate(Fraction);
    ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
    bb.Expand(ImVec2(-style.FrameBorderSize, -style.FrameBorderSize));
    const ImVec2 fill_br = ImVec2(ImLerp(bb.Min.x, bb.Max.x, Fraction), bb.Max.y);
    ImGui::RenderRectFilledRangeH(window->DrawList, bb, ImGui::GetColorU32(ImGuiCol_PlotHistogram), 0.0f, Fraction, style.FrameRounding);

    // Default displaying the fraction as percentage string, but user can override it
    char overlay_buf[32];
    if (!Overlay)
    {
        ImFormatString(overlay_buf, IM_ARRAYSIZE(overlay_buf), "%.0f%%", Fraction * 100 + 0.01f);
        Overlay = overlay_buf;
    }

    ImVec2 overlay_size = ImGui::CalcTextSize(Overlay, NULL);
    if (overlay_size.x > 0.0f)
    {

        ImVec2 pos1(ImClamp(bb.Min.x + (bb.Max.x - bb.Min.x) * 0.5f - overlay_size.x * 0.5f + style.ItemSpacing.x, bb.Min.x, bb.Max.x - style.ItemInnerSpacing.x), bb.Min.y - 1);
        ImVec2 pos2(pos1.x + 1, pos1.y + 1);

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
        ImGui::RenderTextClipped(pos2, bb.Max, Overlay, NULL, &overlay_size, ImVec2(0.0f, 0.5f), &bb);
        ImGui::PopStyleColor();

        ImGui::RenderTextClipped(pos1, bb.Max, Overlay, NULL, &overlay_size, ImVec2(0.0f, 0.5f), &bb);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::ToggleButton(bool* Value, const char* TextTrue, const char* TextFalse, const ImVec4& TrueColor, const ImVec4& FalseColor, const ImVec2& Size)
{
    if (*Value)
    {
        ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(TrueColor.x, TrueColor.y, TrueColor.z, TrueColor.w * 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   ImVec4(TrueColor.x, TrueColor.y, TrueColor.z, TrueColor.w * 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,    ImVec4(TrueColor.x, TrueColor.y, TrueColor.z, TrueColor.w * 1.0f));

        if (ImGui::Button(TextTrue, Size))
        {
            *Value = false;
        }

        ImGui::PopStyleColor(3);
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(FalseColor.x, FalseColor.y, FalseColor.z, FalseColor.w * 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   ImVec4(FalseColor.x, FalseColor.y, FalseColor.z, FalseColor.w * 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,    ImVec4(FalseColor.x, FalseColor.y, FalseColor.z, FalseColor.w * 1.0f));

        if (ImGui::Button(TextFalse, Size))
        {
            *Value = true;
        }

        ImGui::PopStyleColor(3);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::SliderWithReset(const char* Name, float* Value, float Min, float Max, const float& ResetValue, const char* Format)
{
    ImGui::SliderFloat(Name, Value, Min, Max, Format);

    if (ImGui::BeginPopupContextItem(Name))
    {
        if (ImGui::Button("Reset"))
        {
            *Value = ResetValue;
            
            if (ImGuiWindow* Window = FCogImguiHelper::GetCurrentWindow())
            {
                const ImGuiID id = Window->GetID(Name);
                ImGui::MarkItemEdited(id);
            }
        }

        ImGui::EndPopup();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::HelpMarker(const char* Text)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(Text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::PushStyleCompact()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.WindowPadding.x * 0.60f, (float)(int)(style.WindowPadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x * 0.60f, (float)(int)(style.FramePadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x * 0.60f, (float)(int)(style.ItemSpacing.y * 0.60f)));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::PopStyleCompact()
{
    ImGui::PopStyleVar(3);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::AddTextWithShadow(ImDrawList* DrawList, const ImVec2& Position, ImU32 Color, const char* TextBegin, const char* TextEnd /*= NULL*/)
{
    float Alpha = ImGui::ColorConvertU32ToFloat4(Color).w;
    DrawList->AddText(Position + ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, Alpha)), TextBegin, TextEnd);
    DrawList->AddText(Position, Color, TextBegin, TextEnd);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::MenuSearchBar(ImGuiTextFilter& Filter, float Width /*= -1*/)
{
    ImGuiWindow* Window = FCogImguiHelper::GetCurrentWindow();
    ImGui::SameLine();
    ImVec2 Pos1 = Window->DC.CursorPos;
    Filter.Draw("", Width);
    ImVec2 Pos2 = Window->DC.CursorPos;

    if (ImGui::IsItemActive() == false && Filter.Filters.empty())
    {
        static const char* Text = "Search";
        const float Height = ImGui::GetFrameHeight();
        ImGuiContext& g = *ImGui::GetCurrentContext();
        ImVec2 Min = Pos1 + ImVec2(g.Style.ItemSpacing.x, 0.0f);
        ImVec2 Max = Pos2 + ImVec2(-g.Style.ItemSpacing.x, Height);
        ImRect BB(Min, Max);
        ImVec2 TextSize = ImGui::CalcTextSize(Text, NULL);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 128));
        ImGui::RenderTextClipped(Min, Max, Text, NULL, &TextSize, ImVec2(0.0f, 0.5f), &BB);
        ImGui::PopStyleColor();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::PushBackColor(const ImVec4& Color)
{
    ImGui::PushStyleColor(ImGuiCol_Button,              ImVec4(Color.x, Color.y, Color.z, Color.w * 0.25f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,       ImVec4(Color.x, Color.y, Color.z, Color.w * 0.3f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,        ImVec4(Color.x, Color.y, Color.z, Color.w * 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg,             ImVec4(Color.x, Color.y, Color.z, Color.w * 0.25f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,      ImVec4(Color.x, Color.y, Color.z, Color.w * 0.3f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,       ImVec4(Color.x, Color.y, Color.z, Color.w * 0.5f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab,          ImVec4(Color.x, Color.y, Color.z, Color.w * 0.8f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive,    ImVec4(Color.x, Color.y, Color.z, Color.w * 1.0f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark,           ImVec4(Color.x, Color.y, Color.z, Color.w * 0.8f));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::PopBackColor()
{
    ImGui::PopStyleColor(9);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::SetNextItemToShortWidth()
{
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 10);
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogWindowWidgets::GetFontWidth()
{
    return ImGui::GetFontSize() * 0.5f;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::ComboboxEnum(const char* Label, UObject* Object, const char* FieldName, uint8* PointerToEnumValue)
{
    const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Object->GetClass()->FindPropertyByName(FName(FieldName)));
    if (EnumProperty == nullptr)
    {
        return false;
    }

    return ComboboxEnum(Label, EnumProperty, PointerToEnumValue);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::ComboboxEnum(const char* Label, const FEnumProperty* EnumProperty, uint8* PointerToValue)
{
    bool HasChanged = false;

    UEnum* Enum = EnumProperty->GetEnum();
    int64 EnumValue = EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(PointerToValue);
    FString EnumValueName = Enum->GetNameStringByValue(EnumValue);

    int64 NewValue;
    if (ComboboxEnum(Label, Enum, EnumValue, NewValue))
    {
        EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(PointerToValue, NewValue);
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::ComboboxEnum(const char* Label, UEnum* Enum, int64 CurrentValue, int64& NewValue)
{
    bool HasChanged = false;

    FString CurrentEntryName = Enum->GetNameStringByValue(CurrentValue);

    if (ImGui::BeginCombo(Label, TCHAR_TO_ANSI(*CurrentEntryName)))
    {
        for (int32 EnumIndex = 0; EnumIndex < Enum->NumEnums() - 1; ++EnumIndex)
        {

#if WITH_EDITORONLY_DATA
            bool bShouldBeHidden = Enum->HasMetaData(TEXT("Hidden"), EnumIndex) || Enum->HasMetaData(TEXT("Spacer"), EnumIndex);
            if (bShouldBeHidden)
                continue;
#endif //WITH_EDITORONLY_DATA

            FString EnumEntryName = Enum->GetNameStringByIndex(EnumIndex);
            int64 EnumEntryValue = Enum->GetValueByIndex(EnumIndex);

            bool IsSelected = EnumEntryName == CurrentEntryName;

            if (ImGui::Selectable(TCHAR_TO_ANSI(*EnumEntryName), IsSelected))
            {
                HasChanged = true;
                NewValue = EnumEntryValue;
            }

            if (IsSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::CheckBoxState(const char* Label, ECheckBoxState& State)
{
    const char* TooltipText = nullptr;
    ImVec4 ButtonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    ImVec4 TextColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);

    switch (State)
    {
        case ECheckBoxState::Checked:
        {
            TooltipText = "Checked";
            break;
        }

        case ECheckBoxState::Unchecked:
        {
            ButtonColor.w = 0.5f;
            TextColor.w = 0.5f;
            TooltipText = "Unchecked";
            break;
        }

        case ECheckBoxState::Undetermined:
        {
            ButtonColor.w = 0.1f;
            TextColor.w = 0.1f;
            TooltipText = "Undetermined";
            break;
        }
    }

    ImGui::PushStyleColor(ImGuiCol_Text,            TextColor);
    ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(ButtonColor.x, ButtonColor.y, ButtonColor.z, ButtonColor.w * 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   ImVec4(ButtonColor.x, ButtonColor.y, ButtonColor.z, ButtonColor.w * 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,    ImVec4(ButtonColor.x, ButtonColor.y, ButtonColor.z, ButtonColor.w * 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border,          ImVec4(1.0f, 1.0f, 1.0f, 0.1f));

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

    const bool Pressed = ImGui::Button(Label);

    if (State == ECheckBoxState::Unchecked)
    {
        ImVec2 Pos = ImGui::GetItemRectMin();
        ImVec2 Size = ImGui::GetItemRectSize();

        ImGui::GetWindowDrawList()->AddLine(ImVec2(Pos.x, Pos.y + Size.y * 0.5f), ImVec2(Pos.x + Size.x, Pos.y + Size.y * 0.5f), IM_COL32(255, 255, 255, 255), 0.0f);
    }

    ImGui::PopStyleVar();

    ImGui::PopStyleColor(5);

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip(TooltipText);
    }

    if (Pressed)
    {
        switch (State)
        {
            case ECheckBoxState::Checked:       State = ECheckBoxState::Unchecked; break;
            case ECheckBoxState::Unchecked:     State = ECheckBoxState::Undetermined; break;
            case ECheckBoxState::Undetermined:  State = ECheckBoxState::Checked; break;
        }
    }

    return Pressed;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::InputKey(const char* Label, FCogImGuiKeyInfo& KeyInfo)
{
    ImGui::PushID(Label);
   
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(Label);

    const bool HasChanged = InputKey(KeyInfo);

    ImGui::PopID();

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::InputKey(FCogImGuiKeyInfo& KeyInfo)
{
    static TArray<FKey> AllKeys;
    if (AllKeys.IsEmpty())
    {
        EKeys::GetAllKeys(AllKeys);
    }
    
    bool HasKeyChanged = false;

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
    if (ImGui::BeginCombo("##Key", TCHAR_TO_ANSI(*KeyInfo.Key.ToString()), ImGuiComboFlags_HeightLarge))
    {
        for (int32 i = 0; i < AllKeys.Num(); ++i)
        {
            const FKey Key = AllKeys[i];
            if (Key.IsDigital() == false || Key.IsDeprecated() || Key.IsBindableToActions() == false || Key.IsMouseButton() || Key.IsTouch() || Key.IsGamepadKey())
            {
                continue;
            }

            bool IsSelected = KeyInfo.Key == Key;
            if (ImGui::Selectable(TCHAR_TO_ANSI(*Key.ToString()), IsSelected))
            {
                KeyInfo.Key = Key;
                HasKeyChanged = true;
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    HasKeyChanged |= CheckBoxState("Ctrl", KeyInfo.Ctrl);

    ImGui::SameLine();
    HasKeyChanged |= CheckBoxState("Shift", KeyInfo.Shift);

    ImGui::SameLine();
    HasKeyChanged |= CheckBoxState("Alt", KeyInfo.Alt);

    ImGui::SameLine();
    HasKeyChanged |= CheckBoxState("Cmd", KeyInfo.Cmd);

    return HasKeyChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::KeyBind(FKeyBind& KeyBind)
{
    static char Buffer[256] = "";

    const char* Str = TCHAR_TO_ANSI(*KeyBind.Command);
    ImStrncpy(Buffer, Str, IM_ARRAYSIZE(Buffer));

    bool HasChanged = false;
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
    if (ImGui::InputText("##Command", Buffer, IM_ARRAYSIZE(Buffer)))
    {
        KeyBind.Command = FString(Buffer);
        HasChanged = true;
    }

    FCogImGuiKeyInfo KeyInfo;
    FCogImguiInputHelper::KeyBindToKeyInfo(KeyBind, KeyInfo);

    ImGui::SameLine();
    if (InputKey(KeyInfo))
    {
        HasChanged = true;
        FCogImguiInputHelper::KeyInfoToKeyBind(KeyInfo, KeyBind);
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::ButtonWithTooltip(const char* Text, const char* Tooltip)
{
    bool IsPressed = ImGui::Button(Text);

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip(Tooltip);
    }

    return IsPressed;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::DeleteArrayItemButton()
{
    bool IsPressed = ImGui::Button("x");

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("Delete Item");
    }

    return IsPressed;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::MultiChoiceButton(const char* Label, bool IsSelected, const ImVec2& Size)
{
    if (IsSelected)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(128, 128, 128, 50));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(128, 128, 128, 100));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(128, 128, 128, 150));
    }

    const bool IsPressed = ImGui::Button(Label, Size);

    if (IsSelected)
    {
        ImGui::PopStyleVar();
    }
    else
    {
        ImGui::PopStyleColor(3);
    }

    return IsPressed;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::MultiChoiceButtonsInt(TArray<int32>& Values, int32& Value, const ImVec2& Size)
{
    ImGuiStyle& Style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(Style.WindowPadding.x * 0.40f, (float)(int)(Style.WindowPadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(Style.FramePadding.x * 0.40f, (float)(int)(Style.FramePadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(Style.ItemSpacing.x * 0.30f, (float)(int)(Style.ItemSpacing.y * 0.60f)));
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 255, 255, 180));

    bool IsPressed = false;
    for (int32 i = 0; i < Values.Num(); ++i)
    {
        int32 ButtonValue = Values[i];

        const char* Text = TCHAR_TO_ANSI(*FString::Printf(TEXT("%d"), ButtonValue));
        if (MultiChoiceButton(Text, ButtonValue == Value, Size))
        {
            IsPressed = true;
            Value = ButtonValue;
        }

        if (i < Values.Num() - 1)
        {
            ImGui::SameLine();
        }
    }

    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(3);

    return IsPressed;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::MultiChoiceButtonsFloat(TArray<float>& Values, float& Value, const ImVec2& Size)
{
    ImGuiStyle& Style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(Style.WindowPadding.x * 0.40f, (float)(int)(Style.WindowPadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(Style.FramePadding.x * 0.40f, (float)(int)(Style.FramePadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(Style.ItemSpacing.x * 0.30f, (float)(int)(Style.ItemSpacing.y * 0.60f)));
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 255, 255, 180));

    bool IsPressed = false;
    for (int32 i = 0; i < Values.Num(); ++i)
    {
        float ButtonValue = Values[i];

        const char* Text = TCHAR_TO_ANSI(*FString::Printf(TEXT("%g"), ButtonValue).Replace(TEXT("0."), TEXT(".")));
        if (MultiChoiceButton(Text, ButtonValue == Value, Size))
        {
            IsPressed = true;
            Value = ButtonValue;
        }

        if (i < Values.Num() - 1)
        {
            ImGui::SameLine();
        }
    }

    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(3);

    return IsPressed;
}