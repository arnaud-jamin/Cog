#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_TimeScale.generated.h"

class ACogEngineReplicator;

UCLASS()
class COGENGINE_API UCogEngineWindow_TimeScale : public UCogWindow
{
    GENERATED_BODY()

public:
    
    UCogEngineWindow_TimeScale();

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    TArray<float> TimingScales;

private:

};
