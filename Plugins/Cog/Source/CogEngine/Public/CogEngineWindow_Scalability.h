#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"

class COGENGINE_API FCogEngineWindow_Scalability : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;
};
