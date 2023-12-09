#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CogInputDataAsset.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS(Blueprintable)
class COGINPUT_API UCogInputDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UCogInputDataAsset() {}

    UPROPERTY(EditAnywhere)
    TArray<TObjectPtr<const UInputMappingContext>> MappingContexts;
};
