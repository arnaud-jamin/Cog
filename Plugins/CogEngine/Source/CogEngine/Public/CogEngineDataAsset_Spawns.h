#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CogEngineDataAsset_Spawns.generated.h"

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
class COGENGINE_API UCogEngineDataAsset_Spawns : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    
    UPROPERTY(EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogEngineSpawnGroup> SpawnGroups;
};
