#pragma once

#include "CoreMinimal.h"
#include "CogInputDataAsset_Actions.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS(Blueprintable)
class COGINPUT_API UCogInputDataAsset_Actions : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UCogInputDataAsset_Actions() {}

    UPROPERTY(EditAnywhere)
    TObjectPtr<const UInputMappingContext> MappingContext;
};
