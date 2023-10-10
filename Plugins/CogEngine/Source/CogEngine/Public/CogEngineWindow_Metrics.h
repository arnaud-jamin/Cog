#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Metrics.generated.h"

struct FCogDebugMetricEntry;

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class COGENGINE_API UCogEngineWindow_Metrics : public UCogWindow
{
    GENERATED_BODY()

public:

    UCogEngineWindow_Metrics();

protected:

    virtual void PostInitProperties() override;

    virtual void PreSaveConfig() override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;
    
    virtual void RenderTick(float DeltaTime) override;

    virtual void DrawMetric(FCogDebugMetricEntry& Metric);

    virtual void DrawMetricRow(const char* Title, float MitigatedValue, float UnmitigatedValue, const ImVec4& Color);

private:

    UPROPERTY(Config)
    float MaxDurationSetting = 0.0f;

    UPROPERTY(Config)
    float RestartDelaySetting = 5.0f;
};
