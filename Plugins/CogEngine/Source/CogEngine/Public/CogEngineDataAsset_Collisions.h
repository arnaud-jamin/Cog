#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CogEngineDataAsset_Collisions.generated.h"

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
UCLASS(Blueprintable)
class COGENGINE_API UCogEngineDataAsset_Collisions : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    
    UCogEngineDataAsset_Collisions() {}

    UPROPERTY(EditAnywhere, meta = (TitleProperty = "Channel"))
    TArray<FCogCollisionChannel> Channels;
};
