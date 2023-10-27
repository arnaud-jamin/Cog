#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"

class COGENGINE_API FCogEngineWindow_TimeScale : public FCogWindow
{
    typedef FCogWindow Super;

public:
    
    void Initialize();

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    TArray<float> TimingScales;

private:

};
