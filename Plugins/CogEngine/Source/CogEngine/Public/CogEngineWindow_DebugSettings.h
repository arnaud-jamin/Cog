#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_DebugSettings.generated.h"

UCLASS(Config = Cog)
class COGENGINE_API UCogEngineWindow_DebugSettings : public UCogWindow
{
    GENERATED_BODY()

public:

    UCogEngineWindow_DebugSettings();

protected:

    virtual void ResetConfig() override;

    virtual void RenderHelp() override;

    virtual void PreSaveConfig() override;

    virtual void PostInitProperties() override;

    virtual void RenderContent() override;

private:

    UPROPERTY(Config)
    bool FilterBySelection = true;

    UPROPERTY(Config)
    bool Persistent = false;

    UPROPERTY(Config)
    bool TextShadow = true;

    UPROPERTY(Config)
    bool Fade2D = true;

    UPROPERTY(Config)
    float Duration = 3.0f;

    UPROPERTY(Config)
    int DepthPriority = 0;

    UPROPERTY(Config)
    int Segments = 12;

    UPROPERTY(Config)
    float Thickness = 0.0f;

    UPROPERTY(Config)
    float ServerThickness = 2.0f;

    UPROPERTY(Config)
    float ServerColorMultiplier = 0.8f;

    UPROPERTY(Config)
    float ArrowSize = 10.0f;

    UPROPERTY(Config)
    float AxesScale = 1.0f;

    UPROPERTY(Config)
    float GradientColorIntensity = 0.0f;

    UPROPERTY(Config)
    float GradientColorSpeed = 2.0f;

    UPROPERTY(Config)
    float TextSize = 1.0f;
};
