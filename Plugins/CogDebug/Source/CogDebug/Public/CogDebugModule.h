#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ACogDebugReplicator;
class APlayerController;

class COGDEBUG_API FCogDebugModule : public IModuleInterface
{
public:

    static inline FCogDebugModule& Get() { return FModuleManager::LoadModuleChecked<FCogDebugModule>("CogDebug"); }

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

    ACogDebugReplicator* GetLocalReplicator(UWorld& World);

    void GetRemoteReplicators(UWorld& World, TArray<ACogDebugReplicator*>& Replicators);

private:

};
