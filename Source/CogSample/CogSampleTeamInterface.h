#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CogSampleTeamInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UCogSampleTeamInterface : public UInterface
{
    GENERATED_BODY()
};

class ICogSampleTeamInterface
{
    GENERATED_BODY()

public:

    virtual int32 GetTeam() const { return 0; }

    virtual void SetTeam(int32) { }
};

