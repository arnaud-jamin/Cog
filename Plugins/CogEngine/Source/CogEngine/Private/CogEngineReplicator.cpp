#include "CogEngineReplicator.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineReplicator::Create(APlayerController* Controller)
{
    if (Controller->GetWorld()->GetNetMode() != NM_Client)
    {
        FActorSpawnParameters SpawnInfo;
        SpawnInfo.Owner = Controller;
        Controller->GetWorld()->SpawnActor<ACogEngineReplicator>(SpawnInfo);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
ACogEngineReplicator::ACogEngineReplicator(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
#if !UE_BUILD_SHIPPING

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bAllowTickOnDedicatedServer = true;
    PrimaryActorTick.bTickEvenWhenPaused = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics;

    bHasAuthority = false;
    bIsLocal = false;
    bReplicates = true;
    bOnlyRelevantToOwner = true;

#endif // !UE_BUILD_SHIPPING
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineReplicator::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    check(World);
    const ENetMode NetMode = World->GetNetMode();
    bHasAuthority = NetMode != NM_Client;
    bIsLocal = NetMode != NM_DedicatedServer;

    OwnerPlayerController = Cast<APlayerController>(GetOwner());

    if (OwnerPlayerController->IsLocalController())
    {
        FCogEngineModule::Get().SetLocalReplicator(this);
    }
    else
    {
        FCogEngineModule::Get().AddRemoteReplicator(this);
    }
}

//--------------------------------------------------------------------------------------------------------------------------

void ACogEngineReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;

    DOREPLIFETIME_WITH_PARAMS_FAST(ACogEngineReplicator, TimeDilation, Params);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineReplicator::TickSpawnActions()
{
#if !UE_BUILD_SHIPPING

    if (OwnerPlayerController == nullptr)
    {
        return;
    }

    for (int32 i = SpawnActions.Num() - 1; i >= 0; --i)
    {
        const FCogEngineReplicatorSpawnAction& SpawnInfo = SpawnActions[i];
        SpawnActions.RemoveAt(i);
    }

#endif // !UE_BUILD_SHIPPING
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineReplicator::Server_SetTimeDilation_Implementation(float Value)
{
#if !UE_BUILD_SHIPPING

    COMPARE_ASSIGN_AND_MARK_PROPERTY_DIRTY(ACogEngineReplicator, TimeDilation, Value, this);
    OnRep_TimeDilation();

#endif // !UE_BUILD_SHIPPING
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineReplicator::OnRep_TimeDilation()
{
#if !UE_BUILD_SHIPPING

    UWorld* World = GetWorld();
    if (World == nullptr)
        return;

    AWorldSettings* WorldSettings = World->GetWorldSettings();
    if (WorldSettings == nullptr)
        return;

    WorldSettings->SetTimeDilation(TimeDilation);

#endif // !UE_BUILD_SHIPPING
}
