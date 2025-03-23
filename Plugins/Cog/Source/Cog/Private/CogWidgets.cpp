#include "CogWidgets.h"

#include "CogDebug.h"
#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "CogHelper.h"
#include "Components/PrimitiveComponent.h"
#include "EngineUtils.h"
#include "imgui.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerInput.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "InputCoreTypes.h"

#if WITH_EDITOR
#include "IAssetTools.h"
#include "Subsystems/AssetEditorSubsystem.h"
#endif

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::BeginTableTooltip()
{
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(29, 42, 62, 240));
    if (ImGui::BeginTooltip() == false)
    {
        EndTableTooltip();
        return false;
    }
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::EndTableTooltip()
{
    ImGui::EndTooltip();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::BeginItemTooltipWrappedText()
{
    const bool result = ImGui::BeginItemTooltip();
    if (result == false)
    { return false; }
    
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::EndItemTooltipWrappedText()
{
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
}
    
//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::ItemTooltipWrappedText(const char* InText)
{
    const bool result = BeginItemTooltipWrappedText();
    if (result)
    {
        ImGui::TextUnformatted(InText);
        EndItemTooltipWrappedText();
    }
    return result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::BeginItemTableTooltip()
{
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayShort) == false)
    { return false; }

    return BeginTableTooltip();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::EndItemTableTooltip()
{
    return EndTableTooltip();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::ThinSeparatorText(const char* Label)
{
    ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextBorderSize, 2);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));

    ImGui::SeparatorText(Label);

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::DarkCollapsingHeader(const char* InLabel, ImGuiTreeNodeFlags InFlags)
{
    ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(66, 66, 66, 79));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(62, 62, 62, 204));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(86, 86, 86, 255));
    const bool open = ImGui::CollapsingHeader(InLabel, InFlags);
    ImGui::PopStyleColor(3);
    return open;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::ProgressBarCentered(float Fraction, const ImVec2& Size, const char* Overlay)
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
    ImGui::RenderRectFilledRangeH(window->DrawList, bb, ImGui::GetColorU32(ImGuiCol_PlotHistogram), 0.0f, Fraction, style.FrameRounding);

    // Default displaying the fraction as percentage string, but user can override it
    char overlay_buf[32];
    if (!Overlay)
    {
        ImFormatString(overlay_buf, IM_ARRAYSIZE(overlay_buf), "%.0f%%", Fraction * 100 + 0.01f);
        Overlay = overlay_buf;
    }

    ImVec2 overlay_size = ImGui::CalcTextSize(Overlay, nullptr);
    if (overlay_size.x > 0.0f)
    {

        ImVec2 pos1(ImClamp(bb.Min.x + (bb.Max.x - bb.Min.x) * 0.5f - overlay_size.x * 0.5f + style.ItemSpacing.x, bb.Min.x, bb.Max.x - style.ItemInnerSpacing.x), bb.Min.y - 1);
        ImVec2 pos2(pos1.x + 1, pos1.y + 1);

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
        ImGui::RenderTextClipped(pos2, bb.Max, Overlay, nullptr, &overlay_size, ImVec2(0.0f, 0.5f), &bb);
        ImGui::PopStyleColor();

        ImGui::RenderTextClipped(pos1, bb.Max, Overlay, nullptr, &overlay_size, ImVec2(0.0f, 0.5f), &bb);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::ToggleMenuButton(bool* Value, const char* Text, const ImVec4& TrueColor)
{
    bool IsTrue = *Value;
    if (IsTrue)
    {
        ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(TrueColor.x, TrueColor.y, TrueColor.z, TrueColor.w * 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   ImVec4(TrueColor.x, TrueColor.y, TrueColor.z, TrueColor.w * 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,    ImVec4(TrueColor.x, TrueColor.y, TrueColor.z, TrueColor.w * 1.0f));
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    }

    bool IsPressed = ImGui::Button(Text);
    if (IsPressed)
    {
        *Value = !*Value;
    }
    
    if (IsTrue)
    {
        ImGui::PopStyleColor(3);
    }
    else
    {
        ImGui::PopStyleColor(1);
    }

    return IsPressed;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::ToggleButton(bool* Value, const char* Text, const ImVec4& TrueColor, const ImVec4& FalseColor, const ImVec2& Size)
{
    return ToggleButton(Value, Text, Text, TrueColor, FalseColor, Size);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::ToggleButton(bool* Value, const char* TextTrue, const char* TextFalse, const ImVec4& TrueColor, const ImVec4& FalseColor, const ImVec2& Size)
{
    bool IsPressed = false;
    if (*Value)
    {
        ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(TrueColor.x, TrueColor.y, TrueColor.z, TrueColor.w * 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   ImVec4(TrueColor.x, TrueColor.y, TrueColor.z, TrueColor.w * 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,    ImVec4(TrueColor.x, TrueColor.y, TrueColor.z, TrueColor.w * 1.0f));

        IsPressed = ImGui::Button(TextTrue, Size);
        if (IsPressed)
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

    return IsPressed;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::SliderWithReset(const char* Name, float* Value, float Min, float Max, const float& ResetValue, const char* Format)
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
void FCogWidgets::HelpMarker(const char* Text)
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
void FCogWidgets::PushStyleCompact()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.WindowPadding.x * 0.60f, static_cast<int>(style.WindowPadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x * 0.60f, static_cast<int>(style.FramePadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x * 0.60f, static_cast<int>(style.ItemSpacing.y * 0.60f)));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::PopStyleCompact()
{
    ImGui::PopStyleVar(3);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::AddTextWithShadow(ImDrawList* DrawList, const ImVec2& Position, ImU32 Color, const char* TextBegin, const char* TextEnd /*= NULL*/)
{
    float Alpha = ImGui::ColorConvertU32ToFloat4(Color).w;
    DrawList->AddText(Position + ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, Alpha)), TextBegin, TextEnd);
    DrawList->AddText(Position, Color, TextBegin, TextEnd);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::SearchBar(const char* InLabel, ImGuiTextFilter& InFilter, float InWidth /*= -1*/)
{
    if (InWidth != 0.0f)
    {
        ImGui::SetNextItemWidth(InWidth);
    }
    //
    bool value_changed = ImGui::InputTextWithHint(InLabel, "Search", InFilter.InputBuf, IM_ARRAYSIZE(InFilter.InputBuf));
    if (value_changed)
    {
        InFilter.Build();
    }
    return value_changed;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::PushButtonBackColor(const ImVec4& Color)
{
    ImGui::PushStyleColor(ImGuiCol_Button,              ImVec4(Color.x, Color.y, Color.z, Color.w * 0.25f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,       ImVec4(Color.x, Color.y, Color.z, Color.w * 0.3f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,        ImVec4(Color.x, Color.y, Color.z, Color.w * 0.5f));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::PopButtonBackColor()
{
    ImGui::PopStyleColor(3);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::PushFrameBackColor(const ImVec4& Color)
{
    ImGui::PushStyleColor(ImGuiCol_FrameBg,             ImVec4(Color.x, Color.y, Color.z, Color.w * 0.25f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,      ImVec4(Color.x, Color.y, Color.z, Color.w * 0.3f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,       ImVec4(Color.x, Color.y, Color.z, Color.w * 0.5f));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::PopFrameBackColor()
{
    ImGui::PopStyleColor(3);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::PushSliderBackColor(const ImVec4& Color)
{
    ImGui::PushStyleColor(ImGuiCol_SliderGrab,          ImVec4(Color.x, Color.y, Color.z, Color.w * 0.8f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive,    ImVec4(Color.x, Color.y, Color.z, Color.w * 1.0f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark,           ImVec4(Color.x, Color.y, Color.z, Color.w * 0.8f));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::PopSliderBackColor()
{
    ImGui::PopStyleColor(3);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::PushBackColor(const ImVec4& Color)
{
    PushButtonBackColor(Color);
    PushFrameBackColor(Color);
    PushSliderBackColor(Color);
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::PopBackColor()
{
    PopSliderBackColor();
    PopFrameBackColor();
    PopButtonBackColor();
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogWidgets::GetShortWidth()
{
    return ImGui::GetFontSize() * 10;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::SetNextItemToShortWidth()
{
    ImGui::SetNextItemWidth(GetShortWidth());
}

//--------------------------------------------------------------------------------------------------------------------------
float FCogWidgets::GetFontWidth()
{
    return ImGui::GetFontSize() * 0.5f;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::ComboboxEnum(const char* Label, const UObject* Object, const char* FieldName, uint8* PointerToEnumValue)
{
    const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Object->GetClass()->FindPropertyByName(FName(FieldName)));
    if (EnumProperty == nullptr)
    {
        return false;
    }

    return ComboboxEnum(Label, EnumProperty, PointerToEnumValue);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::ComboboxEnum(const char* Label, const FEnumProperty* EnumProperty, uint8* PointerToValue)
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
bool FCogWidgets::ComboboxEnum(const char* Label, const UEnum* Enum, int64 CurrentValue, int64& NewValue)
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
bool FCogWidgets::CheckBoxState(const char* Label, ECheckBoxState& State, bool ShowTooltip)
{
    const char* TooltipText = "Invalid";
    
    int32 Flags = 0;
    ImVec4 ButtonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    ImVec4 TextColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);

    switch (State)
    {
        case ECheckBoxState::Unchecked:
        {
            TooltipText = "Unchecked";
            Flags = 0;
            break;
        }

        case ECheckBoxState::Checked:
        {
            TooltipText = "Checked";
            Flags = 3;
            break;
        }

        case ECheckBoxState::Undetermined:
        {
            ButtonColor.w = 0.1f;
            TextColor.w = 0.1f;
            TooltipText = "Undetermined";
            Flags = 1;
            break;
        }
    }

    ImGui::PushStyleColor(ImGuiCol_Text,             TextColor);
    ImGui::PushStyleColor(ImGuiCol_FrameBg,          ImVec4(ButtonColor.x, ButtonColor.y, ButtonColor.z, ButtonColor.w * 0.6f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,   ImVec4(ButtonColor.x, ButtonColor.y, ButtonColor.z, ButtonColor.w * 0.8f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,    ImVec4(ButtonColor.x, ButtonColor.y, ButtonColor.z, ButtonColor.w * 1.0f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark,        ImVec4(ButtonColor.x, ButtonColor.y, ButtonColor.z, ButtonColor.w * 1.0f));

    const bool IsPressed = ImGui::CheckboxFlags(Label, &Flags, 3);

    ImGui::PopStyleColor(5);

    if (ShowTooltip)
    {
        ImGui::SetItemTooltip("%s", TooltipText);
    }

    if (IsPressed)  
    {
        switch (State) 
        {
            case ECheckBoxState::Checked:       State = ECheckBoxState::Unchecked; break;
            case ECheckBoxState::Unchecked:     State = ECheckBoxState::Undetermined; break;
            case ECheckBoxState::Undetermined:  State = ECheckBoxState::Checked; break;
        }
    }

    return IsPressed;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::InputChord(const char* Label, FInputChord& InInputChord)
{
    ImGui::PushID(Label);
   
    ImGui::AlignTextToFramePadding();
    ImGui::BeginDisabled();
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
    ImGui::InputText("##Shortcut", const_cast<char*>(Label), IM_ARRAYSIZE(Label));
    ImGui::EndDisabled();

    ImGui::SameLine();
    const bool HasChanged = InputChord(InInputChord);

    ImGui::PopID();

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::InputChord(FInputChord& InInputChord)
{
    bool HasKeyChanged = false;

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
    HasKeyChanged |= Key(InInputChord.Key);

    bool Value = false;
    
    ImGui::SameLine();
    Value = static_cast<bool>(InInputChord.bCtrl);
    HasKeyChanged |=  ImGui::Selectable("Ctrl", &Value, ImGuiSelectableFlags_None, ImVec2(ImGui::GetFontSize() * 2, 0));
    InInputChord.bCtrl = Value;

    ImGui::SameLine();
    Value = static_cast<bool>(InInputChord.bShift);
    HasKeyChanged |= ImGui::Selectable("Shift", &Value, ImGuiSelectableFlags_None, ImVec2(ImGui::GetFontSize() * 3, 0));
    InInputChord.bShift = Value;

    ImGui::SameLine();
    Value = static_cast<bool>(InInputChord.bAlt);
    HasKeyChanged |= ImGui::Selectable("Alt", &Value, ImGuiSelectableFlags_None, ImVec2(ImGui::GetFontSize() * 2, 0));
    InInputChord.bAlt = Value;

    ImGui::SameLine();
    Value = static_cast<bool>(InInputChord.bCmd);
    HasKeyChanged |= ImGui::Selectable("Cmd", &Value, ImGuiSelectableFlags_None, ImVec2(ImGui::GetFontSize() * 2, 0));
    InInputChord.bCmd = Value;

    return HasKeyChanged;
}


//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::Key(FKey& InKey)
{
    bool HasKeyChanged = false;
    if (ImGui::BeginCombo("##Key", TCHAR_TO_ANSI(*InKey.ToString()), ImGuiComboFlags_HeightLarge))
    {
        {
            bool IsSelected = InKey == FKey();
            if (ImGui::Selectable("None", IsSelected))
            {
                InKey = FKey();
                HasKeyChanged = true;
            }
        }

        ImGui::Separator();
        
        static TArray<FKey> AllKeys;
        if (AllKeys.IsEmpty())
        {
            EKeys::GetAllKeys(AllKeys);
        }
        
        for (int32 i = 0; i < AllKeys.Num(); ++i)
        {
            const FKey Key = AllKeys[i];
            if (Key.IsDigital() == false || Key.IsDeprecated() || Key.IsBindableToActions() == false || Key.IsMouseButton() || Key.IsTouch() || Key.IsGamepadKey())
            {
                continue;
            }

            bool IsSelected = InKey == Key;
            if (ImGui::Selectable(TCHAR_TO_ANSI(*Key.ToString()), IsSelected))
            {
                InKey = Key;
                HasKeyChanged = true;
            }
        }
        ImGui::EndCombo();
    }

    return HasKeyChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::KeyBind(FKeyBind& InKeyBind)
{
    static char Buffer[256] = "";

    const auto Str = StringCast<ANSICHAR>(*InKeyBind.Command);
    ImStrncpy(Buffer, Str.Get(), IM_ARRAYSIZE(Buffer));

    bool Disable = !InKeyBind.bDisabled; 
    if (ImGui::Checkbox("##Disable", &Disable))
    {
        InKeyBind.bDisabled = !Disable;
    }
    if (InKeyBind.bDisabled)
    {
        ImGui::SetItemTooltip("Enable command");
    }
    else
    {
        ImGui::SetItemTooltip("Disable command");
    }        

    if (InKeyBind.bDisabled)
    {
        ImGui::BeginDisabled();
    }

    ImGui::SameLine();

    bool HasChanged = false;
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
    if (ImGui::InputText("##Command", Buffer, IM_ARRAYSIZE(Buffer)))
    {
        InKeyBind.Command = FString(Buffer);
        HasChanged = true;
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
    HasChanged |= Key(InKeyBind.Key);

    {
        ImGui::SameLine();
        ECheckBoxState State = FCogImguiInputHelper::MakeCheckBoxState(InKeyBind.Control, InKeyBind.bIgnoreCtrl);
        HasChanged |= CheckBoxState("Ctrl", State);
        BREAK_CHECKBOX_STATE(State, InKeyBind.Control, InKeyBind.bIgnoreCtrl);
    }

    {
        ImGui::SameLine();
        ECheckBoxState State = FCogImguiInputHelper::MakeCheckBoxState(InKeyBind.Shift, InKeyBind.bIgnoreShift);
        HasChanged |= CheckBoxState("Shift", State);
        BREAK_CHECKBOX_STATE(State, InKeyBind.Shift, InKeyBind.bIgnoreShift);
    }

    {
        ImGui::SameLine();
        ECheckBoxState State = FCogImguiInputHelper::MakeCheckBoxState(InKeyBind.Alt, InKeyBind.bIgnoreAlt);
        HasChanged |= CheckBoxState("Alt", State);
        BREAK_CHECKBOX_STATE(State, InKeyBind.Alt, InKeyBind.bIgnoreAlt);
    }

    {
        ImGui::SameLine();
        ECheckBoxState State = FCogImguiInputHelper::MakeCheckBoxState(InKeyBind.Cmd, InKeyBind.bIgnoreCmd);
        HasChanged |= CheckBoxState("Cmd", State);
        BREAK_CHECKBOX_STATE(State, InKeyBind.Cmd, InKeyBind.bIgnoreCmd);
    }

    if (InKeyBind.bDisabled)
    {
        ImGui::EndDisabled();
    }
        
    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::ButtonWithTooltip(const char* Text, const char* Tooltip)
{
    bool IsPressed = ImGui::Button(Text);

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("%s", Tooltip);
    }

    return IsPressed;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::DeleteArrayItemButton()
{
    bool IsPressed = ImGui::Button("x");

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("Delete Item");
    }

    return IsPressed;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::MultiChoiceButton(const char* Label, bool IsSelected, const ImVec2& Size)
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
bool FCogWidgets::MultiChoiceButtonsInt(TArray<int32>& InValues, int32& InCurrentValue, const ImVec2& InSize, bool InInline)
{
    ImGuiStyle& Style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(Style.WindowPadding.x * 0.40f, static_cast<int>(Style.WindowPadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(Style.FramePadding.x * 0.40f, static_cast<int>(Style.FramePadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(Style.ItemSpacing.x * 0.30f, static_cast<int>(Style.ItemSpacing.y * 0.60f)));
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 255, 255, 180));

    bool IsPressed = false;
    for (int32 i = 0; i < InValues.Num(); ++i)
    {
        int32 ButtonValue = InValues[i];

        const auto Text = StringCast<ANSICHAR>(*FString::Printf(TEXT("%d"), ButtonValue));
        if (MultiChoiceButton(Text.Get(), ButtonValue == InCurrentValue, InSize))
        {
            IsPressed = true;
            InCurrentValue = ButtonValue;
        }

        if (InInline && i < InValues.Num() - 1)
        {
            ImGui::SameLine();
        }
    }

    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(3);

    return IsPressed;
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogWidgets::RemoveFirstZero(const FString& InText)
{
    return InText.Replace(TEXT("0."), TEXT("."));
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogWidgets::FormatSmallFloat(float InValue)
{
    return RemoveFirstZero(FString::Printf(TEXT("%g"), InValue));
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::MultiChoiceButtonsFloat(TArray<float>& InValues, float& InValue, const ImVec2& InSize, bool InInline, float InTolerance)
{
    ImGuiStyle& Style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(Style.WindowPadding.x * 0.40f, static_cast<int>(Style.WindowPadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(Style.FramePadding.x * 0.40f, static_cast<int>(Style.FramePadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(Style.ItemSpacing.x * 0.30f, static_cast<int>(Style.ItemSpacing.y * 0.60f)));
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 255, 255, 180));

    bool IsPressed = false;
    for (int32 i = 0; i < InValues.Num(); ++i)
    {
        float ButtonValue = InValues[i];

        const auto Text = StringCast<ANSICHAR>(*FormatSmallFloat(ButtonValue));

        ImGui::PushID(i);
        if (MultiChoiceButton(Text.Get(), FMath::IsNearlyEqual(ButtonValue, InValue, InTolerance) , InSize))
        {
            IsPressed = true;
            InValue = ButtonValue;
        }
        ImGui::PopID();

        if (InInline && i < InValues.Num() - 1)
        {
            ImGui::SameLine();
        }
    }

    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(3);

    return IsPressed;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::ComboTraceChannel(const char* Label, ECollisionChannel& Channel)
{
    FColor ChannelColors[ECC_MAX];
    FCogDebug::GetDebugChannelColors(ChannelColors);

    const UCollisionProfile* CollisionProfile = UCollisionProfile::Get();
    if (CollisionProfile == nullptr)
    { return false; }

    const FName SelectedChannelName = CollisionProfile->ReturnChannelNameFromContainerIndex(Channel);
    
    bool Result = false;
    if (ImGui::BeginCombo(Label, TCHAR_TO_ANSI(*SelectedChannelName.ToString()), ImGuiComboFlags_HeightLarge))
    {
        for (int32 ChannelIndex = 0; ChannelIndex < static_cast<int32>(ECC_OverlapAll_Deprecated); ++ChannelIndex)
        {
            if (CollisionProfile->ConvertToTraceType(static_cast<ECollisionChannel>(ChannelIndex)) == TraceTypeQuery_MAX)
            { continue; }
            
            ImGui::PushID(ChannelIndex);

            FColor Color = ChannelColors[ChannelIndex];
            FCogImguiHelper::ColorEdit4("Color", Color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();

            const FName ChannelName = CollisionProfile->ReturnChannelNameFromContainerIndex(ChannelIndex);
            if (ChannelName.IsValid())
            {
                if (ImGui::Selectable(TCHAR_TO_ANSI(*ChannelName.ToString())))
                {
                    Channel = static_cast<ECollisionChannel>(ChannelIndex);
                    Result = true;
                }
            }

            ImGui::PopID();
        }
        ImGui::EndCombo();
    }

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::CollisionProfileChannel(const UCollisionProfile& InCollisionProfile, const int32 InChannelIndex, FColor& InChannelColor, int32& InChannels)
{
    bool Result = false;

    FCogImguiHelper::ColorEdit4("Color", InChannelColor, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    ImGui::SameLine();

    bool IsCollisionActive = (InChannels & ECC_TO_BITFIELD(InChannelIndex)) > 0;
    const FName ChannelName = InCollisionProfile.ReturnChannelNameFromContainerIndex(InChannelIndex);
    if (ImGui::Checkbox(TCHAR_TO_ANSI(*ChannelName.ToString()), &IsCollisionActive))
    {
        Result = true;

        if (IsCollisionActive)
        {
            InChannels |= ECC_TO_BITFIELD(InChannelIndex);
        }
        else
        {
            InChannels &= ~ECC_TO_BITFIELD(InChannelIndex);
        }
    }

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::CollisionProfileChannels(int32& OutChannels)
{
    bool Result = false;
    ImGui::SeparatorText("Trace Type");
    Result |= CollisionTraceChannels(OutChannels);
    ImGui::SeparatorText("Object Type");
    Result |= CollisionObjectTypeChannels(OutChannels);
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::CollisionTraceChannels(int32& OutChannels)
{
    const UCollisionProfile* CollisionProfile = UCollisionProfile::Get();
    if (CollisionProfile == nullptr)
    { return false; }
    
    FColor ChannelColors[ECC_MAX];
    FCogDebug::GetDebugChannelColors(ChannelColors);

    bool Result = false;

    for (int32 ChannelIndex = 0; ChannelIndex < static_cast<int32>(ECC_OverlapAll_Deprecated); ++ChannelIndex)
    {
        if (CollisionProfile->ConvertToTraceType(static_cast<ECollisionChannel>(ChannelIndex)) == TraceTypeQuery_MAX)
        { continue; }
        
        ImGui::PushID(ChannelIndex);
        FColor Color = ChannelColors[ChannelIndex];
        Result |= CollisionProfileChannel(*CollisionProfile, ChannelIndex, Color, OutChannels);
        ImGui::PopID();
    }

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::CollisionObjectTypeChannels(int32& OutChannels)
{
    const UCollisionProfile* CollisionProfile = UCollisionProfile::Get();
    if (CollisionProfile == nullptr)
    { return false; }
    
    FColor ChannelColors[ECC_MAX];
    FCogDebug::GetDebugChannelColors(ChannelColors);

    bool Result = false;

    for (int32 ChannelIndex = 0; ChannelIndex < static_cast<int32>(ECC_OverlapAll_Deprecated); ++ChannelIndex)
    {
        if (CollisionProfile->ConvertToObjectType(static_cast<ECollisionChannel>(ChannelIndex)) == TraceTypeQuery_MAX)
        { continue; }
        
        ImGui::PushID(ChannelIndex);
        FColor Color = ChannelColors[ChannelIndex];
        Result |= CollisionProfileChannel(*CollisionProfile, ChannelIndex, Color, OutChannels);
        ImGui::PopID();
    }

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::ActorsListWithFilters(AActor*& NewSelection, const UWorld& World, const TArray<TSubclassOf<AActor>>& ActorClasses, int32& SelectedActorClassIndex, ImGuiTextFilter* Filter, const APawn* LocalPlayerPawn, const FCogWindowActorContextMenuFunction& ContextMenuFunction)
{
    TSubclassOf<AActor> SelectedClass = AActor::StaticClass();
    if (ActorClasses.IsValidIndex(SelectedActorClassIndex))
    {
        SelectedClass = ActorClasses[SelectedActorClassIndex];
    }

    bool AddSeparator = false;

    //------------------------
    // Actor Class Combo
    //------------------------
    if (ActorClasses.Num() > 1)
    {
        ImGui::SetNextItemWidth(18 * GetFontWidth());
        if (ImGui::BeginCombo("##SelectionType", TCHAR_TO_ANSI(*GetNameSafe(SelectedClass))))
        {
            for (int32 i = 0; i < ActorClasses.Num(); ++i)
            {
                TSubclassOf<AActor> SubClass = ActorClasses[i];
                if (ImGui::Selectable(TCHAR_TO_ANSI(*GetNameSafe(SubClass)), false))
                {
                    SelectedActorClassIndex = i;
                    SelectedClass = SubClass;
                }
            }
            ImGui::EndCombo();
        }

        AddSeparator = true;
    }

    if (Filter != nullptr)
    {
        ImGui::SameLine();
        SearchBar("##Filter", *Filter);
        AddSeparator = true;
    }

    if (AddSeparator)
    {
        ImGui::Separator();
    }

    //------------------------
    // Actor List
    //------------------------
    ImGui::BeginChild("ActorsList", ImVec2(-1, -1), false);
    const bool SelectionChanged = ActorsList(NewSelection, World, SelectedClass, Filter, LocalPlayerPawn, ContextMenuFunction);
    ImGui::EndChild();

    return SelectionChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::ActorsList(AActor*& NewSelection, const UWorld& World, const TSubclassOf<AActor>& ActorClass, const ImGuiTextFilter* Filter, const APawn* LocalPlayerPawn, const FCogWindowActorContextMenuFunction& ContextMenuFunction)
{
    TArray<AActor*> Actors;
    for (TActorIterator It(&World, ActorClass); It; ++It)
    {
        AActor* Actor = *It;

        bool AddActor = true;
        if (Filter != nullptr && Filter->IsActive())
        {
            const auto ActorName = StringCast<ANSICHAR>(*FCogHelper::GetActorName(*Actor));
            if (Filter != nullptr && Filter->PassFilter(ActorName.Get()) == false)
            {
                AddActor = false;
            }
        }

        if (AddActor)
        {
            Actors.Add(Actor);
        }
    }

    AActor* OldSelection = FCogDebug::GetSelection();
    NewSelection = OldSelection;

    ImGuiListClipper Clipper;
    Clipper.Begin(Actors.Num());
    while (Clipper.Step())
    {
        for (int32 i = Clipper.DisplayStart; i < Clipper.DisplayEnd; i++)
        {
            AActor* Actor = Actors[i];
            if (Actor == nullptr)
            {
                continue;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, Actor == LocalPlayerPawn ? IM_COL32(255, 255, 0, 255) : IM_COL32(255, 255, 255, 255));

            const bool bIsSelected = Actor == FCogDebug::GetSelection();
            if (ImGui::Selectable(TCHAR_TO_ANSI(*FCogHelper::GetActorName(*Actor)), bIsSelected))
            {
                //FCogDebug::SetSelection(&World, Actor);
                NewSelection = Actor;
            }

            ImGui::PopStyleColor(1);

            
            ActorContextMenu(*Actor, ContextMenuFunction);

            //------------------------
            // Draw Frame
            //------------------------
            if (ImGui::IsItemHovered())
            {
                ActorFrame(*Actor);
            }

            if (bIsSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
    }
    Clipper.End();

    return NewSelection != OldSelection;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::MenuActorsCombo(const char* StrID, AActor*& NewSelection, const UWorld& World, const TSubclassOf<AActor>& ActorClass, const FCogWindowActorContextMenuFunction& ContextMenuFunction)
{
    int32 SelectedActorClassIndex = 0;
    const TArray ActorClasses = { ActorClass };

    return MenuActorsCombo(StrID, NewSelection, World, ActorClasses, SelectedActorClassIndex, nullptr, nullptr, ContextMenuFunction);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::MenuActorsCombo(const char* StrID, AActor*& NewSelection, const UWorld& World, const TArray<TSubclassOf<AActor>>& ActorClasses, int32& SelectedActorClassIndex, ImGuiTextFilter* Filter, const APawn* LocalPlayerPawn, const FCogWindowActorContextMenuFunction& ContextMenuFunction)
{
    bool Result = false;
    ImGui::PushID(StrID);

    const ImVec2 Pos1 = ImGui::GetCursorScreenPos();
    const float Width = FCogImguiHelper::GetNextItemWidth();

    //-----------------------------------
    // Combo button
    //-----------------------------------
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));

        TSubclassOf<AActor> SelectedClass = AActor::StaticClass();
        if (ActorClasses.IsValidIndex(SelectedActorClassIndex))
        {
            SelectedClass = ActorClasses[SelectedActorClassIndex];
        }

        AActor* Selection = FCogDebug::GetSelection();
        if (Selection != nullptr && Selection->IsA(SelectedClass) == false)
        {
            Selection = nullptr;
        }

        const FString CurrentSelectionName = FCogHelper::GetActorName(Selection);
        if (ImGui::Button(TCHAR_TO_ANSI(*CurrentSelectionName), ImVec2(Width, 0.0f)))
        {
            ImGui::OpenPopup("ActorListPopup");
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Current Selection: %s", TCHAR_TO_ANSI(*CurrentSelectionName));
        }

        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar(2);

        if (Selection != nullptr)
        {
            ActorContextMenu(*Selection, ContextMenuFunction);
        }
    }

    //-----------------------------------
    // Popup
    //-----------------------------------
    const ImVec2 Pos2 = ImGui::GetCursorScreenPos();
    ImGui::SetNextWindowPos(ImVec2(Pos1.x, Pos1.y + ImGui::GetFrameHeight()));
    if (ImGui::BeginPopup("ActorListPopup"))
    {
        ImGui::BeginChild("Child", ImVec2(Pos2.x - Pos1.x, GetFontWidth() * 40), false);

        Result = ActorsListWithFilters(NewSelection, World, ActorClasses, SelectedActorClassIndex, Filter, LocalPlayerPawn, ContextMenuFunction);
        if (Result)
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndChild();
        ImGui::EndPopup();
    }

    ImGui::PopID();

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::ActorContextMenu(AActor& Selection, const FCogWindowActorContextMenuFunction& ContextMenuFunction)
{
    if (ContextMenuFunction == nullptr)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(GetFontWidth() * 30, 0));
    if (ImGui::BeginPopupContextItem())
    {
        ContextMenuFunction(Selection);
        ImGui::EndPopup();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::ActorFrame(const AActor& Actor)
{
    const APlayerController* PlayerController = Actor.GetWorld()->GetFirstPlayerController();
    if (PlayerController == nullptr)
    {
        return;
    }

    ImGuiViewport* Viewport = ImGui::GetMainViewport();
    if (Viewport == nullptr)
    {
        return;
    }

    ImDrawList* DrawList = ImGui::GetBackgroundDrawList(Viewport);

    FVector BoxOrigin, BoxExtent;

    bool PrimitiveFound = false;
    FBox Bounds(ForceInit);

    if (const UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Actor.GetRootComponent()))
    {
        PrimitiveFound = true;

        Bounds += PrimitiveComponent->Bounds.GetBox();
    }
    else
    {
        Actor.ForEachComponent<UPrimitiveComponent>(true, [&](const UPrimitiveComponent* InPrimComp)
            {
                if (InPrimComp->IsRegistered() && InPrimComp->IsCollisionEnabled())
                {
                    Bounds += InPrimComp->Bounds.GetBox();
                    PrimitiveFound = true;
                }
            });
    }

    if (PrimitiveFound)
    {
        Bounds.GetCenterAndExtents(BoxOrigin, BoxExtent);
    }
    else
    {
        BoxOrigin = Actor.GetActorLocation();
        BoxExtent = FVector::ZeroVector;
    }

    FVector2D ScreenPosMin, ScreenPosMax;
    if (FCogHelper::ComputeBoundingBoxScreenPosition(PlayerController, BoxOrigin, BoxExtent, ScreenPosMin, ScreenPosMax))
    {
        const ImU32 Color = (&Actor == FCogDebug::GetSelection()) ? IM_COL32(255, 255, 255, 255) : IM_COL32(255, 255, 255, 128);
        if (ScreenPosMin != ScreenPosMax)
        {
            DrawList->AddRect(FCogImguiHelper::ToImVec2(ScreenPosMin) + Viewport->Pos, FCogImguiHelper::ToImVec2(ScreenPosMax) + Viewport->Pos, Color, 0.0f, 0, 1.0f);
        }
        AddTextWithShadow(DrawList, FCogImguiHelper::ToImVec2(ScreenPosMin + FVector2D(0, -14.0f)) + Viewport->Pos, Color, TCHAR_TO_ANSI(*FCogHelper::GetActorName(Actor)));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::SmallButton(const char* Text, const ImVec4& Color)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(Color.x, Color.y, Color.z, Color.w * 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(Color.x, Color.y, Color.z, Color.w * 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(Color.x, Color.y, Color.z, Color.w * 1.0f));
    ImGui::SmallButton(Text);
    ImGui::PopStyleColor(3);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::InputText(const char* Text, FString& Value, ImGuiInputTextFlags InFlags, ImGuiInputTextCallback InCallback, void* InUserData)
{
    static char ValueBuffer[256] = "";
    ImStrncpy(ValueBuffer, TCHAR_TO_ANSI(*Value), IM_ARRAYSIZE(ValueBuffer));
    
    bool result = ImGui::InputText(Text, ValueBuffer, IM_ARRAYSIZE(ValueBuffer), InFlags, InCallback, InUserData);
    if (result)
    {
        Value = FString(ValueBuffer);
    }

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::InputTextWithHint(const char* InText, const char* InHint, FString& InValue, ImGuiInputTextFlags InFlags, ImGuiInputTextCallback InCallback, void* InUserData)
{
    static char ValueBuffer[256] = "";
    ImStrncpy(ValueBuffer, TCHAR_TO_ANSI(*InValue), IM_ARRAYSIZE(ValueBuffer));

    bool result = ImGui::InputTextWithHint(InText, InHint, ValueBuffer, IM_ARRAYSIZE(ValueBuffer), InFlags, InCallback, InUserData);
    if (result)
    {
        InValue = FString(ValueBuffer);
    }

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::BeginRightAlign(const char* Id)
{
    if (ImGui::BeginTable(Id, 2, ImGuiTableFlags_SizingFixedFit, ImVec2(-1, 0)))
    {
        ImGui::TableSetupColumn("a", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TableNextColumn();
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::EndRightAlign()
{
    ImGui::EndTable();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::MenuItemShortcut(const char* Id, const FString& Text)
{
    ImGui::SameLine();
    if (BeginRightAlign(Id))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        const auto TextStr = StringCast<ANSICHAR>(*Text);
        ImGui::Text("%s", TextStr.Get());
        ImGui::PopStyleColor();

        EndRightAlign();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::BrowseToAssetButton(const UObject* InAsset, const ImVec2& InSize)
{
#if WITH_EDITOR

    if (InAsset == nullptr)
    {
        ImGui::BeginDisabled();
    }

    const bool result = ImGui::Button("Browse To Asset", InSize);
    if (result)
    {
        IAssetTools::Get().SyncBrowserToAssets({ InAsset });
    }

    if (InAsset == nullptr)
    {
        ImGui::EndDisabled();
    }
    return result;

#else
    return false;
#endif
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::BrowseToAssetButton(const FAssetData& InAssetData, const ImVec2& InSize)
{
#if WITH_EDITOR

    const bool result = ImGui::Button("Browse To Asset", InSize);
    if (result)
    {
        IAssetTools::Get().SyncBrowserToAssets({ InAssetData });
    }

    return result;

#else
    return false;
#endif
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::BrowseToObjectAssetButton(const UObject* InObject, const ImVec2& InSize)
{
#if WITH_EDITOR

    const UObject* ObjectAsset = nullptr;
    if (InObject != nullptr && InObject->GetClass() != nullptr)
    {
        ObjectAsset = InObject->GetClass()->ClassGeneratedBy;
    }

    return BrowseToAssetButton(ObjectAsset, InSize);

#else
    return false;
#endif

}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::OpenAssetButton(const UObject* InAsset, const ImVec2& InSize)
{
#if WITH_EDITOR

    UAssetEditorSubsystem* editorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    if (InAsset == nullptr || editorSubsystem == nullptr)
    {
        ImGui::BeginDisabled();
    }

    const bool result = ImGui::Button("Open Asset", InSize);
    if (result)
    {
        if (editorSubsystem != nullptr)
        {
            editorSubsystem->OpenEditorForAsset(InAsset);
        }
    }

    if (InAsset == nullptr)
    {
        ImGui::EndDisabled();
    }
    return result;

#else
    return false;
#endif
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::OpenObjectAssetButton(const UObject* InObject, const ImVec2& InSize)
{
#if WITH_EDITOR

    const UObject* ObjectAsset = nullptr;
    if (InObject != nullptr && InObject->GetClass() != nullptr)
    {
        ObjectAsset = InObject->GetClass()->ClassGeneratedBy;
    }

    return OpenAssetButton(ObjectAsset, InSize);

#else
    return false;
#endif

}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::FloatArray(const char* InLabel, TArray<float>& InArray, int32 InMaxEntries, const ImVec2& Size)
{
    ScalarArray(InLabel, ImGuiDataType_Float, InArray, InMaxEntries, Size);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::IntArray(const char* InLabel, TArray<int>& InArray, int32 InMaxEntries, const ImVec2& Size)
{
    ScalarArray(InLabel, ImGuiDataType_S32, InArray, InMaxEntries, Size);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::RenderCloseButton(const ImVec2& InPos)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    // Tweak 1: Shrink hit-testing area if button covers an abnormally large proportion of the visible region. That's in order to facilitate moving the window away. (#3825)
    // This may better be applied as a general hit-rect reduction mechanism for all widgets to ensure the area to move window is always accessible?
    const ImRect bb(InPos, InPos + ImVec2(g.FontSize, g.FontSize));
    
    ImU32 cross_col = ImGui::GetColorU32(ImGuiCol_Text);
    ImVec2 cross_center = bb.GetCenter() - ImVec2(0.5f, 0.5f);
    float cross_extent = g.FontSize * 0.5f * 0.7071f - 1.0f;
    window->DrawList->AddLine(cross_center + ImVec2(+cross_extent, +cross_extent), cross_center + ImVec2(-cross_extent, -cross_extent), cross_col, 1.0f);
    window->DrawList->AddLine(cross_center + ImVec2(+cross_extent, -cross_extent), cross_center + ImVec2(-cross_extent, +cross_extent), cross_col, 1.0f);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::PickButton(const char* InLabel, const ImVec2& InSize, ImGuiButtonFlags InFlags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiID id = window->GetID(InLabel);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + InSize);
    const float default_size = ImGui::GetFrameHeight();
    ImGui::ItemSize(InSize, (InSize.y >= default_size) ? g.Style.FramePadding.y : -1.0f);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, InFlags);

    // Render
    const ImU32 bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    const ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
    ImGui::RenderNavCursor(bb, id);
    ImGui::RenderFrame(bb.Min, bb.Max, bg_col, true, g.Style.FrameRounding);

    const ImVec2 center = bb.GetCenter();
    const float radius = window->DrawList->_Data->FontSize;
    window->DrawList->AddCircle(center, radius * 0.4f, text_col, 0, 1);
    window->DrawList->AddCircleFilled(center, radius * 0.15f, text_col);

    return pressed;
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec2 FCogWidgets::ComputeScreenCornerLocation(const FVector2f& InAlignment, const FIntVector2& InPadding)
{
    return ComputeScreenCornerLocation(FCogImguiHelper::ToImVec2(InAlignment), FCogImguiHelper::ToImVec2(InPadding));
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec2 FCogWidgets::ComputeScreenCornerLocation(const ImVec2& InAlignment, const ImVec2& InPadding)
{
    const ImGuiViewport* Viewport = ImGui::GetMainViewport();
    if (Viewport == nullptr)
    { return ImVec2(0, 0); }

    // +Padding for left, 0 for center, -Padding for left 
    // +Padding for top, 0 for center, -Padding for bottom
    const ImVec2 Offset = (InAlignment * 2 - ImVec2(1.0f, 1.0f)) * InPadding;

    ImVec2 Position = Viewport->WorkPos + (InAlignment * Viewport->WorkSize) - Offset;
    return Position;
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogWidgets::GetStringAfterCharacter(const FString& InString, const TCHAR InChar)
{
    int32 Index = 0;
    if (InString.FindChar(InChar, Index)) 
    {
        return InString.RightChop(Index + 1);
    }

    return FString();
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogWidgets::FormatConfigName(const FString& InConfigName)
{
    return FName::NameToDisplayString(GetStringAfterCharacter(InConfigName, '_'), false);
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogWidgets::FormatShortcutName(const FString& InShortcutName)
{
    return FName::NameToDisplayString(InShortcutName.Replace(TEXT("Shortcut_"), TEXT("")), false);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::TextInputChordProperty(UObject& InConfig, const FProperty& InInputChordProperty)
{
    const FInputChord* InputChord = InInputChordProperty.ContainerPtrToValuePtr<FInputChord>(&InConfig);
    if (InputChord == nullptr)
    { return; }

    const auto Name = StringCast<ANSICHAR>(*FormatShortcutName(InInputChordProperty.GetName()));
    const auto Shortcut = StringCast<ANSICHAR>(*FCogImguiInputHelper::InputChordToString(*InputChord));

    ImGui::Text("%s", Name.Get());
    ImGui::SameLine();
    if (BeginRightAlign(Name.Get()))
    {
        ImGui::TextDisabled("%s", Shortcut.Get());
        EndRightAlign();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::InputChordProperty(UObject& InConfig, const FProperty& InInputChordProperty)
{
    FInputChord* InputChord = InInputChordProperty.ContainerPtrToValuePtr<FInputChord>(&InConfig);
    if (InputChord == nullptr)
    { return false; }
    
    const auto Name = StringCast<ANSICHAR>(*FormatShortcutName(InInputChordProperty.GetName()));

    return FCogWidgets::InputChord(Name.Get(), *InputChord); 
}


//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::IsConfigContainingInputChords(const UObject& InConfig)
{
    for (TFieldIterator<FProperty> It(InConfig.GetClass()); It; ++It)
    {
        if (const FStructProperty* StructProperty = CastField<FStructProperty>(*It))
        {
            if (StructProperty->Struct == FInputChord::StaticStruct())
            {
                return true;
            }
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWidgets::AllInputChordsOfConfig(UObject& InConfig, FProperty** InModifiedProperty)
{
    bool HasChanged = false;
    TArray<FProperty*> Properties;
    for (TFieldIterator<FProperty> It(InConfig.GetClass()); It; ++It)
    {
        if (FStructProperty* StructProperty = CastField<FStructProperty>(*It))
        {
            if (StructProperty->Struct == FInputChord::StaticStruct())
            {
                if (FCogWidgets::InputChordProperty(InConfig, *StructProperty))
                {
                    HasChanged = true;
                    if (InModifiedProperty != nullptr)
                    {
                        *InModifiedProperty = StructProperty;
                    }
                }
            }
        }
    }
    
    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWidgets::TextOfAllInputChordsOfConfig(UObject& InConfig)
{
    TArray<FProperty*> Properties;
    for (TFieldIterator<FProperty> It(InConfig.GetClass()); It; ++It)
    {
        if (const FStructProperty* StructProperty = CastField<FStructProperty>(*It))
        {
            if (StructProperty->Struct == FInputChord::StaticStruct())
            {
                TextInputChordProperty(InConfig, *StructProperty);
            }
        }
    }
}



