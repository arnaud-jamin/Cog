#include "CogWindowWidgets.h"

#include "CogImguiHelper.h"
#include "Kismet/KismetMathLibrary.h"
#include "imgui.h"
#include "imgui_internal.h"

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
void FCogWindowWidgets::MenuSearchBar(ImGuiTextFilter& Filter)
{
    ImGui::SameLine();
    const float Pos1 = ImGui::GetCursorPosX();
    Filter.Draw("", -1);
    const float Pos2 = ImGui::GetCursorPosX();
    if (ImGui::IsItemActive() == false && Filter.Filters.empty())
    {
        ImGui::SetCursorPosX(Pos1 + 3);
        ImGui::TextDisabled("Search");
    }
    ImGui::SetCursorPosX(Pos2);
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

