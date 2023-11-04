#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"

class COGENGINE_API FCogEngineWindow_Stats : public FCogWindow
{
    typedef FCogWindow Super;

public:

    static ImVec4 GetFpsColor(float Value, float Good = 50.0f, float Medium = 30.0f);

    static ImVec4 GetPingColor(float Value, float Good = 100.0f, float Medium = 200.0f);

    static ImVec4 GetPacketLossColor(float Value, float Good = 10.0f, float Medium = 20.0f);

protected:

    virtual void Initialize() override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual float GetMainMenuWidgetWidth(int32 SubWidgetIndex, float MaxWidth) override;

    virtual void RenderMainMenuWidget(int32 SubWidgetIndex, float Width) override;

    virtual void RenderMainMenuWidgetPacketLoss(float Width);

    virtual void RenderMainMenuWidgetPing(float Width);

    virtual void RenderMainMenuWidgetFramerate(float Width);
};
