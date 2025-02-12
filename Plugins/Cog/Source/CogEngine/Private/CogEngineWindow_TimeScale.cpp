#include "CogEngineWindow_TimeScale.h"

#include "CogEngineReplicator.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::Initialize()
{
    Super::Initialize();

    bHasWidget = true;
    bIsWidgetVisible = true;
    
    Config = GetConfig<UCogEngineWindowConfig_TimeScale>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::RenderHelp()
{
    ImGui::Text(
        "This window can be used to change the game global time scale. "
        "If changed on a client the time scale is also modified on the game server. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::RenderContent()
{
    Super::RenderContent();

    ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*GetWorld());
    if (Replicator == nullptr)
    {
        ImGui::TextDisabled("Invalid Replicator");
        return;
    }

    RenderTimeScaleChoices(Replicator);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::RenderContextMenu()
{
    if (IsWindowRenderedInMainMenu() == false)
    {
        ImGui::Checkbox("Inline", &Config->Inline);
    }

    FCogImguiHelper::ColorEdit4("Time Scale Modified Color", Config->TimeScaleModifiedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
    ImGui::SetItemTooltip("Color of the current time scale, in widget mode, when the time scale in not 1.");
    
    FCogWidgets::FloatArray("Time Scales", Config->TimeScales, 10, ImVec2(0, ImGui::GetFontSize() * 10));
    
    ImGui::Separator();
    FCogWindow::RenderContextMenu();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::RenderMainMenuWidget()
{
    Super::RenderMainMenuWidget();
    
    ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*GetWorld());
    if (Replicator == nullptr)
    {
        ImGui::TextDisabled("x?");
        ImGui::SetItemTooltip("Invalid Replicator");
        return;
    }

    const float CurrentValue = Replicator->GetTimeDilation();
    if (CurrentValue != 1.0f)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, FCogImguiHelper::ToImVec4(Config->TimeScaleModifiedColor));
    }
    
    const auto CurrentValueText = StringCast<ANSICHAR>(*FString::Printf(TEXT("x%g"), CurrentValue));
    const bool Open = ImGui::BeginMenu(CurrentValueText.Get());
    
    if (CurrentValue != 1)
    {
        ImGui::PopStyleColor();
    }
    
    if (ImGui::BeginPopupContextItem())
    {
        RenderContextMenu();
        ImGui::EndPopup();
    }
    
    if (Open)
    {
        for (int32 i = 0; i < Config->TimeScales.Num(); ++i)
        {
            const float Value = Config->TimeScales[i];
            const auto ValueText = StringCast<ANSICHAR>(*FString::Printf(TEXT("%g"), Value));
            if (ImGui::Selectable(ValueText.Get(), Value == Replicator->GetTimeDilation()))
            {
                Replicator->SetTimeDilation(Value);
            }
        }
        
        ImGui::EndMenu();
    }
    else
    {
        ImGui::SetItemTooltip("Time Scale");
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::RenderTimeScaleChoices(ACogEngineReplicator* Replicator)
{
    float Value = Replicator->GetTimeDilation();
    if (FCogWidgets::MultiChoiceButtonsFloat(Config->TimeScales, Value, ImVec2(3.5f * FCogWidgets::GetFontWidth(), 0), Config->Inline))
    {
        Replicator->SetTimeDilation(Value);
    }
}