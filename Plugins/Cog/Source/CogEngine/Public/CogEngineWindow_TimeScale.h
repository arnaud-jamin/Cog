#pragma once

#include "CoreMinimal.h"
#include "CogEngineReplicator.h"
#include "CogInputChord.h"
#include "CogWindow.h"
#include "Framework/Commands/InputChord.h"
#include "CogEngineWindow_TimeScale.generated.h"

class UCogEngineWindowConfig_TimeScale;

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_TimeScale : public FCogWindow
{
    typedef FCogWindow Super;

public:
    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;
    
    virtual void RenderContextMenu() override;

    virtual void RenderMainMenuWidget() override;

    virtual void RenderTimeScaleChoices(ACogEngineReplicator* Replicator);

    virtual int32 GetCurrentTimeScaleIndex(const ACogEngineReplicator& Replicator) const;

    virtual void SetCurrentTimeScaleIndex(ACogEngineReplicator& Replicator, int32 InTimeScaleIndex) const;

    virtual void FasterTimeScale();

    virtual void SlowerTimeScale();
    
    virtual void ResetTimeScale();

    virtual void ZeroTimeScale();

    virtual float GetTimeDilation() const;

    virtual int32 GetTimeScaleIndex(float InTimeScale) const;

    virtual void SetCurrentTimeScale(ACogEngineReplicator& Replicator, float Value) const;

    TWeakObjectPtr<UCogEngineWindowConfig_TimeScale> Config;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineWindowConfig_TimeScale : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    virtual void Reset() override
    {
        UCogCommonConfig::Reset();

        TimeScale = 1.0f;
        Inline = true;
        TimeScales = { 0.00f, 0.01f, 0.10f, 0.50f, 1.00f, 2.00f, 5.00f, 10.0f };
        TimeScaleModifiedColor = FColor(255, 30, 210, 255);

        Shortcut_ZeroTimeScale = FCogInputChord(EKeys::NumPadZero);
        Shortcut_ResetTimeScale = FCogInputChord(EKeys::NumPadOne);
        Shortcut_FasterTimeScale = FCogInputChord(EKeys::Add);
        Shortcut_SlowerTimeScale = FCogInputChord(EKeys::Subtract);
    }
    
    UPROPERTY(Config)
    float TimeScale = 1.0f;
    
    UPROPERTY(Config)
    TArray<float> TimeScales = { 0.00f, 0.01f, 0.10f, 0.50f, 1.00f, 2.00f, 5.00f, 10.0f };

    UPROPERTY(Config)
    bool Inline = true;

    UPROPERTY(Config)
    FColor TimeScaleModifiedColor = FColor(255, 30, 210, 255);

    UPROPERTY(Config)
    FCogInputChord Shortcut_ZeroTimeScale = FCogInputChord(EKeys::NumPadZero);
    
    UPROPERTY(Config)
    FCogInputChord Shortcut_ResetTimeScale = FCogInputChord(EKeys::NumPadOne);

    UPROPERTY(Config)
    FCogInputChord Shortcut_FasterTimeScale = FCogInputChord(EKeys::Add);

    UPROPERTY(Config)
    FCogInputChord Shortcut_SlowerTimeScale = FCogInputChord(EKeys::Subtract);
};