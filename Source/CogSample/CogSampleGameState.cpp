#include "CogSampleGameState.h"

#include "CogSampleAbilitySystemComponent.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Modules/ModuleManager.h"
#include "Net/UnrealNetwork.h"

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
void ACogSampleGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ThisClass, _ServerFramerateRaw);
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

    constexpr float smoothing = 10.0f;

    _ClientFramerateSmooth = _ClientFramerateSmooth >= 0.0f
        ? FMath::FInterpTo(_ClientFramerateSmooth, GAverageFPS, DeltaSeconds, smoothing)
        : GAverageFPS;

    _ServerFramerateSmooth = _ServerFramerateSmooth >= 0.0f
        ? FMath::FInterpTo(_ServerFramerateSmooth, _ServerFramerateRaw, DeltaSeconds, smoothing)
        : _ServerFramerateRaw;

    if (GetLocalRole() != ROLE_Authority)
    {
        FCogDebugPlot::PlotValue(this, "Frame Rate Client Raw", GAverageFPS);
        FCogDebugPlot::PlotValue(this, "Frame Rate Client Smooth", _ClientFramerateSmooth);
        FCogDebugPlot::PlotValue(this, "Frame Rate Server Raw", _ServerFramerateRaw);
        FCogDebugPlot::PlotValue(this, "Frame Rate Server Smooth", _ServerFramerateSmooth);

        if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
        {
            if (const APlayerController* PlayerController = LocalPlayer->PlayerController)
            {
                if (const APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>())
                {
                    FCogDebugPlot::PlotValue(this, "Ping", PlayerState->GetPingInMilliseconds());
                }

                if (const UNetConnection* Connection = PlayerController->GetNetConnection())
                {
                    FCogDebugPlot::PlotValue(this,
                        "Packet Loss In",
                        Connection->GetInLossPercentage().GetAvgLossPercentage() * 100.0f);

                    FCogDebugPlot::PlotValue(this,
                        "Packet Loss Out",
                        Connection->GetOutLossPercentage().GetAvgLossPercentage() * 100.0f);
                }
            }
        }
    }
    else
    {
        FCogDebugPlot::PlotValue(this, "Frame Rate Raw", GAverageFPS);
        FCogDebugPlot::PlotValue(this, "Frame Rate Smooth", _ClientFramerateSmooth);
    }


    if (CogWindowManager != nullptr)
    {
        CogWindowManager->Tick(DeltaSeconds);
    }

#endif //ENABLE_COG
}
