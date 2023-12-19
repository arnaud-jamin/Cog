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

    FCogDebugSettings::Data = Config->Data;
    FCogDebugSettings::SetIsFilteringBySelection(GetWorld(), Config->Data.bIsFilteringBySelection);
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

    Config->Data = FCogDebugSettings::Data;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_DebugSettings::RenderContent()
{
    Super::RenderContent();

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::MenuItem("Reset"))
            {
                FCogDebugSettings::Reset();
            }

        	ImGui::Checkbox("Show Advanced Settings", &Config->bShowAdvancedSettings);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    Config->Data.bIsFilteringBySelection = FCogDebugSettings::GetIsFilteringBySelection();
    if (ImGui::Checkbox("Filter by selection", &Config->Data.bIsFilteringBySelection))
    {
        FCogDebugSettings::SetIsFilteringBySelection(GetWorld(), Config->Data.bIsFilteringBySelection);
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("If checked, only show the debug of the currently selected actor. Otherwise show the debug of all actors.");
    }

    FCogDebugData& Data = FCogDebugSettings::Data;

    ImGui::Checkbox("Persistent", &Data.Persistent);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("Make debug draw always persist");
    }

    ImGui::Checkbox("Text Shadow", &Data.TextShadow);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("Show a shadow below debug text.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::Checkbox("Fade 2D", &Data.Fade2D);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("Does the 2D debug is fading out.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Duration", &Data.Duration, 0.01f, 0.0f, 100.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The duration of debug elements.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Thickness", &Data.Thickness, 0.05f, 0.0f, 5.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The thickness of debug lines.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Server Thickness", &Data.ServerThickness, 0.05f, 0.0f, 5.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The thickness the server debug lines.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Server Color Mult", &Data.ServerColorMultiplier, 0.01f, 0.0f, 1.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The color multiplier applied to the server debug lines.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragInt("Depth Priority", &Data.DepthPriority, 0.1f, 0, 100);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The depth priority of debug elements.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragInt("Segments", &Data.Segments, 0.1f, 4, 20.0f);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The number of segments used for circular shapes.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Axes Scale", &Data.AxesScale, 0.1f, 0, 10.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The scaling debug axis.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Arrow Size", &Data.ArrowSize, 1.0f, 0.0f, 200.0f, "%.0f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The size of debug arrows.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Gradient Intensity", &Data.GradientColorIntensity, 0.01f, 0.0f, 1.0f, "%.2f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("How much the debug elements color should be changed by a gradient color over time.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Gradient Speed", &Data.GradientColorSpeed, 0.1f, 0.0f, 10.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The speed of the gradient color change.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Text Size", &Data.TextSize, 0.1f, 0.1f, 5.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The size of the debug texts.");
    }

    if (Config->bShowAdvancedSettings)
    {
        ImGui::SeparatorText("Gizmo");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Gizmo Scale", &Data.GizmoScale, 0.1f, 0.1f, 10.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The scale of the gizmo.");
    }

    if (Config->bShowAdvancedSettings)
    {
        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Axis Length", &Data.GizmoAxisLength, 0.1f, 0.1f, 500.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragInt("Gizmo Z Low", &Data.GizmoZLow, 0.5f, 0, 1000);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragInt("Gizmo Z High", &Data.GizmoZHigh, 0.5f, 0, 1000);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Thickness Z Low", &Data.GizmoThicknessZLow, 0.1f, 0.0f, 10.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Thickness Z High", &Data.GizmoThicknessZHigh, 0.1f, 0.0f, 10.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Mouse Max Distance", &Data.GizmoMouseMaxDistance, 0.1f, 0.0f, 50.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Plane Offset", &Data.GizmoPlaneOffset, 0.1f, 0.0f, 500.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Plane Extent", &Data.GizmoPlaneExtent, 0.1f, 0.0f, 100.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Scale Box Extent", &Data.GizmoScaleBoxExtent, 0.1f, 0.0f, 100.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Scale Speed", &Data.GizmoScaleSpeed, 0.1f, 0.0f, 100.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Scale Min", &Data.GizmoScaleMin, 0.001f, 0.001f, 1.0f, "%.3f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Rotation Radius", &Data.GizmoRotationRadius, 0.1f, 0.1f, 500.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragInt("Gizmo Rotation Segments", &Data.GizmoRotationSegments, 1.0f, 0, 64);

        FCogWindowWidgets::ColorEdit4("Gizmo Axis Colors ZLow X", Data.GizmoAxisColorsZLowX, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogWindowWidgets::ColorEdit4("Gizmo Axis Colors ZLow Y", Data.GizmoAxisColorsZLowY, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogWindowWidgets::ColorEdit4("Gizmo Axis Colors ZLow Z", Data.GizmoAxisColorsZLowZ, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogWindowWidgets::ColorEdit4("Gizmo Axis Colors ZLow W", Data.GizmoAxisColorsZLowW, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);

    	FCogWindowWidgets::ColorEdit4("Gizmo Axis Colors ZHigh X", Data.GizmoAxisColorsZHighX, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogWindowWidgets::ColorEdit4("Gizmo Axis Colors ZHigh Y", Data.GizmoAxisColorsZHighY, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogWindowWidgets::ColorEdit4("Gizmo Axis Colors ZHigh Z", Data.GizmoAxisColorsZHighZ, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogWindowWidgets::ColorEdit4("Gizmo Axis Colors ZHigh W", Data.GizmoAxisColorsZHighW, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);

        FCogWindowWidgets::ColorEdit4("Gizmo Axis Colors Selection X", Data.GizmoAxisColorsSelectionX, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogWindowWidgets::ColorEdit4("Gizmo Axis Colors Selection Y", Data.GizmoAxisColorsSelectionY, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogWindowWidgets::ColorEdit4("Gizmo Axis Colors Selection Z", Data.GizmoAxisColorsSelectionZ, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogWindowWidgets::ColorEdit4("Gizmo Axis Colors Selection W", Data.GizmoAxisColorsSelectionW, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
    }
}
