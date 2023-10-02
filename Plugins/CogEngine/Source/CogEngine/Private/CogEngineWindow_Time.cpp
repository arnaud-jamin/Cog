#include "CogEngineWindow_Time.h"

#include "CogEngineReplicator.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

//--------------------------------------------------------------------------------------------------------------------------
void DrawTimeButton(ACogEngineReplicator* Replicator, float Value)
{
    const bool IsSelected = FMath::IsNearlyEqual(Replicator->GetTimeDilation(), Value, 0.0001f);
    if (IsSelected)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button,          IM_COL32(128, 128, 128, 50));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   IM_COL32(128, 128, 128, 100));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,    IM_COL32(128, 128, 128, 150));
    }

    const char* Text = TCHAR_TO_ANSI(*FString::Printf(TEXT("%g"), Value).Replace(TEXT("0."), TEXT(".")));
    if (ImGui::Button(Text, ImVec2(3.5f * FCogWindowWidgets::TextBaseWidth, 0)))
    {
        Replicator->Server_SetTimeDilation(Value);
    }

    if (IsSelected)
    {
        ImGui::PopStyleVar();
    }
    else
    {
        ImGui::PopStyleColor(3);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
UCogEngineWindow_Time::UCogEngineWindow_Time()
{
    TimingScales.Add(0.00f);
    TimingScales.Add(0.01f);
    TimingScales.Add(0.10f);
    TimingScales.Add(0.50f);
    TimingScales.Add(1.00f);
    TimingScales.Add(2.00f);
    TimingScales.Add(5.00f);
    TimingScales.Add(10.0f);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Time::RenderContent()
{
    Super::RenderContent();

    ACogEngineReplicator* Replicator = FCogEngineModule::Get().GetLocalReplicator();
    if (Replicator == nullptr)
    {
        return;
    }

    ImGuiStyle& Style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(Style.WindowPadding.x * 0.40f, (float)(int)(Style.WindowPadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(Style.FramePadding.x * 0.40f, (float)(int)(Style.FramePadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(Style.ItemSpacing.x * 0.30f, (float)(int)(Style.ItemSpacing.y * 0.60f)));
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 255, 255, 180));

    for (float TimeScale : TimingScales)
    {
        DrawTimeButton(Replicator, TimeScale); 
        ImGui::SameLine();
    }

    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(3);
}
