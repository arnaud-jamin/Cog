#include "CogSampleGameInstance.h"

#include "CogCommon.h"

#if ENABLE_COG
#include "CogAll.h"
#include "CogSampleWindow_Team.h"
#include "CogSubsystem.h"
#endif 

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleGameInstance::Init()
{
    Super::Init();

#if ENABLE_COG
    if (UCogSubsystem* CogSubSystem = GetSubsystem<UCogSubsystem>())
    {
        Cog::AddAllWindows(*CogSubSystem);
        CogSubSystem->AddWindow<FCogSampleWindow_Team>("Gameplay.Team");

        CogSubSystem->Activate();
    }
#endif 
}

