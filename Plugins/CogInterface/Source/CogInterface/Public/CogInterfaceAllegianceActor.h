#pragma once

#include "CoreMinimal.h"
#include "CogInterfaceAllegianceActor.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogInterfacesAllegiance : uint8
{
    Friendly,
    Enemy,
    Neutral
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

    virtual ECogInterfacesAllegiance GetAllegianceWithOtherActor(const AActor* OtherActor) const = 0;
};