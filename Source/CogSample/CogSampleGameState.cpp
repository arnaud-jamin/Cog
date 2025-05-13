#include "CogSampleGameState.h"

#include "CogSampleAbilitySystemComponent.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Modules/ModuleManager.h"
#include "Net/UnrealNetwork.h"

#if ENABLE_COG
#include "CogAll.h"
#include "CogSampleWindow_Team.h"
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
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
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
        FCogDebug::Plot(this, "Frame Rate Client Raw", GAverageFPS);
        FCogDebug::Plot(this, "Frame Rate Client Smooth", _ClientFramerateSmooth);
        FCogDebug::Plot(this, "Frame Rate Server Raw", _ServerFramerateRaw);
        FCogDebug::Plot(this, "Frame Rate Server Smooth", _ServerFramerateSmooth);

        if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
        {
            if (const APlayerController* PlayerController = LocalPlayer->PlayerController)
            {
                if (const APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>())
                {
                    FCogDebug::Plot(this, "Ping", PlayerState->GetPingInMilliseconds());
                }

                if (const UNetConnection* Connection = PlayerController->GetNetConnection())
                {
                    FCogDebug::Plot(this,
                        "Packet Loss In",
                        Connection->GetInLossPercentage().GetAvgLossPercentage() * 100.0f);

                    FCogDebug::Plot(this,
                        "Packet Loss Out",
                        Connection->GetOutLossPercentage().GetAvgLossPercentage() * 100.0f);
                }
            }
        }
    }
    else
    {
        FCogDebug::Plot(this, "Frame Rate Raw", GAverageFPS);
        FCogDebug::Plot(this, "Frame Rate Smooth", _ClientFramerateSmooth);
    }

#endif //ENABLE_COG
}
