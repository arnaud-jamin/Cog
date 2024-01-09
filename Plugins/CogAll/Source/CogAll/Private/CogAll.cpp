#include "CogAll.h"

#include "CogAbilityWindow_Abilities.h"
#include "CogAbilityWindow_Attributes.h"
#include "CogAbilityWindow_Cheats.h"
#include "CogAbilityWindow_Effects.h"
#include "CogAbilityWindow_Pools.h"
#include "CogAbilityWindow_Tags.h"
#include "CogAbilityWindow_Tweaks.h"
#include "CogAIWindow_BehaviorTree.h"
#include "CogAIWindow_Blackboard.h"
#include "CogEngineWindow_CollisionTester.h"
#include "CogEngineWindow_CollisionViewer.h"
#include "CogEngineWindow_CommandBindings.h"
#include "CogEngineWindow_DebugSettings.h"
#include "CogEngineWindow_ImGui.h"
#include "CogEngineWindow_Inspector.h"
#include "CogEngineWindow_LogCategories.h"
#include "CogEngineWindow_Metrics.h"
#include "CogEngineWindow_NetEmulation.h"
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
#include "CogWindowManager.h"

#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/GameUserSettings.h"

//--------------------------------------------------------------------------------------------------------------------------
void Cog::AddAllWindows(UCogWindowManager& CogWindowManager)
{
    //---------------------------------------
    // Engine
    //---------------------------------------
    CogWindowManager.AddWindow<FCogEngineWindow_CollisionTester>("Engine.Collision Tester");

    CogWindowManager.AddWindow<FCogEngineWindow_CollisionViewer>("Engine.Collision Viewer");

    CogWindowManager.AddWindow<FCogEngineWindow_CommandBindings>("Engine.Command Bindings");

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

    CogWindowManager.AddWindow<FCogEngineWindow_LogCategories>("Engine.Log Categories");

    CogWindowManager.AddWindow<FCogEngineWindow_NetEmulation>("Engine.Net Emulation");

    CogWindowManager.AddWindow<FCogEngineWindow_OutputLog>("Engine.Output Log");

    CogWindowManager.AddWindow<FCogEngineWindow_Metrics>("Engine.Metrics");

    CogWindowManager.AddWindow<FCogEngineWindow_Plots>("Engine.Plots");

    FCogEngineWindow_Selection* SelectionWindow = CogWindowManager.AddWindow<FCogEngineWindow_Selection>("Engine.Selection");
    SelectionWindow->SetActorClasses({ ACharacter::StaticClass(), AActor::StaticClass(), AGameModeBase::StaticClass(), AGameStateBase::StaticClass() });
    SelectionWindow->SetTraceType(UEngineTypes::ConvertToTraceType(ECC_Pawn));

    CogWindowManager.AddWindow<FCogEngineWindow_Scalability>("Engine.Scalability");

    CogWindowManager.AddWindow<FCogEngineWindow_Skeleton>("Engine.Skeleton");

    CogWindowManager.AddWindow<FCogEngineWindow_Slate>("Engine.Slate");

    CogWindowManager.AddWindow<FCogEngineWindow_Spawns>("Engine.Spawns");

    FCogEngineWindow_Stats* StatsWindow = CogWindowManager.AddWindow<FCogEngineWindow_Stats>("Engine.Stats");

    CogWindowManager.AddWindow<FCogEngineWindow_TimeScale>("Engine.Time Scale");

    CogWindowManager.AddWindow<FCogEngineWindow_Transform>("Engine.Transform");

    //---------------------------------------
    // Abilities
    //---------------------------------------
    CogWindowManager.AddWindow<FCogAbilityWindow_Abilities>("Gameplay.Abilities");

    CogWindowManager.AddWindow<FCogAbilityWindow_Attributes>("Gameplay.Attributes");

    CogWindowManager.AddWindow<FCogAbilityWindow_BlockedTags>("Gameplay.Blocking Tags");

    CogWindowManager.AddWindow<FCogAbilityWindow_Cheats>("Gameplay.Cheats");

    CogWindowManager.AddWindow<FCogAbilityWindow_Effects>("Gameplay.Effects");

    CogWindowManager.AddWindow<FCogAbilityWindow_Pools>("Gameplay.Pools");

    CogWindowManager.AddWindow<FCogAbilityWindow_OwnedTags>("Gameplay.Owned Tags");

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
