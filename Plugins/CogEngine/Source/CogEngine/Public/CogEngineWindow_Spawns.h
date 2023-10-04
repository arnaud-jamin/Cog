#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Spawns.generated.h"

class UCogEngineDataAsset_Spawns;
struct FCogEngineSpawnGroup;

UCLASS()
class COGENGINE_API UCogEngineWindow_Spawns : public UCogWindow
{
    GENERATED_BODY()

 public:

    const UCogEngineDataAsset_Spawns* GetSpawnsAsset() const { return SpawnAsset; }

    void SetSpawnsAsset(const UCogEngineDataAsset_Spawns* Value) { SpawnAsset = Value; }

protected:

    virtual void RenderHelp();

    virtual void PreRender(ImGuiWindowFlags& WindowFlags) override;

    virtual void RenderContent() override;

    virtual void RenderSpawnGroup(const FCogEngineSpawnGroup& SpawnGroup);

    virtual bool RenderSpawnAsset(const FCogEngineSpawnEntry& SpawnEntry, bool IsLastSelected);

private:

    const UCogEngineDataAsset_Spawns* SpawnAsset = nullptr;
};
