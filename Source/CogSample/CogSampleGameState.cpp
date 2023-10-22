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
    CogWindowManager->CreateWindow<UCogEngineWindow_Collisions>("Engine.Collision");

    CogWindowManager->CreateWindow<UCogEngineWindow_CommandBindings>("Engine.Command Bindings");

    CogWindowManager->CreateWindow<UCogEngineWindow_DebugSettings>("Engine.Debug Settings");

    CogWindowManager->CreateWindow<UCogEngineWindow_ImGui>("Engine.ImGui");

    UCogEngineWindow_Inspector* Inspector = CogWindowManager->CreateWindow<UCogEngineWindow_Inspector>("Engine.Inspector");
    Inspector->AddFavorite(GEngine->GetGameUserSettings(), [](UObject* Object)
    {
        if (UGameUserSettings* UserSettings = Cast<UGameUserSettings>(Object))
        {
            UserSettings->ApplySettings(true);
        }
    });

    CogWindowManager->CreateWindow<UCogEngineWindow_LogCategories>("Engine.Log Categories");

    CogWindowManager->CreateWindow<UCogEngineWindow_NetEmulation>("Engine.Net Emulation");

    CogWindowManager->CreateWindow<UCogEngineWindow_OutputLog>("Engine.Output Log");

    CogWindowManager->CreateWindow<UCogEngineWindow_Metrics>("Engine.Metrics");

    CogWindowManager->CreateWindow<UCogEngineWindow_Plots>("Engine.Plots");

    SelectionWindow = CogWindowManager->CreateWindow<UCogEngineWindow_Selection>("Engine.Selection");
    SelectionWindow->SetActorClasses({ ACharacter::StaticClass(), AActor::StaticClass(), AGameModeBase::StaticClass(), AGameStateBase::StaticClass() });
    SelectionWindow->SetTraceType(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn));

    CogWindowManager->CreateWindow<UCogEngineWindow_Scalability>("Engine.Scalability");

    CogWindowManager->CreateWindow<UCogEngineWindow_Skeleton>("Engine.Skeleton");

    CogWindowManager->CreateWindow<UCogEngineWindow_Spawns>("Engine.Spawns");

    UCogEngineWindow_Stats* StatsWindow = CogWindowManager->CreateWindow<UCogEngineWindow_Stats>("Engine.Stats");

    CogWindowManager->CreateWindow<UCogEngineWindow_TimeScale>("Engine.Time Scale");

    //---------------------------------------
    // Abilities
    //---------------------------------------
    CogWindowManager->CreateWindow<UCogAbilityWindow_Abilities>("Gameplay.Abilities");

    CogWindowManager->CreateWindow<UCogAbilityWindow_Attributes>("Gameplay.Attributes");

    CogWindowManager->CreateWindow<UCogAbilityWindow_Cheats>("Gameplay.Cheats");

    CogWindowManager->CreateWindow<UCogAbilityWindow_Effects>("Gameplay.Effects");

    CogWindowManager->CreateWindow<UCogAbilityWindow_Pools>("Gameplay.Pools");

    CogWindowManager->CreateWindow<UCogAbilityWindow_Tags>("Gameplay.Tags");

    CogWindowManager->CreateWindow<UCogAbilityWindow_Tweaks>("Gameplay.Tweaks");

    //---------------------------------------
    // AI
    //---------------------------------------
    CogWindowManager->CreateWindow<UCogAIWindow_BehaviorTree>("AI.Behavior Tree");

    CogWindowManager->CreateWindow<UCogAIWindow_Blackboard>("AI.Blackboard");

    //---------------------------------------
    // Input
    //---------------------------------------
    CogWindowManager->CreateWindow<UCogInputWindow_Actions>("Input.Actions");

    CogWindowManager->CreateWindow<UCogInputWindow_Gamepad>("Input.Gamepad");

    //---------------------------------------
    // Main Menu Widget
    //---------------------------------------
    CogWindowManager->AddMainMenuWidget(SelectionWindow.Get());
    CogWindowManager->AddMainMenuWidget(StatsWindow);
}

#endif //ENABLE_COG
