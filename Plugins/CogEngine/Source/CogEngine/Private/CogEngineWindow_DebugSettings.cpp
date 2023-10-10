#include "CogEngineWindow_DebugSettings.h"

#include "CogDebugSettings.h"
#include "CogWindowWidgets.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_DebugSettings::RenderHelp()
{
    ImGui::Text(
        "This window can be used to setup how the debug display is drawn. "
        "Check each item for more info. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_DebugSettings::PostInitProperties()
{
    Super::PostInitProperties();

    FCogDebugSettings::FilterBySelection        = FilterBySelection;
    FCogDebugSettings::Persistent               = Persistent;
    FCogDebugSettings::TextShadow               = TextShadow;
    FCogDebugSettings::Fade2D                   = Fade2D;
    FCogDebugSettings::Duration                 = Duration;
    FCogDebugSettings::DepthPriority            = DepthPriority;
    FCogDebugSettings::Segments                 = Segments;
    FCogDebugSettings::Thickness                = Thickness;
    FCogDebugSettings::ServerThickness          = ServerThickness;
    FCogDebugSettings::ServerColorMultiplier    = ServerColorMultiplier;
    FCogDebugSettings::ArrowSize                = ArrowSize;
    FCogDebugSettings::AxesScale                = AxesScale;
    FCogDebugSettings::GradientColorIntensity   = GradientColorIntensity;
    FCogDebugSettings::GradientColorSpeed       = GradientColorSpeed;
    FCogDebugSettings::TextSize                 = TextSize;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_DebugSettings::PreSaveConfig()
{
    Super::PreSaveConfig();

    FilterBySelection        = FCogDebugSettings::FilterBySelection;
    Persistent               = FCogDebugSettings::Persistent;
    TextShadow               = FCogDebugSettings::TextShadow;
    Fade2D                   = FCogDebugSettings::Fade2D;
    Duration                 = FCogDebugSettings::Duration;
    DepthPriority            = FCogDebugSettings::DepthPriority;
    Segments                 = FCogDebugSettings::Segments;
    Thickness                = FCogDebugSettings::Thickness;
    ServerThickness          = FCogDebugSettings::ServerThickness;
    ServerColorMultiplier    = FCogDebugSettings::ServerColorMultiplier;
    ArrowSize                = FCogDebugSettings::ArrowSize;
    AxesScale                = FCogDebugSettings::AxesScale;
    GradientColorIntensity   = FCogDebugSettings::GradientColorIntensity;
    GradientColorSpeed       = FCogDebugSettings::GradientColorSpeed;
    TextSize                 = FCogDebugSettings::TextSize;
}

//--------------------------------------------------------------------------------------------------------------------------
UCogEngineWindow_DebugSettings::UCogEngineWindow_DebugSettings()
{
    bHasMenu = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_DebugSettings::RenderContent()
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

    ImGui::Checkbox("Filter by selection", &FCogDebugSettings::FilterBySelection);
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("If checked, only show the debug of the currently selected actor. Otherwise show the debug of all actors.");

    ImGui::Checkbox("Persistent", &FCogDebugSettings::Persistent);
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("Make debug draw always persist");

    ImGui::Checkbox("Text Shadow", &FCogDebugSettings::TextShadow);
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("Show a shadow below debug text.");

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::Checkbox("Fade 2D", &FCogDebugSettings::Fade2D);
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("Does the 2D debug is fading out.");

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Duration", &FCogDebugSettings::Duration, 0.01f, 0.0f, 100.0f, "%.1f");
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("The duration of debug elements.");

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Thickness", &FCogDebugSettings::Thickness, 0.05f, 0.0f, 5.0f, "%.1f");
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("The thickness of debug lines.");

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Server Thickness", &FCogDebugSettings::ServerThickness, 0.05f, 0.0f, 5.0f, "%.1f");
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("The thickness the server debug lines.");

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Server Color Mult", &FCogDebugSettings::ServerColorMultiplier, 0.01f, 0.0f, 1.0f, "%.1f");
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("The color multiplier applied to the server debug lines.");

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragInt("Depth Priority", &FCogDebugSettings::DepthPriority, 0.1f, 0, 100);
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("The depth priority of debug elements.");

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragInt("Segments", &FCogDebugSettings::Segments, 0.1f, 4, 20.0f);
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("The number of segments used for circular shapes.");

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Axes Scale", &FCogDebugSettings::AxesScale, 0.1f, 0, 10.0f, "%.1f");
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("The scaling debug axis.");

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Arrow Size", &FCogDebugSettings::ArrowSize, 1.0f, 0.0f, 200.0f, "%.0f");
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("The size of debug arrows.");

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Gradient Intensity", &FCogDebugSettings::GradientColorIntensity, 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("How much the debug elements color should be changed by a gradient color over time.");

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Gradient Speed", &FCogDebugSettings::GradientColorSpeed, 0.1f, 0.0f, 10.0f, "%.1f");
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("The speed of the gradient color change.");

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Text Size", &FCogDebugSettings::TextSize, 0.1f, 0.1f, 5.0f, "%.1f");
    ImGui::SameLine();
    FCogWindowWidgets::HelpMarker("The size of the debug texts.");
}
