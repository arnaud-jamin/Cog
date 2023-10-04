#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_NetEmulation.generated.h"

UCLASS()
class COGENGINE_API UCogEngineWindow_NetEmulation : public UCogWindow
{
    GENERATED_BODY()

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void DrawStats();

    virtual void DrawControls();

private:

};
