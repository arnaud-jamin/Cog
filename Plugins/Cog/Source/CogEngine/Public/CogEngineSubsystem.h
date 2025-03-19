#pragma once

#include "CoreMinimal.h"
#include "CogEngineReplicator.h"
#include "CogDebugPluginSubsystem.h"
#include "CogEngineSubsystem.generated.h"

UCLASS()
class COGENGINE_API UCogEngineSubsystem : public UCogDebugPluginSubsystem
{
	GENERATED_BODY()

public:

	virtual void OnPlayerControllerReady(APlayerController* InController) override
	{
		if (InController != nullptr)
		{
			ACogEngineReplicator::Spawn(InController);
		}
	}
};
