#pragma once

#include "CoreMinimal.h"

#include "CogDebugTrack.h"
#include "CogDebugEvent.h"
#include "CogDebugPlotTrack.h"
#include "CogDebugEventTrack.h"

struct COGDEBUG_API FCogDebugTracker
{
    static constexpr int32 AutoRow = -1;

    static constexpr int32 MaxNumViews = 5;

    static constexpr int32 MaxNumTrackPerView = 10;

    void Plot(const UObject* InWorldContextObject, const FCogDebugTrackId InTrackId, const float Value);

    FCogDebugEvent& InstantEvent(const UObject* InWorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId, const int32 Row = AutoRow, const FColor& Color = FColor::Transparent);

    FCogDebugEvent& StartEvent(const UObject* InWorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId, bool IsInstant, const int32 Row = AutoRow, const FColor& Color = FColor::Transparent);

    FCogDebugEvent& StartEvent(const UObject* InWorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId, const int32 Row = AutoRow, const FColor& Color = FColor::Transparent);

    FCogDebugEvent& StopEvent(const UObject* InWorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId);

    FCogDebugEvent& ToggleEvent(const UObject* InWorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId, const bool ToggleValue, const int32 Row = AutoRow, const FColor& Color = FColor::Transparent);

    FCogDebugTrack* FindTrack(const FCogDebugTrackId& InTrackId);

    void SetNumRecordedValues(int32 InValue);
    
    void Reset();

    void Clear();

    TMap<FCogDebugTrackId, FCogDebugPlotTrack> Values;

    TMap<FCogDebugTrackId, FCogDebugEventTrack> Events;

    bool IsVisible = false;

    bool Pause = false;

    bool RecordValuesWhenPause = true;
    
private:
    friend struct FCogDebugEvent;
    friend struct FCogDebugTrack;
    friend struct FCogDebugEventTrack;
    friend struct FCogDebugPlotTrack;

    void ResetLastAddedEvent();

    FCogDebugPlotTrack* GetOrCreatePlotTrack(const UObject* InWorldContextObject, const FCogDebugTrackId& InTrackId);

    FCogDebugEventTrack* GetOrCreateEventTrack(const UObject* InWorldContextObject, const FCogDebugTrackId& InTrackId);

    bool CanCreateTrack(const UObject* WorldContextObject, const UWorld*& World) const;

    static void InitializeTrack(FCogDebugTrack& OutTrack, const UWorld* InWorld, const FCogDebugTrackId& InTrackId);

    FCogDebugEvent* GetLastAddedEvent();

    void OccupyViewRow(const int32 InViewIndex, const int32 InRow);

    void FreeViewRow(const int32 InViewIndex, const int32 InRow);

    int32 FindFreeViewRow(const int32 InViewIndex);

    static int32 NumRecordedValues;

    static FCogDebugEvent DefaultEvent;

    FCogDebugTrackId LastAddedEventTrackId = NAME_None;

    int32 LastAddedEventIndex = INDEX_NONE;

    // view index to row index to number of objects occupying the row
    TMap<int32, TMap<int32, int32>> OccupationMap;
};
