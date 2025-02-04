#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.generated.h"

UCLASS(Config = Cog)
class COGCOMMON_API UCogCommonConfig : public UObject
{
    GENERATED_BODY()

public:

    UCogCommonConfig()
    {
    }

    virtual void Reset()
    {
    }
};
