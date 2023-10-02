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

    ACogDebugReplicator* GetLocalReplicator();

    void SetLocalReplicator(ACogDebugReplicator* Value);

    ACogDebugReplicator* GetRemoteReplicator(const APlayerController* PlayerController);

    TArray<TObjectPtr<ACogDebugReplicator>> GetRemoteReplicators() const { return RemoteReplicators; }

    void AddRemoteReplicator(ACogDebugReplicator* Value);

private:

    TObjectPtr<ACogDebugReplicator> LocalReplicator;

    TArray<TObjectPtr<ACogDebugReplicator>> RemoteReplicators;
};
