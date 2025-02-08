#pragma once

#include "CoreMinimal.h"
#include "CogEngineReplicator.h"
#include "CogWindow.h"

class UCogEngineDataAsset;
struct FCogEngineSpawnGroup;
struct FCogEngineSpawnEntry;

class COGENGINE_API FCogEngineWindow_Spawns : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void RenderSpawnGroup(ACogEngineReplicator& Replicator, const FCogEngineSpawnGroup& SpawnGroup, int32 GroupIndex);

    virtual bool RenderSpawnAsset(ACogEngineReplicator& Replicator, const FCogEngineSpawnEntry& SpawnEntry, bool IsLastSelected);

private:

    TObjectPtr<const UCogEngineDataAsset> Asset = nullptr;
};
