#include "CogEngineWindow_DebugSettings.h"

#include "CogDebug.h"
#include "CogImguiHelper.h"
#include "CogWindowWidgets.h"
#include "Engine/CollisionProfile.h"

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
void RenderCollisionChannelColor(const UCollisionProfile& CollisionProfile, FColor& Color, ECollisionChannel Channel, ImGuiColorEditFlags ColorEditFlags)
{
    const FString ChannelName = CollisionProfile.ReturnChannelNameFromContainerIndex(Channel).ToString();
    FCogImguiHelper::ColorEdit4(StringCast<ANSICHAR>(*ChannelName).Get(), Color, ColorEditFlags);
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

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    constexpr ImGuiColorEditFlags ColorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf;
    FCogDebugSettings& Settings = FCogDebug::Settings;

    if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
    {
        Config->Data.bIsFilteringBySelection = FCogDebug::GetIsFilteringBySelection();
        if (ImGui::Checkbox("Filter by selection", &Config->Data.bIsFilteringBySelection))
        {
            FCogDebug::SetIsFilteringBySelection(GetWorld(), Config->Data.bIsFilteringBySelection);
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
        {
            ImGui::SetTooltip("If checked, only show the debug of the currently selected actor. Otherwise show the debug of all actors.");
        }

        ImGui::Checkbox("Persistent", &Settings.Persistent);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
        {
            ImGui::SetTooltip("Make debug draw always persist");
        }

        ImGui::Checkbox("Actor Name Use Label", &Settings.ActorNameUseLabel);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
        {
            ImGui::SetTooltip("Use the actor label when displaying the actor name.");
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
    }

    if (ImGui::CollapsingHeader("Gizmo"))
    {
        ImGui::SeparatorText("General");

        ImGui::Checkbox("Use Local Space", &Settings.GizmoUseLocalSpace);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Gizmo Scale", &Settings.GizmoScale, 0.1f, 0.1f, 10.0f, "%.1f");
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
        {
            ImGui::SetTooltip("The scale of the gizmo.");
        }

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragInt("Z Low", &Settings.GizmoZLow, 0.5f, 0, 1000);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragInt("Z High", &Settings.GizmoZHigh, 0.5f, 0, 1000);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Thickness Z Low", &Settings.GizmoThicknessZLow, 0.1f, 0.0f, 10.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Thickness Z High", &Settings.GizmoThicknessZHigh, 0.1f, 0.0f, 10.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Mouse Max Distance", &Settings.GizmoCursorSelectionThreshold, 0.1f, 0.0f, 50.0f, "%.1f");

        ImGui::SeparatorText("Translation");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::Checkbox("Translation Snap Enable", &Settings.GizmoTranslationSnapEnable);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Translation Snap", &Settings.GizmoTranslationSnapValue, 0.1f, 0.0f, 1000.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Translation Axis Length", &Settings.GizmoTranslationAxisLength, 0.1f, 0.1f, 500.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Translation Plane Offset", &Settings.GizmoTranslationPlaneOffset, 0.1f, 0.0f, 500.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Translation Plane Extent", &Settings.GizmoTranslationPlaneExtent, 0.1f, 0.0f, 100.0f, "%.1f");

        ImGui::SeparatorText("Rotation");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::Checkbox("Rotation Snap Enable", &Settings.GizmoRotationSnapEnable);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Rotation Snap", &Settings.GizmoRotationSnapValue, 0.1f, 0.0f, 360.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Rotation Speed", &Settings.GizmoRotationSpeed, 0.01f, 0.01f, 100.0f, "%.2f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Rotation Radius", &Settings.GizmoRotationRadius, 0.1f, 0.1f, 500.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragInt("Rotation Segments", &Settings.GizmoRotationSegments, 0.5f, 2, 12);

        ImGui::SeparatorText("Scale");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::Checkbox("Scale Snap Enable", &Settings.GizmoScaleSnapEnable);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Scale Snap", &Settings.GizmoScaleSnapValue, 0.1f, 0.0f, 10.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Scale Box Offset", &Settings.GizmoScaleBoxOffset, 0.0f, 0.0f, 500.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Scale Box Extent", &Settings.GizmoScaleBoxExtent, 0.1f, 0.0f, 100.0f, "%.1f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Scale Speed", &Settings.GizmoScaleSpeed, 0.01f, 0.01f, 100.0f, "%.2f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Scale Min", &Settings.GizmoScaleMin, 0.001f, 0.001f, 1.0f, "%.3f");

        ImGui::SeparatorText("Ground Raycast");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Ground Raycast Length", &Settings.GizmoGroundRaycastLength, 10.0f, 0.0f, 1000000.0f, "%.0f");

        FCogWindowWidgets::SetNextItemToShortWidth();
        ECollisionChannel Channel = Settings.GizmoGroundRaycastChannel.GetValue();
        if (FCogWindowWidgets::ComboCollisionChannel("Channel", Channel))
        {
            Settings.GizmoGroundRaycastChannel = Channel;
        }

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Ground Raycast Circle Radius", &Settings.GizmoGroundRaycastCircleRadius, 0.1f, 0.1f, 1000.0f, "%.1f");

        FCogImguiHelper::ColorEdit4("Ground Raycast Color", Settings.GizmoGroundRaycastColor, ColorEditFlags);
        FCogImguiHelper::ColorEdit4("Ground Raycast Circle Color", Settings.GizmoGroundRaycastCircleColor, ColorEditFlags);

        ImGui::SeparatorText("Axis Colors");

        FCogImguiHelper::ColorEdit4("Axis Colors ZLow X", Settings.GizmoAxisColorsZLowX, ColorEditFlags);
        FCogImguiHelper::ColorEdit4("Axis Colors ZLow Y", Settings.GizmoAxisColorsZLowY, ColorEditFlags);
        FCogImguiHelper::ColorEdit4("Axis Colors ZLow Z", Settings.GizmoAxisColorsZLowZ, ColorEditFlags);
        FCogImguiHelper::ColorEdit4("Axis Colors ZLow W", Settings.GizmoAxisColorsZLowW, ColorEditFlags);

        FCogImguiHelper::ColorEdit4("Axis Colors ZHigh X", Settings.GizmoAxisColorsZHighX, ColorEditFlags);
        FCogImguiHelper::ColorEdit4("Axis Colors ZHigh Y", Settings.GizmoAxisColorsZHighY, ColorEditFlags);
        FCogImguiHelper::ColorEdit4("Axis Colors ZHigh Z", Settings.GizmoAxisColorsZHighZ, ColorEditFlags);
        FCogImguiHelper::ColorEdit4("Axis Colors ZHigh W", Settings.GizmoAxisColorsZHighW, ColorEditFlags);

        FCogImguiHelper::ColorEdit4("Axis Colors Selection X", Settings.GizmoAxisColorsSelectionX, ColorEditFlags);
        FCogImguiHelper::ColorEdit4("Axis Colors Selection Y", Settings.GizmoAxisColorsSelectionY, ColorEditFlags);
        FCogImguiHelper::ColorEdit4("Axis Colors Selection Z", Settings.GizmoAxisColorsSelectionZ, ColorEditFlags);
        FCogImguiHelper::ColorEdit4("Axis Colors Selection W", Settings.GizmoAxisColorsSelectionW, ColorEditFlags);

        FCogImguiHelper::ColorEdit4("Text Color", Settings.GizmoTextColor, ColorEditFlags);
    }

    if (ImGui::CollapsingHeader("Collision Channels"))
    {
        if (const UCollisionProfile* CollisionProfile = UCollisionProfile::Get())
        {
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorWorldStatic, ECC_WorldStatic, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorWorldDynamic, ECC_WorldDynamic, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorPawn, ECC_Pawn, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorVisibility, ECC_Visibility, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorCamera, ECC_Camera, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorPhysicsBody, ECC_PhysicsBody, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorVehicle, ECC_Vehicle, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorDestructible, ECC_Destructible, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorEngineTraceChannel1, ECC_EngineTraceChannel1, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorEngineTraceChannel2, ECC_EngineTraceChannel2, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorEngineTraceChannel3, ECC_EngineTraceChannel3, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorEngineTraceChannel4, ECC_EngineTraceChannel4, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorEngineTraceChannel5, ECC_EngineTraceChannel5, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorEngineTraceChannel6, ECC_EngineTraceChannel6, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel1, ECC_GameTraceChannel1, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel2, ECC_GameTraceChannel2, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel3, ECC_GameTraceChannel3, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel4, ECC_GameTraceChannel4, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel5, ECC_GameTraceChannel5, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel6, ECC_GameTraceChannel6, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel7, ECC_GameTraceChannel7, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel8, ECC_GameTraceChannel8, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel9, ECC_GameTraceChannel9, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel10, ECC_GameTraceChannel10, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel11, ECC_GameTraceChannel11, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel12, ECC_GameTraceChannel12, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel13, ECC_GameTraceChannel13, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel14, ECC_GameTraceChannel14, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel15, ECC_GameTraceChannel15, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel16, ECC_GameTraceChannel16, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel17, ECC_GameTraceChannel17, ColorEditFlags);
            RenderCollisionChannelColor(*CollisionProfile, Settings.ChannelColorGameTraceChannel18, ECC_GameTraceChannel18, ColorEditFlags);
        }
    }

    if (ImGui::CollapsingHeader("Collision Query"))
    {
        ImGui::Checkbox("Draw Hit Primitives", &Settings.CollisionQueryDrawHitPrimitives);
        ImGui::Checkbox("Draw Hit Primitive Actors Name", &Settings.CollisionQueryDrawHitPrimitiveActorsName);
        ImGui::Checkbox("Prim Hit Primitive Actors Name Shadow", &Settings.CollisionQueryHitPrimitiveActorsNameShadow);
        ImGui::Checkbox("Draw Hit Shapes", &Settings.CollisionQueryDrawHitShapes);
        ImGui::Checkbox("Draw Hit Location", &Settings.CollisionQueryDrawHitLocation);
        ImGui::Checkbox("Draw Hit Impact Points", &Settings.CollisionQueryDrawHitImpactPoints);
        ImGui::Checkbox("Draw Hit Normals", &Settings.CollisionQueryDrawHitNormals);
        ImGui::Checkbox("Draw Hit Impact Normals", &Settings.CollisionQueryDrawHitImpactNormals);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Primitive Actors Name Size", &Settings.CollisionQueryHitPrimitiveActorsNameSize);

        FCogWindowWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Hit Point Size", &Settings.CollisionQueryHitPointSize);

        FCogImguiHelper::ColorEdit4("Hit Color", Settings.CollisionQueryHitColor, ColorEditFlags);
        FCogImguiHelper::ColorEdit4("No Hit Color", Settings.CollisionQueryNoHitColor, ColorEditFlags);
        FCogImguiHelper::ColorEdit4("Normal Color", Settings.CollisionQueryNormalColor, ColorEditFlags);
        FCogImguiHelper::ColorEdit4("Impact Normal Color", Settings.CollisionQueryImpactNormalColor, ColorEditFlags);
    }
}
