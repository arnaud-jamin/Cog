#include "CogEngineWindow_TimeScale.h"

#include "CogEngineReplicator.h"
#include "CogWindowWidgets.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_TimeScale::Initialize()
{
    Super::Initialize();

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

    float Value = Replicator->GetTimeDilation();
    if (FCogWindowWidgets::MultiChoiceButtonsFloat(TimingScales, Value, ImVec2(3.5f * FCogWindowWidgets::GetFontWidth(), 0)))
    {
        Replicator->SetTimeDilation(Value);
    }

}
