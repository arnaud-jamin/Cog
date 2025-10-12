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
#include "CogAIWindow_Perception.h"
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
void Cog::AddAllWindows(UCogSubsystem& CogSubsystem)
{
    //---------------------------------------
    // Engine
    //---------------------------------------
    CogSubsystem.AddWindow<FCogEngineWindow_BuildInfo>("Engine.Build Info");

    CogSubsystem.AddWindow<FCogEngineWindow_CollisionTester>("Engine.Collision Tester");

    CogSubsystem.AddWindow<FCogEngineWindow_CollisionViewer>("Engine.Collision Viewer");

    CogSubsystem.AddWindow<FCogEngineWindow_CommandBindings>("Engine.Command Bindings");

    CogSubsystem.AddWindow<FCogEngineWindow_Console>("Engine.Console");

    CogSubsystem.AddWindow<FCogEngineWindow_DebugSettings>("Engine.Debug Settings");

    CogSubsystem.AddWindow<FCogEngineWindow_ImGui>("Engine.ImGui");

    FCogEngineWindow_Inspector* Inspector = CogSubsystem.AddWindow<FCogEngineWindow_Inspector>("Engine.Inspector");
    Inspector->AddFavorite(GEngine->GetGameUserSettings(), [](UObject* Object)
        {
            if (UGameUserSettings* UserSettings = Cast<UGameUserSettings>(Object))
            {
                UserSettings->ApplySettings(true);
            }
        });

    CogSubsystem.AddWindow<FCogEngineWindow_Levels>("Engine.Levels");

    CogSubsystem.AddWindow<FCogEngineWindow_LogCategories>("Engine.Log Categories");

    CogSubsystem.AddWindow<FCogEngineWindow_Metrics>("Engine.Metrics");

    CogSubsystem.AddWindow<FCogEngineWindow_NetEmulation>("Engine.Net Emulation");

    CogSubsystem.AddWindow<FCogEngineWindow_NetImgui>("Engine.Net ImGui");

    CogSubsystem.AddWindow<FCogEngineWindow_Notifications>("Engine.Notifications");

    CogSubsystem.AddWindow<FCogEngineWindow_OutputLog>("Engine.Output Log");

    CogSubsystem.AddWindow<FCogEngineWindow_Plots>("Engine.Plots");

    CogSubsystem.AddWindow<FCogEngineWindow_Selection>("Engine.Selection");

    CogSubsystem.AddWindow<FCogEngineWindow_Scalability>("Engine.Scalability");

    CogSubsystem.AddWindow<FCogEngineWindow_Skeleton>("Engine.Skeleton");

    CogSubsystem.AddWindow<FCogEngineWindow_Slate>("Engine.Slate");

    CogSubsystem.AddWindow<FCogEngineWindow_Spawns>("Engine.Spawns");

    CogSubsystem.AddWindow<FCogEngineWindow_Stats>("Engine.Stats");

    CogSubsystem.AddWindow<FCogEngineWindow_TimeScale>("Engine.Time Scale");

    CogSubsystem.AddWindow<FCogEngineWindow_Transform>("Engine.Transform");
    
    //---------------------------------------
    // Abilities
    //---------------------------------------
    CogSubsystem.AddWindow<FCogAbilityWindow_Abilities>("Gameplay.Abilities");

    CogSubsystem.AddWindow<FCogAbilityWindow_Attributes>("Gameplay.Attributes");

    CogSubsystem.AddWindow<FCogAbilityWindow_BlockedTags>("Gameplay.Blocking Tags");

    CogSubsystem.AddWindow<FCogEngineWindow_Cheats>("Gameplay.Cheats");

    CogSubsystem.AddWindow<FCogAbilityWindow_Effects>("Gameplay.Effects");

    CogSubsystem.AddWindow<FCogAbilityWindow_Pools>("Gameplay.Pools");

    CogSubsystem.AddWindow<FCogAbilityWindow_OwnedTags>("Gameplay.Owned Tags");

    CogSubsystem.AddWindow<FCogAbilityWindow_Tasks>("Gameplay.Tasks");

    CogSubsystem.AddWindow<FCogAbilityWindow_Tweaks>("Gameplay.Tweaks");

    //---------------------------------------
    // AI
    //---------------------------------------
    CogSubsystem.AddWindow<FCogAIWindow_BehaviorTree>("AI.Behavior Tree");

    CogSubsystem.AddWindow<FCogAIWindow_Blackboard>("AI.Blackboard");
  
    CogSubsystem.AddWindow<FCogAIWindow_Perception>("AI.Perception");

    //---------------------------------------
    // Input
    //---------------------------------------
    CogSubsystem.AddWindow<FCogInputWindow_Actions>("Input.Actions");

    CogSubsystem.AddWindow<FCogInputWindow_Gamepad>("Input.Gamepad");
}
