#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CogCommonAllegianceActorInterface.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
// Match ETeamAttitude in AIModule
//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogCommonAllegiance : uint8
{
    Friendly,
    Neutral,
    Enemy
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