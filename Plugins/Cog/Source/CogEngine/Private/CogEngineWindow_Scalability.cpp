#include "CogEngineWindow_Scalability.h"

#include "CogCommon.h"
#include "imgui.h"
#include "CogWidgets.h"
#include "Engine/Engine.h"
#include "Scalability.h"

#define SCALABILITY_NUM_LEVELS 5

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Scalability::RenderHelp()
{
    ImGui::Text(
        "This window can be used to configure the rendering quality."
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Scalability::RenderContent()
{
    Super::RenderContent();

    Scalability::FQualityLevels Levels = Scalability::GetQualityLevels();
    const FString CurrentQualityName = Scalability::GetQualityLevelText(Levels.GetMinQualityLevel(), SCALABILITY_NUM_LEVELS).ToString();
    FCogWidgets::SetNextItemToShortWidth();
    if (ImGui::BeginCombo("Scalability", COG_TCHAR_TO_CHAR(*CurrentQualityName)))
    {
        for (int32 i = 0; i < SCALABILITY_NUM_LEVELS; ++i)
        {
            FString QualityName = Scalability::GetQualityLevelText(i, SCALABILITY_NUM_LEVELS).ToString();
            if (ImGui::Selectable(COG_TCHAR_TO_CHAR(*QualityName)))
            {
                Levels.SetFromSingleQualityLevel(i);
                Scalability::SetQualityLevels(Levels);

                ImGui::LogToClipboard();
                ImGui::LogText("%s", COG_TCHAR_TO_CHAR(*FString::Printf(TEXT("Setting Quality Level to %s"), *QualityName)));
                ImGui::LogFinish();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    bool Modified = false;

    FCogWidgets::SetNextItemToShortWidth();
    Modified |= ImGui::SliderFloat("Resolution", &Levels.ResolutionQuality, 10.0f, 100.0f, "%0.f");

    FCogWidgets::SetNextItemToShortWidth();
    Modified |= ImGui::SliderInt("View Distance", &Levels.ViewDistanceQuality, 0, SCALABILITY_NUM_LEVELS - 1);

    FCogWidgets::SetNextItemToShortWidth();
    Modified |= ImGui::SliderInt("Anti Aliasing", &Levels.AntiAliasingQuality, 0, SCALABILITY_NUM_LEVELS - 1);

    FCogWidgets::SetNextItemToShortWidth();
    Modified |= ImGui::SliderInt("Shadow", &Levels.ShadowQuality, 0, SCALABILITY_NUM_LEVELS - 1);

    FCogWidgets::SetNextItemToShortWidth();
    Modified |= ImGui::SliderInt("Global Illumination", &Levels.GlobalIlluminationQuality, 0, SCALABILITY_NUM_LEVELS - 1);

    FCogWidgets::SetNextItemToShortWidth();
    Modified |= ImGui::SliderInt("Reflection", &Levels.ReflectionQuality, 0, SCALABILITY_NUM_LEVELS - 1);

    FCogWidgets::SetNextItemToShortWidth();
    Modified |= ImGui::SliderInt("Post Process", &Levels.PostProcessQuality, 0, SCALABILITY_NUM_LEVELS - 1);

    FCogWidgets::SetNextItemToShortWidth();
    Modified |= ImGui::SliderInt("Texture", &Levels.TextureQuality, 0, SCALABILITY_NUM_LEVELS - 1);

    FCogWidgets::SetNextItemToShortWidth();
    Modified |= ImGui::SliderInt("Effects", &Levels.EffectsQuality, 0, SCALABILITY_NUM_LEVELS - 1);

    FCogWidgets::SetNextItemToShortWidth();
    Modified |= ImGui::SliderInt("Foliage", &Levels.FoliageQuality, 0, SCALABILITY_NUM_LEVELS - 1);

    FCogWidgets::SetNextItemToShortWidth();
    Modified |= ImGui::SliderInt("Shading", &Levels.ShadingQuality, 0, SCALABILITY_NUM_LEVELS - 1);

    if (Modified)
    {
        Scalability::SetQualityLevels(Levels);
    }

}
