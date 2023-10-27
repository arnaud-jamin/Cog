#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/Class.h"
#include "UObject/ObjectMacros.h"
#include "CogEngineReplicator.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCogEngine, Verbose, All);

class APlayerController;

using FCogEnineSpawnFunction = TFunction<void(const FCogEngineSpawnEntry& SpawnEntry)>;

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(NotBlueprintable, NotBlueprintType, notplaceable, noteditinlinenew, hidedropdown, Transient)
class COGENGINE_API ACogEngineReplicator : public AActor
{
    GENERATED_UCLASS_BODY()

public:

    static ACogEngineReplicator* Spawn(APlayerController* Controller);

    static ACogEngineReplicator* GetLocalReplicator(UWorld& World);

    static void GetRemoteReplicators(UWorld& World, TArray<ACogEngineReplicator*>& Replicators);

    virtual void BeginPlay() override;

    APlayerController* GetPlayerController() const { return OwnerPlayerController.Get(); }

    FCogEnineSpawnFunction GetSpawnFunction() const { return SpawnFunction; }

    void SetSpawnFunction(FCogEnineSpawnFunction Value) { SpawnFunction = Value; }

    UFUNCTION(Server, Reliable)
    void Server_Spawn(const FCogEngineSpawnEntry& SpawnEntry);

    float GetTimeDilation() const { return TimeDilation; }

    void SetTimeDilation(float Value);

    UFUNCTION(Server, Reliable)
    void Server_Possess(APawn* Pawn);

    UFUNCTION(Server, Reliable)
    void Server_ResetPossession();

protected:

    UFUNCTION(Server, Reliable)
    void Server_SetTimeDilation(float Value);

    UFUNCTION()
    void OnRep_TimeDilation();

    TObjectPtr<APlayerController> OwnerPlayerController;

    uint32 bHasAuthority : 1;
    uint32 bIsLocal : 1;

private:

    UPROPERTY(ReplicatedUsing = OnRep_TimeDilation)
    float TimeDilation = 1.0f;

    FCogEnineSpawnFunction SpawnFunction;
};
