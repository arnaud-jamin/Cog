#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Spawns.generated.h"

class UCogEngineDataAsset;
struct FCogEngineSpawnGroup;
struct FCogEngineSpawnEntry;

UCLASS()
class COGENGINE_API UCogEngineWindow_Spawns : public UCogWindow
{
    GENERATED_BODY()

public:

    const UCogEngineDataAsset* GetAsset() const { return Asset.Get(); }

    void SetAsset(const UCogEngineDataAsset* Value) { Asset = Value; }

protected:

    virtual void RenderHelp();

    virtual void RenderContent() override;

    virtual void RenderSpawnGroup(const FCogEngineSpawnGroup& SpawnGroup);

    virtual bool RenderSpawnAsset(const FCogEngineSpawnEntry& SpawnEntry, bool IsLastSelected);

private:

    UPROPERTY()
    TWeakObjectPtr<const UCogEngineDataAsset> Asset = nullptr;
};
