#pragma once

#include "CoreMinimal.h"
#include "CogSampleSpawnableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UCogSampleSpawnableInterface : public UInterface
{
    GENERATED_BODY()
};

class ICogSampleSpawnableInterface
{
    GENERATED_BODY()

public:

    virtual AActor* GetCreator() const { return nullptr; }

    virtual void SetCreator(AActor* Value) { }
};

