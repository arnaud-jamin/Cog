#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ACogAbilityReplicator;
class APlayerController;

class COGABILITY_API FCogAbilityModule : public IModuleInterface
{
public:

    static inline FCogAbilityModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FCogAbilityModule>("CogAbility");
    }

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    ACogAbilityReplicator* GetLocalReplicator();
    void SetLocalReplicator(ACogAbilityReplicator* Value);
    ACogAbilityReplicator* GetRemoteReplicator(const APlayerController* PlayerController);
    void AddRemoteReplicator(ACogAbilityReplicator* Value);

private:
    TObjectPtr<ACogAbilityReplicator> LocalReplicator;
    TArray<TObjectPtr<ACogAbilityReplicator>> RemoteReplicators;
};
