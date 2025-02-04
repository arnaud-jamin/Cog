#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "CogEngineWindow_Metrics.generated.h"

struct FCogDebugMetricEntry;
class UCogEngineConfig_Metrics;

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_Metrics : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void PreSaveConfig() override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;
    
    virtual void RenderTick(float DeltaTime) override;

    virtual void DrawMetric(FCogDebugMetricEntry& Metric);

    virtual void DrawMetricRow(const char* RowTitle, float MitigatedValue, float UnmitigatedValue, const ImVec4& Color);

private:

    TObjectPtr<UCogEngineConfig_Metrics> Config = nullptr;

};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_Metrics : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    float MaxDurationSetting = 0.0f;

    UPROPERTY(Config)
    float RestartDelaySetting = 5.0f;

    virtual void Reset() override
    {
        Super::Reset();

        MaxDurationSetting = 0.0f;
        RestartDelaySetting = 5.0f;
    }
};