#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_DebugSettings.generated.h"

UCLASS()
class COGENGINE_API UCogEngineWindow_DebugSettings : public UCogWindow
{
    GENERATED_BODY()

public:
    UCogEngineWindow_DebugSettings();

    virtual void PreRender(ImGuiWindowFlags& WindowFlags) override;

    virtual void RenderContent() override;
};
