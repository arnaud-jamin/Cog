#include "CogSampleWindow_Team.h"

#if ENABLE_COG

#include "CogImguiHelper.h"
#include "CogHelper.h"
#include "CogWidgets.h"
#include "CogSampleTeamInterface.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogSampleWindow_Team::Initialize()
{
    Super::Initialize();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogSampleWindow_Team::RenderHelp()
{
    ImGui::Text("A window part of the sample project.");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogSampleWindow_Team::RenderContent()
{
    Super::RenderContent();

    if (ICogSampleTeamInterface* TeamInterface = Cast<ICogSampleTeamInterface>(GetSelection()))
    {
        int32 Team = TeamInterface->GetTeam();
        if (ImGui::DragInt("Team", &Team, 0.1f, 0, 10))
        {
            TeamInterface->SetTeam(Team);
        }
    }
}

#endif //ENABLE_COG
