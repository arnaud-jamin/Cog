#pragma once

#include "CoreMinimal.h"
#include "CogCommonAllegianceActorInterface.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogCommonAllegiance : uint8
{
    Friendly,
    Enemy,
    Neutral
};

//--------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UCogCommonAllegianceActorInterface : public UInterface
{
    GENERATED_BODY()
};

//--------------------------------------------------------------------------------------------------------------------------
class ICogCommonAllegianceActorInterface
{
    GENERATED_BODY()

public:

    virtual ECogCommonAllegiance GetAllegianceWithOtherActor(const AActor* OtherActor) const = 0;
};