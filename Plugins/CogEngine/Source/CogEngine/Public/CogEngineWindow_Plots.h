#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Plots.generated.h"

UCLASS()
class COGENGINE_API UCogEngineWindow_Plots : public UCogWindow
{
    GENERATED_BODY()

protected:

    virtual void RenderHelp() override;

    virtual void RenderTick(float DeltaTime) override;

    virtual void RenderContent() override;

private:

};
