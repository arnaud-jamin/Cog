#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Metrics.generated.h"

struct FCogInterfaceMetricEventParams;

//--------------------------------------------------------------------------------------------------------------------------
class FCogMetricInstance
{
public:
    void Restart();

    void AddMetric(const float Damage);

    void UpdateMetricPerSecond(const float Duration);

    float Last = 0.0f;

    float Min = 0.0f;

    float Max = 0.0f;

    float PerFrame = 0.0f;

    float PerSecond = 0.0f;

    float Total = 0.0f;
};

//--------------------------------------------------------------------------------------------------------------------------
class FCogMetricInfo
{
public:
    void AddEvent(const FCogInterfaceMetricEventParams& Params);
    
    void Tick(const float DeltaSeconds);

    void Restart();
    
    FName Name;

    int Count = 0;
    
    int Crits = 0;
    
    bool IsInProgress = false;
    
    float TotalCritChances = 0.0f;
    
    float Timer = 0.0f;
    
    float RestartTimer = 0.0f;

    FCogMetricInstance Mitigated;
    
    FCogMetricInstance Unmitigated;

    float MaxDurationSetting = 0.0f;

    float RestartDelaySetting = 0.0f;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class COGABILITY_API UCogAbilityWindow_Metrics : public UCogWindow
{
    GENERATED_BODY()

public:

protected:
    
    virtual void RenderContent() override;
    
    virtual void GameTick(float DeltaSeconds) override;

    virtual void OnSelectionChanged(AActor* OldSelection, AActor* NewSelection) override;

    virtual void DrawMetrics(FCogMetricInfo& Metric);

    virtual void DrawMetricRow(const char* Title, float MitigatedValue, float UnmitigatedValue, const ImVec4& Color);

private:

    UFUNCTION()
    void OnEvent(const FCogInterfaceMetricEventParams& Params);

    UPROPERTY(Config)
    float MaxDurationSetting = 0.0f;

    UPROPERTY(Config)
    float RestartDelaySetting = 5.0f;

    TMap<FName, FCogMetricInfo> Metrics;

    FDelegateHandle OnMetricDelegate;
};
