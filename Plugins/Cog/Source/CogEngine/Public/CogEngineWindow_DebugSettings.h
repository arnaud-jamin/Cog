#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogWindowConfig.h"
#include "CogEngineWindow_DebugSettings.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_DebugSettings : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void ResetConfig() override;

    virtual void RenderHelp() override;

    virtual void PreSaveConfig() override;

    virtual void RenderContent() override;

private:

    TWeakObjectPtr<UCogEngineConfig_DebugSettings> Config = nullptr;

};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_DebugSettings : public UCogWindowConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool bIsFilteringBySelection = true;

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

    virtual void Reset() override
    {
        Super::Reset();

        bIsFilteringBySelection = true;
        Persistent = false;
        TextShadow = true;
        Fade2D = true;
        Duration = 3.0f;
        DepthPriority = 0;
        Segments = 12;
        Thickness = 0.0f;
        ServerThickness = 2.0f;
        ServerColorMultiplier = 0.8f;
        ArrowSize = 10.0f;
        AxesScale = 1.0f;
        GradientColorIntensity = 0.0f;
        GradientColorSpeed = 2.0f;
        TextSize = 1.0f;
    }
};