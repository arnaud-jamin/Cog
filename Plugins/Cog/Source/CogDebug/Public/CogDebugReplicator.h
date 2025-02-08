#pragma once

#include "CoreMinimal.h"
#include "CogCommonLog.h"
#include "GameFramework/Actor.h"
#include "CogDebugShape.h"
#include "UObject/Class.h"
#include "UObject/ObjectMacros.h"
#include "CogDebugReplicator.generated.h"

class ACogDebugReplicator;
class APlayerController;

//--------------------------------------------------------------------------------------------------------------------------

USTRUCT()
struct FCogServerCategoryData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    FName LogCategoryName;

    UPROPERTY()
    ECogLogVerbosity Verbosity = ECogLogVerbosity::Fatal;
};

//--------------------------------------------------------------------------------------------------------------------------

USTRUCT()
struct FCogReplicatorNetPack
{
    GENERATED_USTRUCT_BODY()

    TObjectPtr<ACogDebugReplicator> Owner;

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);

private:

    TArray<FCogDebugShape> SavedShapes;
};

//--------------------------------------------------------------------------------------------------------------------------
template<>
struct TStructOpsTypeTraits<FCogReplicatorNetPack> : public TStructOpsTypeTraitsBase2<FCogReplicatorNetPack>
{
    enum
    {
        WithNetDeltaSerializer = true,
    };
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(NotBlueprintable, NotBlueprintType, notplaceable, noteditinlinenew, hidedropdown, Transient)
class COGDEBUG_API ACogDebugReplicator : public AActor
{
    GENERATED_UCLASS_BODY()

public:
    
    static ACogDebugReplicator* Spawn(APlayerController* Controller);

    static ACogDebugReplicator* GetLocalReplicator(const UWorld& World);

    static void GetRemoteReplicators(const UWorld& World, TArray<ACogDebugReplicator*>& Replicators);

    virtual void BeginPlay() override;
    
    virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

    APlayerController* GetPlayerController() const { return OwnerPlayerController.Get(); }

    TArray<FCogDebugShape> ReplicatedShapes;

    UFUNCTION(Server, Reliable)
    void Server_RequestAllCategoriesVerbosity();

    UFUNCTION(Server, Reliable)
    void Server_SetCategoryVerbosity(FName LogCategoryName, ECogLogVerbosity Verbosity);

    UFUNCTION(NetMulticast, Reliable)
    void NetMulticast_SendCategoriesVerbosity(const TArray<FCogServerCategoryData>& Categories);
    
    UFUNCTION(Client, Reliable)
    void Client_SendCategoriesVerbosity(const TArray<FCogServerCategoryData>& Categories);

    AActor* GetServerSelection() const { return ServerSelection.Get(); }
    
    UFUNCTION(Server, Reliable)
    void Server_SetSelection(AActor* Value, bool ForceSelection);

    bool IsServerFilteringBySelection() const { return bIsServerFilteringBySelection; }
    
    UFUNCTION(Server, Reliable)
    void Server_SetIsFilteringBySelection(bool Value);

protected:
    friend FCogReplicatorNetPack;

    TWeakObjectPtr<APlayerController> OwnerPlayerController;

    uint32 bHasAuthority : 1;

    UPROPERTY(Replicated)
    FCogReplicatorNetPack ReplicatedData;

    TWeakObjectPtr<AActor> ServerSelection = nullptr;

    bool bIsServerFilteringBySelection = true;
};
