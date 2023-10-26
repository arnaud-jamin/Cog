#include "CogEngineWindow_TimeScale.h"

#include "CogEngineReplicator.h"
#include "CogWindowWidgets.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_TimeScale::RenderHelp()
{
    ImGui::Text(
        "This window can be used to change the game global time scale. "
        "If changed on a client the time scale is also modified on the game server. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
UCogEngineWindow_TimeScale::UCogEngineWindow_TimeScale()
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
void UCogEngineWindow_TimeScale::RenderContent()
{
    Super::RenderContent();

    UWorld* World = GetWorld();
    if (World == nullptr)
    {
        return;
    }

    ACogEngineReplicator* Replicator = ACogEngineReplicator::GetLocalReplicator(*World);
    if (Replicator == nullptr)
    {
        return;
    }

  
    float Value = Replicator->GetTimeDilation();
    if (FCogWindowWidgets::MultiChoiceButtonsFloat(TimingScales, Value, ImVec2(3.5f * FCogWindowWidgets::GetFontWidth(), 0)))
    {
        Replicator->Server_SetTimeDilation(Value);
    }

}
