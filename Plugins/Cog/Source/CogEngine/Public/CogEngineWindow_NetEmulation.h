#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"

class COGENGINE_API FCogEngineWindow_NetEmulation : public FCogWindow
{
    typedef FCogWindow Super;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void DrawStats();

    virtual void DrawControls();

private:

};
