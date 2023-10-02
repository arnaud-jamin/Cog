#pragma once

#include "CoreMinimal.h"
#include "CogDebugFilteredActorInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UCogDebugFilteredActorInterface : public UInterface
{
    GENERATED_BODY()
};

class ICogDebugFilteredActorInterface
{
    GENERATED_BODY()

    virtual bool IsActorFilteringDebug() const { return true; }
};
