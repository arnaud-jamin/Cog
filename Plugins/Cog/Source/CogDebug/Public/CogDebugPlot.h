#pragma once

#include "CoreMinimal.h"
#include "CogCommon.h"
#include "imgui.h"

#ifdef ENABLE_COG

struct FCogDebugHistory;

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugPlotEventParams
{
    FName Name;
    FString Value;
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugPlotEvent
{
    float GetActualEndTime(const FCogDebugHistory& Plot) const;

    uint64 GetActualEndFrame(const FCogDebugHistory& Plot) const;

    FCogDebugPlotEvent& AddParam(const FName Name, bool Value);

    FCogDebugPlotEvent& AddParam(const FName Name, int Value);

    FCogDebugPlotEvent& AddParam(const FName Name, float Value);

    FCogDebugPlotEvent& AddParam(const FName Name, FName Value);

    FCogDebugPlotEvent& AddParam(const FName Name, const FString& Value);

    FName Id;
    float StartTime = 0.0f;
    float EndTime = 0.0f;
    uint64 StartFrame = 0;
    uint64 EndFrame = 0;
    ImU32 BorderColor;
    ImU32 FillColor;
    int32 Row;
    FString OwnerName;
    FString DisplayName;
    TArray<FCogDebugPlotEventParams> Params;
};

//--------------------------------------------------------------------------------------------------------------------------
enum class FCogDebugHistoryType
{
    Value,
    Event,
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugHistory
{
    virtual ~FCogDebugHistory() {}

	virtual void Clear() {}

    FName Name;

	float Time = 0;

	uint64 Frame = 0;

    int32 GraphIndex = 0;

    FCogDebugHistoryType Type = FCogDebugHistoryType::Value;

    TWeakObjectPtr<const UWorld> World;
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugValueHistory : FCogDebugHistory
{
    void AddPoint(float X, float Y);

    bool FindValue(float Time, float& Value) const;

    virtual void Clear() override;

    void SetNumRecordedValues(const int32 Value);

    int32 ValueOffset = 0;

	ImVector<ImVec2> Values;

	bool ShowValuesMarkers = false;
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugEventHistory : FCogDebugHistory
{
    FCogDebugPlotEvent& AddEvent(const FString& OwnerName, bool IsInstant, const FName EventId, const int32 Row, const FColor& Color);

    FCogDebugPlotEvent& StopEvent(const FName EventId);

	FCogDebugPlotEvent* GetLastEvent();

    FCogDebugPlotEvent* FindLastEventByName(FName EventId);

    virtual void Clear() override;

    int32 EventOffset = 0;

	TArray<FCogDebugPlotEvent> Events;

	int32 MaxRow = 1;
};

//--------------------------------------------------------------------------------------------------------------------------

class COGDEBUG_API FCogDebugPlot
{
public:
    static constexpr int32 AutoRow = -1;

    static constexpr int32 MaxNumGraphs = 5;

    static constexpr int32 MaxNumEntriesPerGraph = 10;


    static void PlotValue(const UObject* WorldContextObject, const FName PlotName, const float Value);

    static FCogDebugPlotEvent& PlotEvent(const UObject* WorldContextObject, const FName PlotName, const FName EventId, bool IsInstant, const int32 Row = AutoRow, const FColor& Color = FColor::Transparent);

    static FCogDebugPlotEvent& PlotEventInstant(const UObject* WorldContextObject, const FName PlotName, const FName EventId, const int32 Row = AutoRow, const FColor& Color = FColor::Transparent);

    static FCogDebugPlotEvent& PlotEventStart(const UObject* WorldContextObject, const FName PlotName, const FName EventId, const int32 Row = AutoRow, const FColor& Color = FColor::Transparent);

    static FCogDebugPlotEvent& PlotEventStop(const UObject* WorldContextObject, const FName PlotName, const FName EventId);

    static FCogDebugPlotEvent& PlotEventToggle(const UObject* WorldContextObject, const FName PlotName, const FName EventId, const bool ToggleValue, const int32 Row = AutoRow, const FColor& Color = FColor::Transparent);

    static FCogDebugHistory* FindEntry(const FName InName);

    static void SetNumRecordedValues(int32 InValue);
    
    static void Reset();

    static void Clear();

    static TMap<FName, FCogDebugValueHistory> Values;

    static TMap<FName, FCogDebugEventHistory> Events;

    static bool IsVisible;

    static bool Pause;

    static bool RecordValuesWhenPause;


private:
    friend struct FCogDebugHistory;
    friend struct FCogDebugEventHistory;
    friend struct FCogDebugValueHistory;

    static void ResetLastAddedEvent();

    static FCogDebugValueHistory* RegisterValue(const UObject* InWorldContextObject, const FName InName);

    static FCogDebugEventHistory* RegisterEvent(const UObject* InWorldContextObject, const FName InName);

    static bool ShouldRegisterEntry(const UObject* WorldContextObject, const UWorld*& World);

    static void InitializeEntry(FCogDebugHistory& OutValue, const UWorld* InWorld, const FName InName);

    static FCogDebugPlotEvent* GetLastAddedEvent();

    static void OccupyGraphRow(const int32 InGraphIndex, const int32 InRow);

    static void FreeGraphRow(const int32 InGraphIndex, const int32 InRow);

    static int32 FindFreeGraphRow(const int32 InGraphIndex);

    static int32 NumRecordedValues;

    static FName LastAddedEventPlotName;

	static int32 LastAddedEventIndex;

    static FCogDebugPlotEvent DefaultEvent;

    // graph index to row index to number of objects occupying the row
    static TMap<int32, TMap<int32, int32>> OccupationMap;
};

#endif //ENABLE_COG

