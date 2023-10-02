#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Plots.generated.h"

class UCogEngineDataAsset_Collisions;

UCLASS()
class COGENGINE_API UCogEngineWindow_Plots : public UCogWindow
{
    GENERATED_BODY()

public:
    UCogEngineWindow_Plots();

    virtual void RenderTick(float DeltaTime) override;

    virtual void RenderContent() override;

private:

};
