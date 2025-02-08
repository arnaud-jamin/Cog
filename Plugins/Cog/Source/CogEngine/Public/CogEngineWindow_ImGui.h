#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"

class COGENGINE_API FCogEngineWindow_ImGui : public FCogWindow
{
    typedef FCogWindow Super;

protected:

    virtual void RenderTick(float DeltaTime) override;

    virtual void RenderContent() override;

private:

    bool bShowImguiDemo = false;

    bool bShowImguiPlot = false;

    bool bShowImguiMetric = false;

    bool bShowImguiDebugLog = false;

    bool bShowImguiStyleEditor = false;
};
