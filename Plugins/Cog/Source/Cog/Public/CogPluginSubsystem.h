#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CogPluginSubsystem.generated.h"

UCLASS(Abstract)
class COG_API UCogPluginSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    virtual void OnPlayerControllerSet(APlayerController* InController) {}
};
