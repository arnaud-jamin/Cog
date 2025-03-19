#pragma once

#include "CoreMinimal.h"
#include "CogDebugReplicator.h"
#include "CogDebugPluginSubsystem.h"
#include "CogDebugSubsystem.generated.h"

UCLASS()
class COGDEBUG_API UCogDebugSubsystem : public UCogDebugPluginSubsystem
{
	GENERATED_BODY()

public:

	virtual void OnPlayerControllerReady(APlayerController* InController) override
	{
		if (InController != nullptr)
		{
			ACogDebugReplicator::Spawn(InController);
		}
	}
};
