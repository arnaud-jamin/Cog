#include "CogSampleGameState.h"

#include "CogSampleAbilitySystemComponent.h"
#include "GameFramework/GameState.h"
#include "Modules/ModuleManager.h"

#if ENABLE_COG
#include "CogAll.h"
#include "CogDebugPlot.h"
#include "CogSampleWindow_Team.h"
#include "CogWindowManager.h"
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
    CogWindowManager = NewObject<UCogWindowManager>(this);
    CogWindowManagerRef = CogWindowManager;

	// Add all the built-in windows
    Cog::AddAllWindows(*CogWindowManager);

	// Add a custom window 
    CogWindowManager->AddWindow<FCogSampleWindow_Team>("Gameplay.Team");
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

    if (CogWindowManager != nullptr)
    {
        CogWindowManager->Tick(DeltaSeconds);
    }

#endif //ENABLE_COG
}
