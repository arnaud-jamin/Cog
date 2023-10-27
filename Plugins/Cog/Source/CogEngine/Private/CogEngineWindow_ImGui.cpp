#include "CogEngineWindow_ImGui.h"

#include "imgui.h"
#include "implot.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_ImGui::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);

    if (bShowImguiDemo)
    {
        ImGui::ShowDemoWindow(&bShowImguiDemo);
    }

    if (bShowImguiMetric)
    {
        ImGui::ShowMetricsWindow(&bShowImguiMetric);
    }

    if (bShowImguiDebugLog)
    {
        ImGui::ShowDebugLogWindow(&bShowImguiDebugLog);
    }

    if (bShowImguiStyleEditor)
    {
        ImGui::Begin("Dear ImGui Style Editor", &bShowImguiStyleEditor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }

    if (bShowImguiPlot)
    {
        ImPlot::ShowDemoWindow(&bShowImguiPlot);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_ImGui::RenderContent()
{
    Super::RenderContent();

    ImGui::MenuItem("ImGui Demo", nullptr, &bShowImguiDemo);
    ImGui::MenuItem("ImGui Metric", nullptr, &bShowImguiMetric);
    ImGui::MenuItem("ImGui Debug Log", nullptr, &bShowImguiDebugLog);
    ImGui::MenuItem("ImGui Style Editor", nullptr, &bShowImguiStyleEditor);
    ImGui::MenuItem("Plot Demo", "", &bShowImguiPlot);
}
