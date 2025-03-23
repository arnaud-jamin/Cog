#include "CogEngineWindow_DebugSettings.h"

#include "CogDebug.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"
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
void FCogEngineWindow_DebugSettings::PreSaveConfig()
{
    Super::PreSaveConfig();

    if (Config == nullptr)
    { return; }
    
    Config->Data = FCogDebug::Settings;
}

//--------------------------------------------------------------------------------------------------------------------------
void RenderCollisionChannelColor(const UCollisionProfile& CollisionProfile, FColor& Color, ECollisionChannel Channel, ImGuiColorEditFlags ColorEditFlags)
{
	if (CollisionProfile.ConvertToObjectType(Channel) == TraceTypeQuery_MAX && CollisionProfile.ConvertToTraceType(Channel) == TraceTypeQuery_MAX)
	{ return; }

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
		ImGui::SetItemTooltip("If checked, only show the debug of the currently selected actor. Otherwise show the debug of all actors.");

		ImGui::Checkbox("Persistent", &Settings.Persistent);
		ImGui::SetItemTooltip("Make debug draw always persist");

		ImGui::Checkbox("Actor Name Use Label", &Settings.ActorNameUseLabel);
		ImGui::SetItemTooltip("Use the actor label when displaying the actor name.");

		ImGui::Checkbox("Text Shadow", &Settings.TextShadow);
		ImGui::SetItemTooltip("Show a shadow below debug text.");

		FCogWidgets::SetNextItemToShortWidth();
		ImGui::Checkbox("Fade 2D", &Settings.Fade2D);
		ImGui::SetItemTooltip("Does the 2D debug is fading out.");

		FCogWidgets::SetNextItemToShortWidth();
		ImGui::DragFloat("Duration", &Settings.Duration, 0.01f, 0.0f, 100.0f, "%.1f");
		ImGui::SetItemTooltip("The duration of debug elements.");

		FCogWidgets::SetNextItemToShortWidth();
		ImGui::DragFloat("Thickness", &Settings.Thickness, 0.05f, 0.0f, 5.0f, "%.1f");
		ImGui::SetItemTooltip("The thickness of debug lines.");

		FCogWidgets::SetNextItemToShortWidth();
		ImGui::DragFloat("Server Thickness", &Settings.ServerThickness, 0.05f, 0.0f, 5.0f, "%.1f");
		ImGui::SetItemTooltip("The thickness the server debug lines.");

		FCogWidgets::SetNextItemToShortWidth();
		ImGui::DragFloat("Server Color Mult", &Settings.ServerColorMultiplier, 0.01f, 0.0f, 1.0f, "%.1f");
		ImGui::SetItemTooltip("The color multiplier applied to the server debug lines.");

		FCogWidgets::SetNextItemToShortWidth();
		ImGui::DragInt("Depth Priority", &Settings.DepthPriority, 0.1f, 0, 100);
		ImGui::SetItemTooltip("The depth priority of debug elements.");

		FCogWidgets::SetNextItemToShortWidth();
		ImGui::DragInt("Segments", &Settings.Segments, 0.1f, 4, 20.0f);
		ImGui::SetItemTooltip("The number of segments used for circular shapes.");

		FCogWidgets::SetNextItemToShortWidth();
		ImGui::DragFloat("Axes Scale", &Settings.AxesScale, 0.1f, 0, 10.0f, "%.1f");
		ImGui::SetItemTooltip("The scaling debug axis.");

		FCogWidgets::SetNextItemToShortWidth();
		ImGui::DragFloat("Arrow Size", &Settings.ArrowSize, 1.0f, 0.0f, 200.0f, "%.0f");
		ImGui::SetItemTooltip("The size of debug arrows.");

		FCogWidgets::SetNextItemToShortWidth();
		ImGui::DragFloat("Text Size", &Settings.TextSize, 0.1f, 0.1f, 5.0f, "%.1f");
		ImGui::SetItemTooltip("The size of the debug texts.");
    }

	if (ImGui::CollapsingHeader("Recolor", ImGuiTreeNodeFlags_DefaultOpen))
    {
        FCogWidgets::SetNextItemToShortWidth();

        ECogDebugRecolorMode Mode = Settings.RecolorMode;
        if (FCogWidgets::ComboboxEnum("Recolor mode", Mode))
        {
            Settings.RecolorMode = Mode;
        }
		ImGui::SetItemTooltip("How the debug element should be recolored.");

        if (Settings.RecolorMode != ECogDebugRecolorMode::None)
        {
            FCogWidgets::SetNextItemToShortWidth();
            ImGui::DragFloat("Recolor Intensity", &Settings.RecolorIntensity, 0.01f, 0.0f, 1.0f, "%.2f");
            ImGui::SetItemTooltip("How much the debug elements color should be changed.");
        }

        if (Settings.RecolorMode == ECogDebugRecolorMode::Color)
        {
            FCogImguiHelper::ColorEdit4("Recolor Color", Settings.RecolorColor, ColorEditFlags);
        }
        else if (Settings.RecolorMode == ECogDebugRecolorMode::HueOverTime)
        {
            FCogWidgets::SetNextItemToShortWidth();
            ImGui::DragFloat("Recolor Speed", &Settings.RecolorTimeSpeed, 0.1f, 0.0f, 10.0f, "%.1f");
            ImGui::SetItemTooltip("The speed of the recolor.");
        }
        else if (Settings.RecolorMode == ECogDebugRecolorMode::HueOverFrames)
        {
            FCogWidgets::SetNextItemToShortWidth();
            ImGui::DragInt("Recolor Cycle", &Settings.RecolorFrameCycle, 1, 2, 100);
            ImGui::SetItemTooltip("How many frames are used to perform a full hue cycle.");
        }
    }

    if (ImGui::CollapsingHeader("Gizmo"))
    {
        ImGui::SeparatorText("General");

        ImGui::Checkbox("Use Local Space", &Settings.GizmoUseLocalSpace);

    	ImGui::Checkbox("Support Context Menu", &Settings.GizmoSupportContextMenu);
    	ImGui::SetItemTooltip("Does right clicking on the gizmo displays a context menu ?");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Scale", &Settings.GizmoScale, 0.1f, 0.1f, 10.0f, "%.1f");
        ImGui::SetItemTooltip("The scale of the gizmo.");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragInt("Z Low", &Settings.GizmoZLow, 0.5f, 0, 1000);

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragInt("Z High", &Settings.GizmoZHigh, 0.5f, 0, 1000);

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Thickness Z Low", &Settings.GizmoThicknessZLow, 0.1f, 0.0f, 10.0f, "%.1f");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Thickness Z High", &Settings.GizmoThicknessZHigh, 0.1f, 0.0f, 10.0f, "%.1f");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Mouse Max Distance", &Settings.GizmoCursorSelectionThreshold, 0.1f, 0.0f, 50.0f, "%.1f");

        ImGui::SeparatorText("Translation");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::Checkbox("Translation Snap Enable", &Settings.GizmoTranslationSnapEnable);

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Translation Snap", &Settings.GizmoTranslationSnapValue, 0.1f, 0.0f, 1000.0f, "%.1f");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Translation Axis Length", &Settings.GizmoTranslationAxisLength, 0.1f, 0.1f, 500.0f, "%.1f");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Translation Plane Offset", &Settings.GizmoTranslationPlaneOffset, 0.1f, 0.0f, 500.0f, "%.1f");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Translation Plane Extent", &Settings.GizmoTranslationPlaneExtent, 0.1f, 0.0f, 100.0f, "%.1f");

        ImGui::SeparatorText("Rotation");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::Checkbox("Rotation Snap Enable", &Settings.GizmoRotationSnapEnable);

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Rotation Snap", &Settings.GizmoRotationSnapValue, 0.1f, 0.0f, 360.0f, "%.1f");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Rotation Speed", &Settings.GizmoRotationSpeed, 0.01f, 0.01f, 100.0f, "%.2f");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Rotation Radius", &Settings.GizmoRotationRadius, 0.1f, 0.1f, 500.0f, "%.1f");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragInt("Rotation Segments", &Settings.GizmoRotationSegments, 0.5f, 2, 12);

        ImGui::SeparatorText("Scale");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::Checkbox("Scale Snap Enable", &Settings.GizmoScaleSnapEnable);

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Scale Snap", &Settings.GizmoScaleSnapValue, 0.1f, 0.0f, 10.0f, "%.1f");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Scale Box Offset", &Settings.GizmoScaleBoxOffset, 0.0f, 0.0f, 500.0f, "%.1f");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Scale Box Extent", &Settings.GizmoScaleBoxExtent, 0.1f, 0.0f, 100.0f, "%.1f");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Scale Speed", &Settings.GizmoScaleSpeed, 0.01f, 0.01f, 100.0f, "%.2f");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Scale Min", &Settings.GizmoScaleMin, 0.001f, 0.001f, 1.0f, "%.3f");

        ImGui::SeparatorText("Ground Raycast");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Ground Raycast Length", &Settings.GizmoGroundRaycastLength, 10.0f, 0.0f, 1000000.0f, "%.0f");

        FCogWidgets::SetNextItemToShortWidth();
        ECollisionChannel Channel = Settings.GizmoGroundRaycastChannel.GetValue();
        if (FCogWidgets::ComboTraceChannel("Channel", Channel))
        {
            Settings.GizmoGroundRaycastChannel = Channel;
        }

        FCogWidgets::SetNextItemToShortWidth();
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
        ImGui::SetItemTooltip("Draw the shape of the primitives that have been hit.");

    	ImGui::Checkbox("Draw Hit Primitive Actors Name", &Settings.CollisionQueryDrawHitPrimitiveActorsName);
        ImGui::SetItemTooltip("Draw the actor name of the primitives that have been hit.");

    	ImGui::Checkbox("Prim Hit Primitive Actors Name Shadow", &Settings.CollisionQueryHitPrimitiveActorsNameShadow);
        ImGui::SetItemTooltip("Draw the actor name shadow of the primitives that have been hit.");

    	ImGui::Checkbox("Draw Hit Shapes", &Settings.CollisionQueryDrawHitShapes);
        ImGui::SetItemTooltip("Draw the sweep shape at every impact location.");

        ImGui::Checkbox("Draw Hit Location", &Settings.CollisionQueryDrawHitLocation);
        ImGui::SetItemTooltip("Draw the location of hit results.");

        ImGui::Checkbox("Draw Hit Impact Points", &Settings.CollisionQueryDrawHitImpactPoints);
        ImGui::SetItemTooltip("Draw the impact point of hit results.");

        ImGui::Checkbox("Draw Hit Normals", &Settings.CollisionQueryDrawHitNormals);
        ImGui::SetItemTooltip("Draw the hit normal of hit results.");

        ImGui::Checkbox("Draw Hit Impact Normals", &Settings.CollisionQueryDrawHitImpactNormals);
        ImGui::SetItemTooltip("Draw the hit impact normal of hit results.");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Primitive Actors Name Size", &Settings.CollisionQueryHitPrimitiveActorsNameSize, 0.1f, 0.5f, 10.0f, "%0.1f");
        ImGui::SetItemTooltip("Size of the actor name of the primitives that have been hit.");

        FCogWidgets::SetNextItemToShortWidth();
        ImGui::DragFloat("Hit Point Size", &Settings.CollisionQueryHitPointSize, 0.5f, 0.0f, 100.0f, "%0.1f");
        ImGui::SetItemTooltip("Size of the hit result location and impact point.");

        FCogImguiHelper::ColorEdit4("Hit Color", Settings.CollisionQueryHitColor, ColorEditFlags);
        ImGui::SetItemTooltip("Color of the collision query when the query has hit.");

        FCogImguiHelper::ColorEdit4("No Hit Color", Settings.CollisionQueryNoHitColor, ColorEditFlags);
        ImGui::SetItemTooltip("Color of the collision query when the query has not hit.");

        FCogImguiHelper::ColorEdit4("Normal Color", Settings.CollisionQueryNormalColor, ColorEditFlags);
        ImGui::SetItemTooltip("Color of the hit result normal color.");

        FCogImguiHelper::ColorEdit4("Impact Normal Color", Settings.CollisionQueryImpactNormalColor, ColorEditFlags);
        ImGui::SetItemTooltip("Color of the hit result impact normal color.");
    }
}
