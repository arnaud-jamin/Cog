#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CogSampleGameMode.generated.h"

UCLASS(minimalapi)
class ACogSampleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACogSampleGameMode();
    void BeginPlay();
};



