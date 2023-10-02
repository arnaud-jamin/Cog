#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CogAbilityDataAsset_Pools.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGABILITY_API FCogAbilityPool
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FString Name;

    UPROPERTY(EditAnywhere)
    FGameplayAttribute Value;

    UPROPERTY(EditAnywhere)
    FGameplayAttribute Min;

    UPROPERTY(EditAnywhere)
    FGameplayAttribute Max;

    UPROPERTY(EditAnywhere)
    FGameplayAttribute Regen;

    UPROPERTY(EditAnywhere)
    FLinearColor Color = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

    UPROPERTY(EditAnywhere)
    FLinearColor BackColor = FLinearColor(0.15, 0.15, 0.15, 1.0f);
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Blueprintable)
class COGABILITY_API UCogAbilityDataAsset_Pools : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    
    UCogAbilityDataAsset_Pools() {}

    UPROPERTY(EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityPool> Pools;
};
