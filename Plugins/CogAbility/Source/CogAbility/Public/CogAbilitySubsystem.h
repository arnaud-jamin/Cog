#pragma once

#include "CoreMinimal.h"
#include "CogAbilityReplicator.h"
#include "CogDebugPluginSubsystem.h"
#include "CogAbilitySubsystem.generated.h"

UCLASS()
class COGABILITY_API UCogAbilitySubsystem : public UCogDebugPluginSubsystem
{
	GENERATED_BODY()

public:

	virtual void OnPlayerControllerReady(APlayerController* InController) override
	{
		if (InController != nullptr)
		{
			ACogAbilityReplicator::Spawn(InController);
		}
	}
};
