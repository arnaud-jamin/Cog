#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"

class FSlateUser;

class COGENGINE_API FCogEngineWindow_Slate : public FCogWindow
{
    typedef FCogWindow Super;

public:

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void RenderUser(FSlateUser& User);

};
