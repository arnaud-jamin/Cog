#include "CogSampleGameInstance.h"

#include "CogCommon.h"

#if ENABLE_COG
#include "CogAll.h"
#include "CogSampleWindow_Team.h"
#include "CogWindowManager.h"
#endif 

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleGameInstance::Init()
{
    Super::Init();

#if ENABLE_COG
    if (UCogWindowManager* CogSubSystem = GetSubsystem<UCogWindowManager>())
    {
        Cog::AddAllWindows(*CogSubSystem);
        CogSubSystem->AddWindow<FCogSampleWindow_Team>("Gameplay.Team");
    }
#endif 
}

