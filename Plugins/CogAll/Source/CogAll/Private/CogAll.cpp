#include "CogAll.h"

#include "CogAbilityWindow_Abilities.h"
#include "CogAbilityWindow_Attributes.h"
#include "CogAbilityWindow_Effects.h"
#include "CogAbilityWindow_Pools.h"
#include "CogAbilityWindow_Tags.h"
#include "CogAbilityWindow_Tasks.h"
#include "CogAbilityWindow_Tweaks.h"
#include "CogAIWindow_BehaviorTree.h"
#include "CogAIWindow_Blackboard.h"
#include "CogEngineWindow_BuildInfo.h"
#include "CogEngineWindow_Cheats.h"
#include "CogEngineWindow_CollisionTester.h"
#include "CogEngineWindow_CollisionViewer.h"
#include "CogEngineWindow_CommandBindings.h"
#include "CogEngineWindow_Console.h"
#include "CogEngineWindow_DebugSettings.h"
#include "CogEngineWindow_ImGui.h"
#include "CogEngineWindow_Inspector.h"
#include "CogEngineWindow_Levels.h"
#include "CogEngineWindow_LogCategories.h"
#include "CogEngineWindow_Metrics.h"
#include "CogEngineWindow_NetImgui.h"
#include "CogEngineWindow_NetEmulation.h"
#include "CogEngineWindow_Notifications.h"
#include "CogEngineWindow_OutputLog.h"
#include "CogEngineWindow_Plots.h"
#include "CogEngineWindow_Scalability.h"
#include "CogEngineWindow_Selection.h"
#include "CogEngineWindow_Skeleton.h"
#include "CogEngineWindow_Slate.h"
#include "CogEngineWindow_Spawns.h"
#include "CogEngineWindow_Stats.h"
#include "CogEngineWindow_TimeScale.h"
#include "CogEngineWindow_Transform.h"
#include "CogInputWindow_Actions.h"
#include "CogInputWindow_Gamepad.h"
#include "CogSubsystem.h"

#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"

//--------------------------------------------------------------------------------------------------------------------------
void Cog::AddAllWindows(UCogSubsystem& CogWindowManager)
{
    //---------------------------------------
    // Engine
    //---------------------------------------
    CogWindowManager.AddWindow<FCogEngineWindow_BuildInfo>("Engine.Build Info");

    CogWindowManager.AddWindow<FCogEngineWindow_CollisionTester>("Engine.Collision Tester");

    CogWindowManager.AddWindow<FCogEngineWindow_CollisionViewer>("Engine.Collision Viewer");

    CogWindowManager.AddWindow<FCogEngineWindow_CommandBindings>("Engine.Command Bindings");

    CogWindowManager.AddWindow<FCogEngineWindow_Console>("Engine.Console");

    CogWindowManager.AddWindow<FCogEngineWindow_DebugSettings>("Engine.Debug Settings");

    CogWindowManager.AddWindow<FCogEngineWindow_ImGui>("Engine.ImGui");

    FCogEngineWindow_Inspector* Inspector = CogWindowManager.AddWindow<FCogEngineWindow_Inspector>("Engine.Inspector");
    Inspector->AddFavorite(GEngine->GetGameUserSettings(), [](UObject* Object)
        {
            if (UGameUserSettings* UserSettings = Cast<UGameUserSettings>(Object))
            {
                UserSettings->ApplySettings(true);
            }
        });

    CogWindowManager.AddWindow<FCogEngineWindow_Levels>("Engine.Levels");

    CogWindowManager.AddWindow<FCogEngineWindow_LogCategories>("Engine.Log Categories");

    CogWindowManager.AddWindow<FCogEngineWindow_Metrics>("Engine.Metrics");

    CogWindowManager.AddWindow<FCogEngineWindow_NetEmulation>("Engine.Net Emulation");

    CogWindowManager.AddWindow<FCogEngineWindow_NetImgui>("Engine.Net ImGui");

    CogWindowManager.AddWindow<FCogEngineWindow_Notifications>("Engine.Notifications");

    CogWindowManager.AddWindow<FCogEngineWindow_OutputLog>("Engine.Output Log");

    CogWindowManager.AddWindow<FCogEngineWindow_Plots>("Engine.Plots");

    CogWindowManager.AddWindow<FCogEngineWindow_Selection>("Engine.Selection");

    CogWindowManager.AddWindow<FCogEngineWindow_Scalability>("Engine.Scalability");

    CogWindowManager.AddWindow<FCogEngineWindow_Skeleton>("Engine.Skeleton");

    CogWindowManager.AddWindow<FCogEngineWindow_Slate>("Engine.Slate");

    CogWindowManager.AddWindow<FCogEngineWindow_Spawns>("Engine.Spawns");

    CogWindowManager.AddWindow<FCogEngineWindow_Stats>("Engine.Stats");

    CogWindowManager.AddWindow<FCogEngineWindow_TimeScale>("Engine.Time Scale");

    CogWindowManager.AddWindow<FCogEngineWindow_Transform>("Engine.Transform");
    
    //---------------------------------------
    // Abilities
    //---------------------------------------
    CogWindowManager.AddWindow<FCogAbilityWindow_Abilities>("Gameplay.Abilities");

    CogWindowManager.AddWindow<FCogAbilityWindow_Attributes>("Gameplay.Attributes");

    CogWindowManager.AddWindow<FCogAbilityWindow_BlockedTags>("Gameplay.Blocking Tags");

    CogWindowManager.AddWindow<FCogEngineWindow_Cheats>("Gameplay.Cheats");

    CogWindowManager.AddWindow<FCogAbilityWindow_Effects>("Gameplay.Effects");

    CogWindowManager.AddWindow<FCogAbilityWindow_Pools>("Gameplay.Pools");

    CogWindowManager.AddWindow<FCogAbilityWindow_OwnedTags>("Gameplay.Owned Tags");

    CogWindowManager.AddWindow<FCogAbilityWindow_Tasks>("Gameplay.Tasks");

    CogWindowManager.AddWindow<FCogAbilityWindow_Tweaks>("Gameplay.Tweaks");

    //---------------------------------------
    // AI
    //---------------------------------------
    CogWindowManager.AddWindow<FCogAIWindow_BehaviorTree>("AI.Behavior Tree");

    CogWindowManager.AddWindow<FCogAIWindow_Blackboard>("AI.Blackboard");

    //---------------------------------------
    // Input
    //---------------------------------------
    CogWindowManager.AddWindow<FCogInputWindow_Actions>("Input.Actions");

    CogWindowManager.AddWindow<FCogInputWindow_Gamepad>("Input.Gamepad");
}
