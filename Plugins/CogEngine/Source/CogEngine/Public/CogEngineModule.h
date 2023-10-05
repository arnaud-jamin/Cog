#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ACogEngineReplicator;
class APlayerController;

class COGENGINE_API FCogEngineModule : public IModuleInterface
{
public:

    static inline FCogEngineModule& Get() { return FModuleManager::LoadModuleChecked<FCogEngineModule>("CogEngine"); }

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

    ACogEngineReplicator* GetLocalReplicator();

    void SetLocalReplicator(ACogEngineReplicator* Value);

    TArray<TObjectPtr<ACogEngineReplicator>> GetRemoteReplicators() const { return RemoteReplicators; }

    void AddRemoteReplicator(ACogEngineReplicator* Value);

private:
    TObjectPtr<ACogEngineReplicator> LocalReplicator;
    TArray<TObjectPtr<ACogEngineReplicator>> RemoteReplicators;
};
