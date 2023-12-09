#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "CogEngineDataAsset.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGENGINE_API FCogCollisionChannel
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TEnumAsByte<ECollisionChannel> Channel = ECollisionChannel::ECC_WorldStatic;

    UPROPERTY(EditAnywhere)
    FLinearColor Color = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGENGINE_API FCogEngineSpawnEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> Class;

    UPROPERTY(EditAnywhere)
    TObjectPtr<const UObject> Asset = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGENGINE_API FCogEngineSpawnGroup
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FString Name;

    UPROPERTY(EditAnywhere)
    FLinearColor Color = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

    UPROPERTY(EditAnywhere)
    TArray<FCogEngineSpawnEntry> Spawns;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Blueprintable)
class COGENGINE_API UCogEngineDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    
    UCogEngineDataAsset() {}

    UPROPERTY(Category = "Spawns", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogEngineSpawnGroup> SpawnGroups;

    UPROPERTY(Category = "Collisions", EditAnywhere, meta = (TitleProperty = "Channel"))
    TArray<FCogCollisionChannel> Channels;
};
