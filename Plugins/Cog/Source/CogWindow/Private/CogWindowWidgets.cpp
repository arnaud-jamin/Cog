#include "CogWindowWidgets.h"

#include "CogDebug.h"
#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "CogImguiKeyInfo.h"
#include "CogWindowHelper.h"
#include "Components/PrimitiveComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerInput.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "InputCoreTypes.h"

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
void FCogWindowWidgets::ThinSeparatorText(const char* Label)
{
	ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextBorderSize, 2);
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));

	ImGui::SeparatorText(Label);

    ImGui::PopStyleColor();
	ImGui::PopStyleVar();
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
bool FCogWindowWidgets::ToggleMenuButton(bool* Value, const char* Text, const ImVec4& TrueColor)
{
    bool IsPressed = false;
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

    IsPressed = ImGui::Button(Text);
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
bool FCogWindowWidgets::ToggleButton(bool* Value, const char* Text, const ImVec4& TrueColor, const ImVec4& FalseColor, const ImVec2& Size)
{
    return ToggleButton(Value, Text, Text, TrueColor, FalseColor, Size);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::ToggleButton(bool* Value, const char* TextTrue, const char* TextFalse, const ImVec4& TrueColor, const ImVec4& FalseColor, const ImVec2& Size)
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
void FCogWindowWidgets::SearchBar(ImGuiTextFilter& Filter, float Width /*= -1*/)
{
	const ImGuiWindow* Window = FCogImguiHelper::GetCurrentWindow();
	const ImVec2 Pos1 = Window->DC.CursorPos;
    Filter.Draw("##Filter", Width);
	const ImVec2 Pos2 = Window->DC.CursorPosPrevLine;

    if (ImGui::IsItemActive() == false && Filter.Filters.empty())
    {
        static const char* Text = "Search";
        const float Height = ImGui::GetFrameHeight();
        const ImGuiContext& g = *ImGui::GetCurrentContext();
        const ImVec2 Min = Pos1 + ImVec2(g.Style.ItemSpacing.x, 0.0f);
        const ImVec2 Max = Pos2 + ImVec2(-g.Style.ItemSpacing.x, Height);
        const ImRect BB(Min, Max);
        const ImVec2 TextSize = ImGui::CalcTextSize(Text, nullptr);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 128));
        ImGui::RenderTextClipped(Min, Max, Text, nullptr, &TextSize, ImVec2(0.0f, 0.5f), &BB);
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
float FCogWindowWidgets::GetShortWidth()
{
    return ImGui::GetFontSize() * 10;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogWindowWidgets::SetNextItemToShortWidth()
{
    ImGui::SetNextItemWidth(GetShortWidth());
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

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("%s", TooltipText);
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

    const auto Str = StringCast<ANSICHAR>(*KeyBind.Command);
    ImStrncpy(Buffer, Str.Get(), IM_ARRAYSIZE(Buffer));

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
        ImGui::SetTooltip("%s", Tooltip);
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

        const auto Text = StringCast<ANSICHAR>(*FString::Printf(TEXT("%d"), ButtonValue));
        if (MultiChoiceButton(Text.Get(), ButtonValue == Value, Size))
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

        const auto Text = StringCast<ANSICHAR>(*FString::Printf(TEXT("%g"), ButtonValue).Replace(TEXT("0."), TEXT(".")));
        if (MultiChoiceButton(Text.Get(), ButtonValue == Value, Size))
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
bool FCogWindowWidgets::ComboCollisionChannel(const char* Label, ECollisionChannel& Channel)
{
    FColor ChannelColors[ECC_MAX];
    FCogDebug::GetDebugChannelColors(ChannelColors);

    FName SelectedChannelName;
    const UCollisionProfile* CollisionProfile = UCollisionProfile::Get();
    if (CollisionProfile != nullptr)
    {
        SelectedChannelName = CollisionProfile->ReturnChannelNameFromContainerIndex(Channel);
    }

    bool Result = false;
    if (ImGui::BeginCombo(Label, TCHAR_TO_ANSI(*SelectedChannelName.ToString()), ImGuiComboFlags_HeightLarge))
    {
        for (int32 ChannelIndex = 0; ChannelIndex < (int32)ECC_OverlapAll_Deprecated; ++ChannelIndex)
        {
            FColor Color = ChannelColors[ChannelIndex];
            if (Color == FColor::Transparent)
            {
                continue;
            }

            ImGui::PushID(ChannelIndex);

            const FName ChannelName = CollisionProfile->ReturnChannelNameFromContainerIndex(ChannelIndex);

            FCogImguiHelper::ColorEdit4("Color", Color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();

            if (ChannelName.IsValid())
            {
                if (ImGui::Selectable(TCHAR_TO_ANSI(*ChannelName.ToString())))
                {
                    Channel = (ECollisionChannel)ChannelIndex;
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
bool FCogWindowWidgets::CollisionProfileChannel(const UCollisionProfile& CollisionProfile, const int32 ChannelIndex, FColor& ChannelColor, int32& Channels)
{
    bool Result = false;

    FCogImguiHelper::ColorEdit4("Color", ChannelColor, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::SameLine();

    bool IsCollisionActive = (Channels & ECC_TO_BITFIELD(ChannelIndex)) > 0;
    const FName ChannelName = CollisionProfile.ReturnChannelNameFromContainerIndex(ChannelIndex);
    if (ImGui::Checkbox(TCHAR_TO_ANSI(*ChannelName.ToString()), &IsCollisionActive))
    {
        Result = true;

        if (IsCollisionActive)
        {
            Channels |= ECC_TO_BITFIELD(ChannelIndex);
        }
        else
        {
            Channels &= ~ECC_TO_BITFIELD(ChannelIndex);
        }
    }

	return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::CollisionProfileChannels(int32& Channels)
{
    const UCollisionProfile* CollisionProfile = UCollisionProfile::Get();
    if (CollisionProfile == nullptr)
    {
        return false;
    }

    FColor ChannelColors[ECC_MAX];
    FCogDebug::GetDebugChannelColors(ChannelColors);

    bool Result = false;

    for (int32 ChannelIndex = 0; ChannelIndex < (int32)ECC_OverlapAll_Deprecated; ++ChannelIndex)
    {
        FColor Color = ChannelColors[ChannelIndex];
        if (Color == FColor::Transparent)
        {
            continue;
        }

        ImGui::PushID(ChannelIndex);
        Result |= CollisionProfileChannel(*CollisionProfile, ChannelIndex, Color, Channels);
        ImGui::PopID();
    }

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::ActorsListWithFilters(const UWorld& World, const TArray<TSubclassOf<AActor>>& ActorClasses, int32& SelectedActorClassIndex, ImGuiTextFilter* Filter, const APawn* LocalPlayerPawn, const FCogWindowActorContextMenuFunction& ContextMenuFunction)
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
        SearchBar(*Filter);
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
    const bool SelectionChanged = ActorsList(World, SelectedClass, Filter, LocalPlayerPawn, ContextMenuFunction);
    ImGui::EndChild();

    return SelectionChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::ActorsList(const UWorld& World, const TSubclassOf<AActor> ActorClass, const ImGuiTextFilter* Filter, const APawn* LocalPlayerPawn, const FCogWindowActorContextMenuFunction& ContextMenuFunction)
{
    TArray<AActor*> Actors;
    for (TActorIterator It(&World, ActorClass); It; ++It)
    {
        AActor* Actor = *It;

        bool AddActor = true;
        if (Filter != nullptr && Filter->IsActive())
        {
            const auto ActorName = StringCast<ANSICHAR>(*FCogWindowHelper::GetActorName(*Actor));
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

    const AActor* OldSelection = FCogDebug::GetSelection();
    const AActor* NewSelection = OldSelection;

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
            if (ImGui::Selectable(TCHAR_TO_ANSI(*FCogWindowHelper::GetActorName(*Actor)), bIsSelected))
            {
                FCogDebug::SetSelection(&World, Actor);
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
bool FCogWindowWidgets::MenuActorsCombo(const char* StrID, const UWorld& World, TSubclassOf<AActor> ActorClass, const FCogWindowActorContextMenuFunction& ContextMenuFunction)
{
    int32 SelectedActorClassIndex = 0;
    const TArray ActorClasses = { ActorClass };

    AActor* Actor = nullptr;
    return MenuActorsCombo(StrID, World, ActorClasses, SelectedActorClassIndex, nullptr, nullptr, ContextMenuFunction);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogWindowWidgets::MenuActorsCombo(const char* StrID, const UWorld& World, const TArray<TSubclassOf<AActor>>& ActorClasses, int32& SelectedActorClassIndex, ImGuiTextFilter* Filter, const APawn* LocalPlayerPawn, const FCogWindowActorContextMenuFunction& ContextMenuFunction)
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

        const FString CurrentSelectionName = FCogWindowHelper::GetActorName(Selection);
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

        Result = ActorsListWithFilters(World, ActorClasses, SelectedActorClassIndex, Filter, LocalPlayerPawn, ContextMenuFunction);
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
void FCogWindowWidgets::ActorContextMenu(AActor& Selection, const FCogWindowActorContextMenuFunction& ContextMenuFunction)
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
void FCogWindowWidgets::ActorFrame(const AActor& Actor)
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
    if (FCogWindowHelper::ComputeBoundingBoxScreenPosition(PlayerController, BoxOrigin, BoxExtent, ScreenPosMin, ScreenPosMax))
    {
        const ImU32 Color = (&Actor == FCogDebug::GetSelection()) ? IM_COL32(255, 255, 255, 255) : IM_COL32(255, 255, 255, 128);
        if (ScreenPosMin != ScreenPosMax)
        {
            DrawList->AddRect(FCogImguiHelper::ToImVec2(ScreenPosMin) + Viewport->Pos, FCogImguiHelper::ToImVec2(ScreenPosMax) + Viewport->Pos, Color, 0.0f, 0, 1.0f);
        }
        AddTextWithShadow(DrawList, FCogImguiHelper::ToImVec2(ScreenPosMin + FVector2D(0, -14.0f)) + Viewport->Pos, Color, TCHAR_TO_ANSI(*FCogWindowHelper::GetActorName(Actor)));
    }
}
