#pragma once

#include "CoreMinimal.h"
#include "CogDebugAllegianceInterface.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogAllegiance : uint8
{
    Ally,
    Enemy
};

//--------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UCogAllegianceInterface : public UInterface
{
    GENERATED_BODY()
};

//--------------------------------------------------------------------------------------------------------------------------
class ICogAllegianceInterface
{
    GENERATED_BODY()

public:

    virtual ECogAllegiance GetAllegiance(const AActor* OtherActor) const = 0;
};