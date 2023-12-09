#pragma once

#include "CoreMinimal.h"
#include "CogCommon.h"
#include "UObject/Object.h"
#include "UObject/ObjectPtr.h"

#ifdef ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugMetricParams
{
    TObjectPtr<const UObject> WorldContextObject = nullptr;

    FName Name;

    float MitigatedValue = 0;

    float UnmitigatedValue = 0;

    bool IsCritical = false;
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugMetricValue
{
    void Reset();

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
struct COGDEBUG_API FCogDebugMetricEntry
{
public:

    void Add(const FCogDebugMetricParams& Params);

    void Tick(const float DeltaSeconds);

    void Reset();

    int Count = 0;

    int Crits = 0;

    bool IsInProgress = false;

    float TotalCritChances = 0.0f;

    float Timer = 0.0f;

    float RestartTimer = 0.0f;

    FCogDebugMetricValue Mitigated;

    FCogDebugMetricValue Unmitigated;
};

//--------------------------------------------------------------------------------------------------------------------------
class COGDEBUG_API FCogDebugMetric
{
public:

    static void Tick(float DeltaSeconds);

    static void AddMetric(const FCogDebugMetricParams& Params);

    static void AddMetric(const UObject* WorldContextObject, FName Name, float MitigatedValue, float UnmitigatedValue, bool IsCritical);

    static void Reset();
    
    static bool IsVisible;

    static float MaxDurationSetting;

    static float RestartDelaySetting;

    static TMap<FName, FCogDebugMetricEntry> Metrics;
};

#endif //ENABLE_COG

