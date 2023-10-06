#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Audio.generated.h"

UCLASS()
class COGENGINE_API UCogEngineWindow_Audio : public UCogWindow
{
    GENERATED_BODY()

public:

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;
};
