#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Spawns.generated.h"

class UCogEngineDataAsset;
struct FCogEngineSpawnGroup;

UCLASS()
class COGENGINE_API UCogEngineWindow_Spawns : public UCogWindow
{
    GENERATED_BODY()

public:

    const UCogEngineDataAsset* GetSpawnsAsset() const { return SpawnAsset; }

    void SetAsset(const UCogEngineDataAsset* Value) { SpawnAsset = Value; }

protected:

    virtual void RenderHelp();

    virtual void RenderContent() override;

    virtual void RenderSpawnGroup(const FCogEngineSpawnGroup& SpawnGroup);

    virtual bool RenderSpawnAsset(const FCogEngineSpawnEntry& SpawnEntry, bool IsLastSelected);

private:

    const UCogEngineDataAsset* SpawnAsset = nullptr;
};
