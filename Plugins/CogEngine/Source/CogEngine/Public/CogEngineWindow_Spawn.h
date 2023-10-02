#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Spawn.generated.h"

class UCogEngineDataAsset_Spawns;
struct FCogEngineSpawnGroup;

UCLASS()
class COGENGINE_API UCogEngineWindow_Spawn : public UCogWindow
{
    GENERATED_BODY()

 public:

    const UCogEngineDataAsset_Spawns* GetSpawnsAsset() const { return Asset; }

    void SetSpawnsAsset(const UCogEngineDataAsset_Spawns* Value) { Asset = Value; }

protected:

    virtual void PreRender(ImGuiWindowFlags& WindowFlags) override;

    virtual void RenderContent() override;

    virtual void RenderSpawnGroup(const FCogEngineSpawnGroup& SpawnGroup);

    virtual bool RenderSpawnAsset(const FCogEngineSpawnEntry& SpawnEntry, bool IsLastSelected);

private:

    const UCogEngineDataAsset_Spawns* Asset = nullptr;
};
