#include "CogEngineWindow_DebugSettings.h"

#include "CogDebugSettings.h"
#include "CogWindowWidgets.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_DebugSettings::RenderHelp()
{
    ImGui::Text(
        "This window can be used to tweak how the debug display is drawn. "
        "Check each item for more info. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_DebugSettings::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    Config = GetConfig<UCogEngineConfig_DebugSettings>();

    FCogDebugSettings::Persistent = Config->Persistent;
    FCogDebugSettings::TextShadow = Config->TextShadow;
    FCogDebugSettings::Fade2D = Config->Fade2D;
    FCogDebugSettings::Duration = Config->Duration;
    FCogDebugSettings::DepthPriority = Config->DepthPriority;
    FCogDebugSettings::Segments = Config->Segments;
    FCogDebugSettings::Thickness = Config->Thickness;
    FCogDebugSettings::ServerThickness = Config->ServerThickness;
    FCogDebugSettings::ServerColorMultiplier = Config->ServerColorMultiplier;
    FCogDebugSettings::ArrowSize = Config->ArrowSize;
    FCogDebugSettings::AxesScale = Config->AxesScale;
    FCogDebugSettings::GradientColorIntensity = Config->GradientColorIntensity;
    FCogDebugSettings::GradientColorSpeed = Config->GradientColorSpeed;
    FCogDebugSettings::TextSize = Config->TextSize;

    FCogDebugSettings::SetIsFilteringBySelection(GetWorld(), Config->bIsFilteringBySelection);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_DebugSettings::ResetConfig()
{
    Super::ResetConfig();

    Config->Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_DebugSettings::PreSaveConfig()
{
    Super::PreSaveConfig();

    Config->Persistent               = FCogDebugSettings::Persistent;
    Config->TextShadow               = FCogDebugSettings::TextShadow;
    Config->Fade2D                   = FCogDebugSettings::Fade2D;
    Config->Duration                 = FCogDebugSettings::Duration;
    Config->DepthPriority            = FCogDebugSettings::DepthPriority;
    Config->Segments                 = FCogDebugSettings::Segments;
    Config->Thickness                = FCogDebugSettings::Thickness;
    Config->ServerThickness          = FCogDebugSettings::ServerThickness;
    Config->ServerColorMultiplier    = FCogDebugSettings::ServerColorMultiplier;
    Config->ArrowSize                = FCogDebugSettings::ArrowSize;
    Config->AxesScale                = FCogDebugSettings::AxesScale;
    Config->GradientColorIntensity   = FCogDebugSettings::GradientColorIntensity;
    Config->GradientColorSpeed       = FCogDebugSettings::GradientColorSpeed;
    Config->TextSize                 = FCogDebugSettings::TextSize;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_DebugSettings::RenderContent()
{
    Super::RenderContent();

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::MenuItem("Reset"))
        {
            FCogDebugSettings::Reset();
        }

        ImGui::EndMenuBar();
    }

    Config->bIsFilteringBySelection = FCogDebugSettings::GetIsFilteringBySelection();
    if (ImGui::Checkbox("Filter by selection", &Config->bIsFilteringBySelection))
    {
        FCogDebugSettings::SetIsFilteringBySelection(GetWorld(), Config->bIsFilteringBySelection);
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("If checked, only show the debug of the currently selected actor. Otherwise show the debug of all actors.");
    }

    ImGui::Checkbox("Persistent", &FCogDebugSettings::Persistent);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("Make debug draw always persist");
    }

    ImGui::Checkbox("Text Shadow", &FCogDebugSettings::TextShadow);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("Show a shadow below debug text.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::Checkbox("Fade 2D", &FCogDebugSettings::Fade2D);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("Does the 2D debug is fading out.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Duration", &FCogDebugSettings::Duration, 0.01f, 0.0f, 100.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The duration of debug elements.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Thickness", &FCogDebugSettings::Thickness, 0.05f, 0.0f, 5.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The thickness of debug lines.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Server Thickness", &FCogDebugSettings::ServerThickness, 0.05f, 0.0f, 5.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The thickness the server debug lines.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Server Color Mult", &FCogDebugSettings::ServerColorMultiplier, 0.01f, 0.0f, 1.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The color multiplier applied to the server debug lines.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragInt("Depth Priority", &FCogDebugSettings::DepthPriority, 0.1f, 0, 100);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The depth priority of debug elements.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragInt("Segments", &FCogDebugSettings::Segments, 0.1f, 4, 20.0f);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The number of segments used for circular shapes.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Axes Scale", &FCogDebugSettings::AxesScale, 0.1f, 0, 10.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The scaling debug axis.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Arrow Size", &FCogDebugSettings::ArrowSize, 1.0f, 0.0f, 200.0f, "%.0f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The size of debug arrows.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Gradient Intensity", &FCogDebugSettings::GradientColorIntensity, 0.01f, 0.0f, 1.0f, "%.2f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("How much the debug elements color should be changed by a gradient color over time.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Gradient Speed", &FCogDebugSettings::GradientColorSpeed, 0.1f, 0.0f, 10.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The speed of the gradient color change.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Text Size", &FCogDebugSettings::TextSize, 0.1f, 0.1f, 5.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The size of the debug texts.");
    }
}
