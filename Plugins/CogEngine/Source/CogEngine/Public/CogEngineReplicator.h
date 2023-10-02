#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/Class.h"
#include "UObject/ObjectMacros.h"
#include "CogEngineReplicator.generated.h"

class APlayerController;

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogEngineReplicatorSpawnAction
{
    GENERATED_USTRUCT_BODY()

    bool Control = false;
    bool Select = false;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(NotBlueprintable, NotBlueprintType, notplaceable, noteditinlinenew, hidedropdown, Transient)
class COGENGINE_API ACogEngineReplicator : public AActor
{
    GENERATED_UCLASS_BODY()

public:
    static void Create(APlayerController* Controller);

    virtual void BeginPlay() override;

    APlayerController* GetPlayerController() const { return OwnerPlayerController.Get(); }

    bool IsLocal() const { return bIsLocal; }

    UFUNCTION(Server, Reliable)
    void Server_SetTimeDilation(float Value);

    float GetTimeDilation() const { return TimeDilation; }

protected:

    void TickSpawnActions();

    UFUNCTION()
    void OnRep_TimeDilation();

    UPROPERTY()
    TArray<FCogEngineReplicatorSpawnAction> SpawnActions;

    TObjectPtr<APlayerController> OwnerPlayerController;

    uint32 bHasAuthority : 1;
    uint32 bIsLocal : 1;

private:
    UPROPERTY(ReplicatedUsing = OnRep_TimeDilation)
    float TimeDilation = 1.0f;

};
