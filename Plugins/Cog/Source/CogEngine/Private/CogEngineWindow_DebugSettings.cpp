#include "CogEngineWindow_DebugSettings.h"

#include "CogDebug.h"
#include "CogImGuiHelper.h"
#include "CogWindowWidgets.h"
#include "imgui_internal.h"

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

    FCogDebug::Settings = Config->Data;
    FCogDebug::SetIsFilteringBySelection(GetWorld(), Config->Data.bIsFilteringBySelection);
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

    Config->Data = FCogDebug::Settings;
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
                FCogDebug::Reset();
            }

        	ImGui::Checkbox("Show Advanced Settings", &Config->bShowAdvancedSettings);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    Config->Data.bIsFilteringBySelection = FCogDebug::GetIsFilteringBySelection();
    if (ImGui::Checkbox("Filter by selection", &Config->Data.bIsFilteringBySelection))
    {
        FCogDebug::SetIsFilteringBySelection(GetWorld(), Config->Data.bIsFilteringBySelection);
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("If checked, only show the debug of the currently selected actor. Otherwise show the debug of all actors.");
    }

    FCogDebugSettings& Settings = FCogDebug::Settings;

    ImGui::Checkbox("Persistent", &Settings.Persistent);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("Make debug draw always persist");
    }

    ImGui::Checkbox("Text Shadow", &Settings.TextShadow);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("Show a shadow below debug text.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::Checkbox("Fade 2D", &Settings.Fade2D);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("Does the 2D debug is fading out.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Duration", &Settings.Duration, 0.01f, 0.0f, 100.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The duration of debug elements.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Thickness", &Settings.Thickness, 0.05f, 0.0f, 5.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The thickness of debug lines.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Server Thickness", &Settings.ServerThickness, 0.05f, 0.0f, 5.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The thickness the server debug lines.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Server Color Mult", &Settings.ServerColorMultiplier, 0.01f, 0.0f, 1.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The color multiplier applied to the server debug lines.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragInt("Depth Priority", &Settings.DepthPriority, 0.1f, 0, 100);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The depth priority of debug elements.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragInt("Segments", &Settings.Segments, 0.1f, 4, 20.0f);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The number of segments used for circular shapes.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Axes Scale", &Settings.AxesScale, 0.1f, 0, 10.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The scaling debug axis.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Arrow Size", &Settings.ArrowSize, 1.0f, 0.0f, 200.0f, "%.0f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The size of debug arrows.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Gradient Intensity", &Settings.GradientColorIntensity, 0.01f, 0.0f, 1.0f, "%.2f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("How much the debug elements color should be changed by a gradient color over time.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Gradient Speed", &Settings.GradientColorSpeed, 0.1f, 0.0f, 10.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The speed of the gradient color change.");
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Text Size", &Settings.TextSize, 0.1f, 0.1f, 5.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The size of the debug texts.");
    }

    ImGui::SeparatorText("Gizmo");

    ImGui::Checkbox("Gizmo Use Local Space", &Settings.GizmoUseLocalSpace);

    FCogWindowWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat("Gizmo Scale", &Settings.GizmoScale, 0.1f, 0.1f, 10.0f, "%.1f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
    {
        ImGui::SetTooltip("The scale of the gizmo.");
    }

    if (Config->bShowAdvancedSettings)
    {
        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragInt("Gizmo Z Low", &Settings.GizmoZLow, 0.5f, 0, 1000);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragInt("Gizmo Z High", &Settings.GizmoZHigh, 0.5f, 0, 1000);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Thickness Z Low", &Settings.GizmoThicknessZLow, 0.1f, 0.0f, 10.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Thickness Z High", &Settings.GizmoThicknessZHigh, 0.1f, 0.0f, 10.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Mouse Max Distance", &Settings.GizmoCursorSelectionThreshold, 0.1f, 0.0f, 50.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::Checkbox("Gizmo Translation Snap Enable", &Settings.GizmoTranslationSnapEnable);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Translation Snap", &Settings.GizmoTranslationSnapValue, 0.1f, 0.0f, 1000.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Translation Axis Length", &Settings.GizmoTranslationAxisLength, 0.1f, 0.1f, 500.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Translation Plane Offset", &Settings.GizmoTranslationPlaneOffset, 0.1f, 0.0f, 500.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Translation Plane Extent", &Settings.GizmoTranslationPlaneExtent, 0.1f, 0.0f, 100.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::Checkbox("Gizmo Rotation Snap Enable", &Settings.GizmoRotationSnapEnable);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Rotation Snap", &Settings.GizmoRotationSnapValue, 0.1f, 0.0f, 360.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Rotation Speed", &Settings.GizmoRotationSpeed, 0.01f, 0.01f, 100.0f, "%.2f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Rotation Radius", &Settings.GizmoRotationRadius, 0.1f, 0.1f, 500.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragInt("Gizmo Rotation Segments", &Settings.GizmoRotationSegments, 0.5f, 2, 12);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::Checkbox("Gizmo Scale Snap Enable", &Settings.GizmoScaleSnapEnable);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Scale Snap", &Settings.GizmoScaleSnapValue, 0.1f, 0.0f, 10.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Scale Box Offset", &Settings.GizmoScaleBoxOffset, 0.0f, 0.0f, 500.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Scale Box Extent", &Settings.GizmoScaleBoxExtent, 0.1f, 0.0f, 100.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Scale Speed", &Settings.GizmoScaleSpeed, 0.01f, 0.01f, 100.0f, "%.2f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Scale Min", &Settings.GizmoScaleMin, 0.001f, 0.001f, 1.0f, "%.3f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Ground Raycast Length", &Settings.GizmoGroundRaycastLength, 10.0f, 0.0f, 1000000.0f, "%.0f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ECollisionChannel Channel = Settings.GizmoGroundRaycastChannel.GetValue();
        if (FCogWindowWidgets::ComboCollisionChannel("Channel", Channel))
        {
            Settings.GizmoGroundRaycastChannel = Channel;
        }

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Ground Raycast Circle Radius", &Settings.GizmoGroundRaycastCircleRadius, 0.1f, 0.1f, 1000.0f, "%.1f");

        FCogImguiHelper::ColorEdit4("Gizmo Axis Colors ZLow X", Settings.GizmoAxisColorsZLowX, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogImguiHelper::ColorEdit4("Gizmo Axis Colors ZLow Y", Settings.GizmoAxisColorsZLowY, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogImguiHelper::ColorEdit4("Gizmo Axis Colors ZLow Z", Settings.GizmoAxisColorsZLowZ, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogImguiHelper::ColorEdit4("Gizmo Axis Colors ZLow W", Settings.GizmoAxisColorsZLowW, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);

    	FCogImguiHelper::ColorEdit4("Gizmo Axis Colors ZHigh X", Settings.GizmoAxisColorsZHighX, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogImguiHelper::ColorEdit4("Gizmo Axis Colors ZHigh Y", Settings.GizmoAxisColorsZHighY, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogImguiHelper::ColorEdit4("Gizmo Axis Colors ZHigh Z", Settings.GizmoAxisColorsZHighZ, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogImguiHelper::ColorEdit4("Gizmo Axis Colors ZHigh W", Settings.GizmoAxisColorsZHighW, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);

        FCogImguiHelper::ColorEdit4("Gizmo Axis Colors Selection X", Settings.GizmoAxisColorsSelectionX, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogImguiHelper::ColorEdit4("Gizmo Axis Colors Selection Y", Settings.GizmoAxisColorsSelectionY, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogImguiHelper::ColorEdit4("Gizmo Axis Colors Selection Z", Settings.GizmoAxisColorsSelectionZ, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogImguiHelper::ColorEdit4("Gizmo Axis Colors Selection W", Settings.GizmoAxisColorsSelectionW, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);

        FCogImguiHelper::ColorEdit4("Gizmo Text Color", Settings.GizmoTextColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);

        FCogImguiHelper::ColorEdit4("Gizmo Ground Raycast Color", Settings.GizmoGroundRaycastColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
        FCogImguiHelper::ColorEdit4("Gizmo Ground Raycast Circle Color", Settings.GizmoGroundRaycastCircleColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
    }
}
