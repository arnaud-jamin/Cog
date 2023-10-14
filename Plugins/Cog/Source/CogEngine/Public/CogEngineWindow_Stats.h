#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Stats.generated.h"

UCLASS()
class COGENGINE_API UCogEngineWindow_Stats : public UCogWindow
{
    GENERATED_BODY()

public:

    static ImVec4 GetFpsColor(float Value, float Good = 50.0f, float Medium = 30.0f);

    static ImVec4 GetPingColor(float Value, float Good = 100.0f, float Medium = 200.0f);

    static ImVec4 GetPacketLossColor(float Value, float Good = 10.0f, float Medium = 20.0f);

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void RenderMainMenuWidget(bool Draw, float& Width) override;
};
