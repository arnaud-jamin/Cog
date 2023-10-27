#pragma once

#include "CoreMinimal.h"
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

    virtual void RenderHelp();

    virtual void RenderContent() override;

    virtual void RenderSpawnGroup(const FCogEngineSpawnGroup& SpawnGroup);

    virtual bool RenderSpawnAsset(const FCogEngineSpawnEntry& SpawnEntry, bool IsLastSelected);

private:

    TObjectPtr<const UCogEngineDataAsset> Asset = nullptr;
};
