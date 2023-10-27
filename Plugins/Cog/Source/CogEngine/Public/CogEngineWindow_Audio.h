#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"

class COGENGINE_API FCogEngineWindow_Audio : public FCogWindow
{
    typedef FCogWindow Super;

public:

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;
};
