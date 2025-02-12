#include "CogSampleGameInstance.h"

#include "CogCommon.h"

#if ENABLE_COG
#include "CogAll.h"
#include "CogSampleWindow_Team.h"
#include "CogSubsystem.h"
#endif


void UCogSampleGameInstance::Init()
{
    Super::Init();

#if ENABLE_COG

    // Get the cog subsystem  
    if (UCogSubsystem* CogSubSystem = GetSubsystem<UCogSubsystem>())
    {
        // Add all the built-in windows. You copy paste this function code to organize the menu differently.
        Cog::AddAllWindows(*CogSubSystem);

        // Add a custom window
        CogSubSystem->AddWindow<FCogSampleWindow_Team>("Gameplay.Team");

        // Activate Cog
        CogSubSystem->Activate();
    }
#endif 
}