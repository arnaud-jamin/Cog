#pragma once

#include "CoreMinimal.h"
#include "CogInterfacesAllegiance.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogInterfacesAllegiance : uint8
{
    Ally,
    Enemy
};

//--------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UCogInterfacesAllegianceActor : public UInterface
{
    GENERATED_BODY()
};

//--------------------------------------------------------------------------------------------------------------------------
class ICogInterfacesAllegianceActor
{
    GENERATED_BODY()

public:

    virtual ECogInterfacesAllegiance GetAllegiance(const AActor* OtherActor) const = 0;
};