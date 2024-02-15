#pragma once

#include "CoreMinimal.h"
#include "CogSampleTargetableInterface.generated.h"

class UCapsuleComponent;

//--------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UCogSampleTargetableInterface : public UInterface
{
    GENERATED_BODY()
};

class ICogSampleTargetableInterface
{
    GENERATED_BODY()

public:

    virtual FVector GetTargetActorLocation() const { return FVector::ZeroVector; }

    virtual void GetTargetCapsules(TArray<const UCapsuleComponent*>& Capsules) const { }
};
