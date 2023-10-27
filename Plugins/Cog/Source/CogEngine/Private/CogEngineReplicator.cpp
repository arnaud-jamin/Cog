#include "CogEngineReplicator.h"

#include "CogCommon.h"
#include "CogCommonPossessorInterface.h"
#include "CogEngineDataAsset.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
#include "EngineUtils.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogCogEngine);

//--------------------------------------------------------------------------------------------------------------------------
ACogEngineReplicator* ACogEngineReplicator::Spawn(APlayerController* Controller)
{
    if (Controller->GetWorld()->GetNetMode() == NM_Client)
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnInfo;
    SpawnInfo.Owner = Controller;
    ACogEngineReplicator* Replicator = Controller->GetWorld()->SpawnActor<ACogEngineReplicator>(SpawnInfo);

    return Replicator;
}

//--------------------------------------------------------------------------------------------------------------------------
ACogEngineReplicator* ACogEngineReplicator::GetLocalReplicator(UWorld& World)
{
    for (TActorIterator<ACogEngineReplicator> It(&World, ACogEngineReplicator::StaticClass()); It; ++It)
    {
        ACogEngineReplicator* Replicator = *It;
        return Replicator;
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineReplicator::GetRemoteReplicators(UWorld& World, TArray<ACogEngineReplicator*>& Replicators)
{
    for (TActorIterator<ACogEngineReplicator> It(&World, ACogEngineReplicator::StaticClass()); It; ++It)
    {
        ACogEngineReplicator* Replicator = Cast<ACogEngineReplicator>(*It);
        Replicators.Add(Replicator);
    }
}


//--------------------------------------------------------------------------------------------------------------------------
ACogEngineReplicator::ACogEngineReplicator(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
#if !UE_BUILD_SHIPPING

    bHasAuthority = false;
    bIsLocal = false;
    bReplicates = true;
    bOnlyRelevantToOwner = true;

#endif // !UE_BUILD_SHIPPING
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineReplicator::BeginPlay()
{
    COG_LOG_OBJECT(LogCogEngine, ELogVerbosity::Verbose, this, TEXT(""));

    Super::BeginPlay();

    UWorld* World = GetWorld();
    check(World);
    const ENetMode NetMode = World->GetNetMode();
    bHasAuthority = NetMode != NM_Client;
    bIsLocal = NetMode != NM_DedicatedServer;

    OwnerPlayerController = Cast<APlayerController>(GetOwner());
}

//--------------------------------------------------------------------------------------------------------------------------

void ACogEngineReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

#if !UE_BUILD_SHIPPING

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    DOREPLIFETIME_WITH_PARAMS_FAST(ACogEngineReplicator, TimeDilation, Params);

#endif // !UE_BUILD_SHIPPING
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineReplicator::Server_Spawn_Implementation(const FCogEngineSpawnEntry& SpawnEntry)
{
#if !UE_BUILD_SHIPPING

    if (GetWorld() == nullptr)
    {
        return;
    }

    if (SpawnFunction)
    {
        SpawnFunction(SpawnEntry);
    }
    else
    {
        FTransform Transform(FTransform::Identity);
        if (APawn* Pawn = GetPlayerController()->GetPawn())
        {
            Transform = Pawn->GetTransform();
            Transform.SetLocation(Transform.GetLocation() + Transform.GetUnitAxis(EAxis::X) * 200.0f);
            Transform.SetScale3D(FVector(1.0f));
        }

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        AActor* SpawnerActor = GetWorld()->SpawnActor(SpawnEntry.Class, &Transform, Params);
        if (APawn* SpawnedPawn = Cast<APawn>(SpawnerActor))
        {
            SpawnedPawn->SpawnDefaultController();
        }
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

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineReplicator::Server_Possess_Implementation(APawn* Pawn)
{
#if !UE_BUILD_SHIPPING

    if (ICogCommonPossessorInterface* Possessor = Cast<ICogCommonPossessorInterface>(OwnerPlayerController))
    {
        Possessor->SetPossession(Pawn);
    }

#endif // !UE_BUILD_SHIPPING
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineReplicator::Server_ResetPossession_Implementation()
{
#if !UE_BUILD_SHIPPING

    if (ICogCommonPossessorInterface* Possessor = Cast<ICogCommonPossessorInterface>(OwnerPlayerController))
    {
        Possessor->ResetPossession();
    }

#endif // !UE_BUILD_SHIPPING
}
