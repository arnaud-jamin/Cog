#pragma once

#include "CoreMinimal.h"
#include "CogCommon.h"

#if ENABLE_COG

#include "CogWindow.h"

//--------------------------------------------------------------------------------------------------------------------------
class FCogSampleWindow_Team : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;
};

#endif //ENABLE_COG