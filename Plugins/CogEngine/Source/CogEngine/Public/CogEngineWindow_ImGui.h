#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_ImGui.generated.h"

UCLASS()
class COGENGINE_API UCogEngineWindow_ImGui : public UCogWindow
{
    GENERATED_BODY()

public:
    UCogEngineWindow_ImGui();

    virtual void RenderTick(float DeltaTime) override;
    virtual void RenderContent() override;

private:
    bool bShowImguiDemo = false;
    bool bShowImguiPlot = false;
    bool bShowImguiMetric = false;
    bool bShowImguiDebugLog = false;
    bool bShowImguiStyleEditor = false;
};
