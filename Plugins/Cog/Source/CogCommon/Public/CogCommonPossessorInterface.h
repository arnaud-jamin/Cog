#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CogCommonPossessorInterface.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UCogCommonPossessorInterface : public UInterface
{
    GENERATED_BODY()
};

//--------------------------------------------------------------------------------------------------------------------------
class ICogCommonPossessorInterface
{
    GENERATED_BODY()

public:

    virtual void SetPossession(APawn* Pawn) = 0;

    virtual void ResetPossession() = 0;
};