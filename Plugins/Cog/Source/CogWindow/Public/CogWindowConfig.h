#pragma once

#include "CoreMinimal.h"
#include "CogWindowConfig.generated.h"

UCLASS(Config = Cog)
class COGWINDOW_API UCogWindowConfig : public UObject
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool bHideMenu = false;

    UCogWindowConfig()
    {
        Reset();
    }

    virtual void Reset()
    {
        bHideMenu = true;
    }
};
