#pragma once

#include "CoreMinimal.h"
#include "CogEngineDataAsset.h"
#include "GameFramework/Actor.h"
#include "UObject/Class.h"
#include "UObject/ObjectMacros.h"
#include "CogEngineReplicator.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCogEngine, Verbose, All);

class APlayerController;

using FCogEngineSpawnFunction = TFunction<void(const FCogEngineSpawnEntry& SpawnEntry)>;

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(NotBlueprintable, NotBlueprintType, notplaceable, noteditinlinenew, hidedropdown, Transient)
class COGENGINE_API ACogEngineReplicator : public AActor
{
    GENERATED_UCLASS_BODY()

public:

    static ACogEngineReplicator* Spawn(APlayerController* Controller);

    static ACogEngineReplicator* GetLocalReplicator(const UWorld& World);

    static void GetRemoteReplicators(const UWorld& World, TArray<ACogEngineReplicator*>& Replicators);

    virtual void BeginPlay() override;

    APlayerController* GetPlayerController() const { return OwnerPlayerController.Get(); }

    FCogEngineSpawnFunction GetSpawnFunction() const { return SpawnFunction; }

    void SetSpawnFunction(const FCogEngineSpawnFunction& Value) { SpawnFunction = Value; }

    UFUNCTION(Server, Reliable)
    void Server_Spawn(const FCogEngineSpawnEntry& SpawnEntry);

    float GetTimeDilation() const { return TimeDilation; }

    void SetTimeDilation(float Value);

    UFUNCTION(Server, Reliable)
    void Server_Possess(APawn* Pawn);

    UFUNCTION(Server, Reliable)
    void Server_ResetPossession();

    UFUNCTION(Server, Reliable)
    void Server_DeleteActor(AActor* Actor);

    UFUNCTION(Reliable, Server)
    void Server_ApplyCheat(const AActor* CheatInstigator, const TArray<AActor*>& TargetActors, const FCogEngineCheat& Cheat) const;

	ECogEngineCheat_ActiveState IsCheatActiveOnTargets(const TArray<AActor*>& Targets, const FCogEngineCheat& Cheat) const;

protected:

    UFUNCTION(Server, Reliable)
    void Server_SetTimeDilation(float Value);

    UFUNCTION()
    void OnRep_TimeDilation() const;

    TWeakObjectPtr<APlayerController> OwnerPlayerController;

    uint32 bHasAuthority : 1;
    uint32 bIsLocal : 1;

private:

    UPROPERTY(ReplicatedUsing = OnRep_TimeDilation)
    float TimeDilation = 1.0f;

    FCogEngineSpawnFunction SpawnFunction;
};
