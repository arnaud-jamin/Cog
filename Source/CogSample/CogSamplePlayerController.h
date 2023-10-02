#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "CogSamplePlayerController.generated.h"

UCLASS(config=Game)
class ACogSamplePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    
    ACogSamplePlayerController();
    virtual void BeginPlay() override;
    virtual void AcknowledgePossession(APawn* P);

private:


};

