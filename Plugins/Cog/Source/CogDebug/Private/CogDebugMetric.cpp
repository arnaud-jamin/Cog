#include "CogDebugMetric.h"

#include "CogDebug.h"

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugMetric::MaxDurationSetting = 0.0f;
float FCogDebugMetric::RestartDelaySetting = 5.0f;
bool FCogDebugMetric::IsVisible = false;
TMap<FName, FCogDebugMetricEntry> FCogDebugMetric::Metrics;

//--------------------------------------------------------------------------------------------------------------------------
// FCogDebugMetric
//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugMetric::AddMetric(const FCogDebugMetricParams& Params)
{
    if (FCogDebug::IsDebugActiveForObject(Params.WorldContextObject) == false)
    {
        return;
    }

    FCogDebugMetricEntry& Entry = Metrics.FindOrAdd(Params.Name);
    Entry.Add(Params);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugMetric::AddMetric(const UObject* WorldContextObject, FName Name, float MitigatedValue, float UnmitigatedValue, bool IsCritical)
{
    FCogDebugMetricParams Params;
    Params.WorldContextObject = WorldContextObject;
    Params.Name = Name;
    Params.MitigatedValue = MitigatedValue;
    Params.UnmitigatedValue = UnmitigatedValue;
    Params.IsCritical = IsCritical;
    AddMetric(Params);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugMetric::Tick(float DeltaSeconds)
{
    for (auto& Entry : Metrics)
    {
        FCogDebugMetricEntry& Metric = Entry.Value;
        Metric.Tick(DeltaSeconds);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugMetric::Reset()
{
    for (auto& Entry : Metrics)
    {
        FCogDebugMetricEntry& Metric = Entry.Value;
        Metric.Reset();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// FCogMetricInstance
//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugMetricValue::Reset()
{
    Last = 0.0f;
    Min = 0.0f;
    Max = 0.0f;
    PerSecond = 0.0f;
    PerFrame = 0.0f;
    Total = 0.0f;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugMetricValue::AddMetric(const float Metric)
{
    Last = Metric;
    Min = Min == 0.0f ? Metric : FMath::Min(Min, Metric);
    Max = FMath::Max(Max, Metric);
    PerFrame += Metric;
    Total += Metric;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugMetricValue::UpdateMetricPerSecond(const float Duration)
{
    PerSecond = Duration > 1.0f ? Total / Duration : Total;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugMetricEntry::Reset()
{
    Count = 0;
    Crits = 0;
    TotalCritChances = 0.0f;

    IsInProgress = false;
    Timer = 0.0f;
    RestartTimer = 0.0f;

    Mitigated.Reset();
    Unmitigated.Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugMetricEntry::Add(const FCogDebugMetricParams& Params)
{
    // If the max duration is reached, stop adding
    if (FCogDebugMetric::MaxDurationSetting != 0 && Timer >= FCogDebugMetric::MaxDurationSetting)
    {
        return;
    }

    IsInProgress = true;
    Count++;
    Crits += Params.IsCritical ? 1 : 0;

    Mitigated.AddMetric(Params.MitigatedValue);
    Unmitigated.AddMetric(Params.UnmitigatedValue);
    Mitigated.UpdateMetricPerSecond(Timer);
    Unmitigated.UpdateMetricPerSecond(Timer);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugMetricEntry::Tick(const float DeltaSeconds)
{
    if (IsInProgress)
    {
        // If the max duration is reached, stop increasing time.
        if (FCogDebugMetric::MaxDurationSetting <= 0 || Timer < FCogDebugMetric::MaxDurationSetting)
        {
            Timer += DeltaSeconds;
        }
        else
        {
            IsInProgress = false;
            Timer = FCogDebugMetric::MaxDurationSetting;
            Mitigated.UpdateMetricPerSecond(Timer);
            Unmitigated.UpdateMetricPerSecond(Timer);
        }
    }

    if (FCogDebugMetric::RestartDelaySetting > 0.0f)
    {
        if (Unmitigated.PerFrame == 0.0f)
        {
            RestartTimer += DeltaSeconds;
            if (RestartTimer > FCogDebugMetric::RestartDelaySetting)
            {
                Reset();
            }
        }
        else
        {
            RestartTimer = 0.0f;
        }
    }

    Mitigated.PerFrame = 0.0f;
    Unmitigated.PerFrame = 0.0f;
}