#pragma once

#include "CoreMinimal.h"
#include "CogInterfacesFilteredActor.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UCogInterfacesFilteredActor : public UInterface
{
    GENERATED_BODY()
};

class ICogInterfacesFilteredActor
{
    GENERATED_BODY()

    virtual bool IsActorFilteringDebug() const { return true; }
};
