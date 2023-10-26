#pragma once

#include "CoreMinimal.h"
#include "CogSampleProgressionLevelInterface.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UCogSampleProgressionLevelInterface : public UInterface
{
    GENERATED_BODY()
};

//--------------------------------------------------------------------------------------------------------------------------
class ICogSampleProgressionLevelInterface
{
    GENERATED_BODY()

public:

    virtual int32 GetProgressionLevel() const { return 0; }

    virtual void SetProgressionLevel(int32) { }
};