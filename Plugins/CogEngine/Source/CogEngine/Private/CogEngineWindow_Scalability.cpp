#include "CogEngineWindow_Scalability.h"

#include "imgui.h"
#include "CogImguiHelper.h"
#include "Engine/Engine.h"

#define SCALABILITY_NUM_LEVELS 5

//--------------------------------------------------------------------------------------------------------------------------
UCogEngineWindow_Scalability::UCogEngineWindow_Scalability()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Scalability::RenderContent()
{
    Super::RenderContent();

    Scalability::FQualityLevels Levels = Scalability::GetQualityLevels();
    FString CurrentQualityName = Scalability::GetQualityLevelText(Levels.GetMinQualityLevel(), SCALABILITY_NUM_LEVELS).ToString();
    if (ImGui::BeginCombo("Scalability", TCHAR_TO_ANSI(*CurrentQualityName)))
    {
        for (int32 i = 0; i < SCALABILITY_NUM_LEVELS; ++i)
        {
            FString QualityName = Scalability::GetQualityLevelText(i, SCALABILITY_NUM_LEVELS).ToString();
            if (ImGui::Selectable(TCHAR_TO_ANSI(*QualityName)))
            {
                Levels.SetFromSingleQualityLevel(i);
                Scalability::SetQualityLevels(Levels);

                ImGui::LogToClipboard();
                ImGui::LogText("%s", TCHAR_TO_ANSI(*FString::Printf(TEXT("Setting Quality Level to %s"), *QualityName)));
                ImGui::LogFinish();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    if (ImGui::SliderFloat("Resolution", &Levels.ResolutionQuality, 10.0f, 100.0f, "%0.f"))
    {
        Scalability::SetQualityLevels(Levels);
    }

    if (ImGui::SliderInt("View Distance", &Levels.ViewDistanceQuality, 0, SCALABILITY_NUM_LEVELS - 1))
    {
        Scalability::SetQualityLevels(Levels);
    }

    if (ImGui::SliderInt("Anti Aliasing", &Levels.AntiAliasingQuality, 0, SCALABILITY_NUM_LEVELS - 1))
    {
        Scalability::SetQualityLevels(Levels);
    }

    if (ImGui::SliderInt("Shadow", &Levels.ShadowQuality, 0, SCALABILITY_NUM_LEVELS - 1))
    {
        Scalability::SetQualityLevels(Levels);
    }

    if (ImGui::SliderInt("Global Illumination", &Levels.GlobalIlluminationQuality, 0, SCALABILITY_NUM_LEVELS - 1))
    {
        Scalability::SetQualityLevels(Levels);
    }

    if (ImGui::SliderInt("Reflection", &Levels.ReflectionQuality, 0, SCALABILITY_NUM_LEVELS - 1))
    {
        Scalability::SetQualityLevels(Levels);
    }

    if (ImGui::SliderInt("Post Process", &Levels.PostProcessQuality, 0, SCALABILITY_NUM_LEVELS - 1))
    {
        Scalability::SetQualityLevels(Levels);
    }

    if (ImGui::SliderInt("Texture", &Levels.TextureQuality, 0, SCALABILITY_NUM_LEVELS - 1))
    {
        Scalability::SetQualityLevels(Levels);
    }

    if (ImGui::SliderInt("Effects", &Levels.EffectsQuality, 0, SCALABILITY_NUM_LEVELS - 1))
    {
        Scalability::SetQualityLevels(Levels);
    }

    if (ImGui::SliderInt("Foliage", &Levels.FoliageQuality, 0, SCALABILITY_NUM_LEVELS - 1))
    {
        Scalability::SetQualityLevels(Levels);
    }

    if (ImGui::SliderInt("Shading", &Levels.ShadingQuality, 0, SCALABILITY_NUM_LEVELS - 1))
    {
        Scalability::SetQualityLevels(Levels);
    }
}
