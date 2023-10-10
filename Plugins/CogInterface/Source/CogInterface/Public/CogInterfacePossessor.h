#pragma once

#include "CoreMinimal.h"
#include "CogInterfacePossessor.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UCogInterfacePossessor : public UInterface
{
    GENERATED_BODY()
};

//--------------------------------------------------------------------------------------------------------------------------
class ICogInterfacePossessor
{
    GENERATED_BODY()

public:

    virtual void SetPossession(APawn* Pawn) = 0;

    virtual void ResetPossession() = 0;
};