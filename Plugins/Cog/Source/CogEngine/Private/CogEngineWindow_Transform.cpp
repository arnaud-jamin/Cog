#include "CogEngineWindow_Transform.h"

#include "CogDebug.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "GameFramework/Actor.h"
#include "imgui.h"
#include "imgui_internal.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Transform::Initialize()
{
    Super::Initialize();
    Config = GetConfig<UCogEngineConfig_Transform>();
    bHasMenu = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Transform::RenderHelp()
{
    ImGui::Text(
        "This window can be used to modify the transform of the selected actor."
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Transform::RenderContent()
{
    Super::RenderContent();

    AActor* Selection = GetSelection();
    if (Selection == nullptr)
    {
        ImGui::TextDisabled("No Selection");
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            FCogWidgets::SetNextItemToShortWidth();
            ImGui::DragFloat("Drag Speed Location", &Config->LocationSpeed, 0.1f, 0.1f, 100.0f);

        	FCogWidgets::SetNextItemToShortWidth();
            ImGui::DragFloat("Drag Speed Rotation", &Config->RotationSpeed, 0.1f, 0.1f, 100.0f);

        	FCogWidgets::SetNextItemToShortWidth();
            ImGui::DragFloat("Drag Speed Scale", &Config->ScaleSpeed, 0.1f, 0.1f, 100.0f);

        	ImGui::SeparatorText("Gizmo");
            FCogDebugSettings& Settings = FCogDebug::Settings;
            ImGui::Checkbox("Local Space Gizmo", &Settings.GizmoUseLocalSpace);
            RenderSnap("##Location", "Snap Location", &Settings.GizmoTranslationSnapEnable, &Settings.GizmoTranslationSnapValue);
            RenderSnap("##Rotation", "Snap Rotation", &Settings.GizmoRotationSnapEnable, &Settings.GizmoRotationSnapValue);
            RenderSnap("##Scale", "Snap Scale", &Settings.GizmoScaleSnapEnable, &Settings.GizmoScaleSnapValue);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    FTransform Transform = Selection->GetActorTransform();
    if (RenderTransform(Transform))
    {
        Selection->SetActorTransform(Transform);
    }

    if (const APlayerController* LocalPlayerController = GetLocalPlayerController())
    {
        if (Gizmo.Draw("Transform", *LocalPlayerController, Transform))
        {
            Selection->SetActorTransform(Transform);
        }       
    }

    ImGui::PopStyleVar();
}


//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Transform::RenderComponent(const char* Label, double* Value, float Speed, double Min, double Max, double Reset)
{
    ImGui::PushItemWidth(-1);
    bool Result = FCogImguiHelper::DragDouble(Label, Value, Speed, Min, Max, "%.2f");

    if (ImGui::IsItemActive() && ImGui::TempInputIsActive(ImGui::GetActiveID()))
    {
        Result = false;
    }

    if (ImGui::BeginItemTooltip())
    {
        ImGui::Text("%.2f", *Value);
        ImGui::TextDisabled("Reset [RMB]");
        ImGui::EndTooltip();
    }

    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
        *Value = Reset;
        Result = true;
    }

    ImGui::PopItemWidth();
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Transform::RenderSnap(const char* CheckboxLabel, const char* InputLabel, bool* SnapEnable, float* Snap)
{
    ImGui::Checkbox(CheckboxLabel, SnapEnable);

    ImGui::SameLine();
    FCogWidgets::SetNextItemToShortWidth();
    ImGui::DragFloat(InputLabel, Snap, 0.1f, 0.1f, 1000.0f, "%.1f");
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Transform::RenderLocation(FTransform& InOutTransform)
{
    ImGui::PushID("Location");
    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::Text("Location");

    FVector Location = InOutTransform.GetTranslation();
    bool Result = false;

    ImGui::TableNextColumn();
    if (RenderComponent("##X", &Location.X, Config->LocationSpeed, 0.0, 0.0, 0.0))
    {
        InOutTransform.SetTranslation(Location);
        Result = true;
    }

    ImGui::TableNextColumn();
    if (RenderComponent("##Y", &Location.Y, Config->LocationSpeed, 0.0, 0.0, 0.0))
    {
        InOutTransform.SetTranslation(Location);
        Result = true;
    }

    ImGui::TableNextColumn();
    if (RenderComponent("##Z", &Location.Z, Config->LocationSpeed, 0.0, 0.0, 0.0))
    {
        InOutTransform.SetTranslation(Location);
        Result = true;
    }

    ImGui::PopID();

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Transform::RenderRotation(FTransform& InOutTransform)
{
    ImGui::PushID("Rotation");
    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::Text("Rotation");

    FRotator Rotation = InOutTransform.GetRotation().Rotator();
    bool Result = false;

    ImGui::TableNextColumn();
    if (RenderComponent("##X", &Rotation.Yaw, Config->RotationSpeed, 0.0, 0.0, 0.0))
    {
        InOutTransform.SetRotation(Rotation.Quaternion());
        Result = true;
    }

    ImGui::TableNextColumn();
    if (RenderComponent("##Y", &Rotation.Pitch, Config->RotationSpeed, 0.0, 0.0, 0.0))
    {
        InOutTransform.SetRotation(Rotation.Quaternion());
        Result = true;
    }

    ImGui::TableNextColumn();
    if (RenderComponent("##Z", &Rotation.Roll, Config->RotationSpeed, 0.0, 0.0, 0.0))
    {
        InOutTransform.SetRotation(Rotation.Quaternion());
        Result = true;
    }


    ImGui::PopID();

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Transform::RenderScale(FTransform& InOutTransform)
{
    ImGui::PushID("Scale");
    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::Text("Scale");

    FVector Scale = InOutTransform.GetScale3D();
    bool Result = false;

    ImGui::TableNextColumn();
    if (RenderComponent("##X", &Scale.X, Config->ScaleSpeed, 0.01, 1000.0, 1.0))
    {
        InOutTransform.SetScale3D(Scale);
        Result = true;
    }

    ImGui::TableNextColumn();
    if (RenderComponent("##Y", &Scale.Y, Config->ScaleSpeed, 0.01, 1000.0, 1.0))
    {
        InOutTransform.SetScale3D(Scale);
        Result = true;
    }

    ImGui::TableNextColumn();
    if (RenderComponent("##Z", &Scale.Z, Config->ScaleSpeed, 0.01, 1000.0, 1.0))
    {
        InOutTransform.SetScale3D(Scale);
        Result = true;
    }

    ImGui::PopID();

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Transform::RenderTransform(FTransform& InOutTransform)
{
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(1.0f, 1.0f));

    bool Result = false;

    if (ImGui::BeginTable("Pools", 4, ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 5);
        ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthStretch);

        Result |= RenderLocation(InOutTransform);
        Result |= RenderRotation(InOutTransform);
        Result |= RenderScale(InOutTransform);

        ImGui::EndTable();
    }

    return Result;
}
