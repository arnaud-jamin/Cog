#include "CogSampleGameState.h"

#include "CogSampleAbilitySystemComponent.h"
#include "CogSampleFunctionLibrary_Tag.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameState.h"
#include "Modules/ModuleManager.h"

#if ENABLE_COG
#include "CogAbilityDataAsset.h"
#include "CogAbilityModule.h"
#include "CogAbilityWindow_Abilities.h"
#include "CogAbilityWindow_Attributes.h"
#include "CogAbilityWindow_Cheats.h"
#include "CogAbilityWindow_Effects.h"
#include "CogAbilityWindow_Pools.h"
#include "CogAbilityWindow_Tags.h"
#include "CogAbilityWindow_Tweaks.h"
#include "CogAIWindow_BehaviorTree.h"
#include "CogAIWindow_Blackboard.h"
#include "CogDebugDrawImGui.h"
#include "CogDebugPlot.h"
#include "CogEngineDataAsset.h"
#include "CogEngineModule.h"
#include "CogEngineWindow_Collisions.h"
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
#include "CogEngineWindow_Spawns.h"
#include "CogEngineWindow_Stats.h"
#include "CogEngineWindow_TimeScale.h"
#include "CogImguiModule.h"
#include "CogInputDataAsset.h"
#include "CogInputWindow_Actions.h"
#include "CogInputWindow_Gamepad.h"
#include "CogSampleLogCategories.h"
#include "CogWindowManager.h"

#include "GameFramework/GameUserSettings.h"

#endif //ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
ACogSampleGameState::ACogSampleGameState(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetActorTickEnabled(true);
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.SetTickFunctionEnable(true);
    PrimaryActorTick.bStartWithTickEnabled = true;

    AbilitySystemComponent = CreateDefaultSubobject<UCogSampleAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleGameState::BeginPlay()
{
    Super::BeginPlay();

    AbilitySystemComponent->InitAbilityActorInfo(this, this);

#if ENABLE_COG
    InitializeCog();
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

#if ENABLE_COG
    
    if (CogWindowManager != nullptr)
    {
        CogWindowManager->Shutdown();
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleGameState::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

#if ENABLE_COG

    extern ENGINE_API float GAverageFPS;
    extern ENGINE_API float GAverageMS;
    FCogDebugPlot::PlotValue(this, "Frame Rate", GAverageFPS);
    FCogDebugPlot::PlotValue(this, "Frame Time", GAverageMS);

    //const float LocalWorldTime = GetWorld()->GetTimeSeconds();
    //const float ServerWorldTime = GetServerWorldTimeSeconds();
    //const float ServerWorldTimeDelta = LocalWorldTime - ServerWorldTime;
    //FCogDebugPlot::PlotValue(this, "Server World Time Delta", ServerWorldTimeDelta);

    CogWindowManager->Tick(DeltaSeconds);

#endif //ENABLE_COG
}

#if ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleGameState::InitializeCog()
{
    CogWindowManager = NewObject<UCogWindowManager>(this);
    CogWindowManagerRef = CogWindowManager;

    //---------------------------------------
    // Engine
    //---------------------------------------
    CogWindowManager->AddWindow<FCogEngineWindow_Collisions>("Engine.Collision");

    CogWindowManager->AddWindow<FCogEngineWindow_CommandBindings>("Engine.Command Bindings");

    CogWindowManager->AddWindow<FCogEngineWindow_DebugSettings>("Engine.Debug Settings");

    CogWindowManager->AddWindow<FCogEngineWindow_ImGui>("Engine.ImGui");

    FCogEngineWindow_Inspector* Inspector = CogWindowManager->AddWindow<FCogEngineWindow_Inspector>("Engine.Inspector");
    Inspector->AddFavorite(GEngine->GetGameUserSettings(), [](UObject* Object)
    {
        if (UGameUserSettings* UserSettings = Cast<UGameUserSettings>(Object))
        {
            UserSettings->ApplySettings(true);
        }
    });

    CogWindowManager->AddWindow<FCogEngineWindow_LogCategories>("Engine.Log Categories");

    CogWindowManager->AddWindow<FCogEngineWindow_NetEmulation>("Engine.Net Emulation");

    CogWindowManager->AddWindow<FCogEngineWindow_OutputLog>("Engine.Output Log");

    CogWindowManager->AddWindow<FCogEngineWindow_Metrics>("Engine.Metrics");

    CogWindowManager->AddWindow<FCogEngineWindow_Plots>("Engine.Plots");

    FCogEngineWindow_Selection* SelectionWindow = CogWindowManager->AddWindow<FCogEngineWindow_Selection>("Engine.Selection");
    SelectionWindow->SetActorClasses({ ACharacter::StaticClass(), AActor::StaticClass(), AGameModeBase::StaticClass(), AGameStateBase::StaticClass() });
    SelectionWindow->SetTraceType(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn));

    CogWindowManager->AddWindow<FCogEngineWindow_Scalability>("Engine.Scalability");

    CogWindowManager->AddWindow<FCogEngineWindow_Skeleton>("Engine.Skeleton");

    CogWindowManager->AddWindow<FCogEngineWindow_Spawns>("Engine.Spawns");

    FCogEngineWindow_Stats* StatsWindow = CogWindowManager->AddWindow<FCogEngineWindow_Stats>("Engine.Stats");

    CogWindowManager->AddWindow<FCogEngineWindow_TimeScale>("Engine.Time Scale");

    //---------------------------------------
    // Abilities
    //---------------------------------------
    CogWindowManager->AddWindow<FCogAbilityWindow_Abilities>("Gameplay.Abilities");

    CogWindowManager->AddWindow<FCogAbilityWindow_Attributes>("Gameplay.Attributes");

    CogWindowManager->AddWindow<FCogAbilityWindow_BlockedTags>("Gameplay.Blocking Tags");

    CogWindowManager->AddWindow<FCogAbilityWindow_Cheats>("Gameplay.Cheats");

    CogWindowManager->AddWindow<FCogAbilityWindow_Effects>("Gameplay.Effects");

    CogWindowManager->AddWindow<FCogAbilityWindow_Pools>("Gameplay.Pools");

    CogWindowManager->AddWindow<FCogAbilityWindow_OwnedTags>("Gameplay.Owned Tags");

    CogWindowManager->AddWindow<FCogAbilityWindow_Tweaks>("Gameplay.Tweaks");

    //---------------------------------------
    // AI
    //---------------------------------------
    CogWindowManager->AddWindow<FCogAIWindow_BehaviorTree>("AI.Behavior Tree");

    CogWindowManager->AddWindow<FCogAIWindow_Blackboard>("AI.Blackboard");

    //---------------------------------------
    // Input
    //---------------------------------------
    CogWindowManager->AddWindow<FCogInputWindow_Actions>("Input.Actions");

    CogWindowManager->AddWindow<FCogInputWindow_Gamepad>("Input.Gamepad");
}

#endif //ENABLE_COG
