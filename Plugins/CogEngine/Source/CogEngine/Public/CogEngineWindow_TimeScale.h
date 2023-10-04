#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_TimeScale.generated.h"

UCLASS()
class COGENGINE_API UCogEngineWindow_TimeScale : public UCogWindow
{
    GENERATED_BODY()

public:
    
    UCogEngineWindow_TimeScale();

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void DrawTimeButton(ACogEngineReplicator* Replicator, float Value);

    TArray<float> TimingScales;

private:

};
