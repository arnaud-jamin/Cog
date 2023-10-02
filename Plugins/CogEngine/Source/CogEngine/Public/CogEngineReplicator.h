#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/Class.h"
#include "UObject/ObjectMacros.h"
#include "CogEngineReplicator.generated.h"

class APlayerController;

using FCogEnineSpawnFunction = TFunction<void(const FCogEngineSpawnEntry& SpawnEntry)>;

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(NotBlueprintable, NotBlueprintType, notplaceable, noteditinlinenew, hidedropdown, Transient)
class COGENGINE_API ACogEngineReplicator : public AActor
{
    GENERATED_UCLASS_BODY()

public:
    static void Create(APlayerController* Controller);

    virtual void BeginPlay() override;

    APlayerController* GetPlayerController() const { return OwnerPlayerController.Get(); }

    FCogEnineSpawnFunction GetSpawnFunction() const { return SpawnFunction; }

    void SetSpawnFunction(FCogEnineSpawnFunction Value) { SpawnFunction = Value; }

    UFUNCTION(Server, Reliable)
    void Server_Spawn(const FCogEngineSpawnEntry& SpawnEntry);

    float GetTimeDilation() const { return TimeDilation; }

    UFUNCTION(Server, Reliable)
    void Server_SetTimeDilation(float Value);

protected:

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
