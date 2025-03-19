#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CogDebugPluginSubsystem.generated.h"

UCLASS(Abstract)
class COGDEBUG_API UCogDebugPluginSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    virtual void OnPlayerControllerReady(APlayerController* InController) {}
};
