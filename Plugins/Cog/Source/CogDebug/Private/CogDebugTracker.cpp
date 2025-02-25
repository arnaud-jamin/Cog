#include "CogDebugTracker.h"

#include "CogDebug.h"
#include "CogDebugEventTrack.h"
#include "CogDebugPlotTrack.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

int32 FCogDebugTracker::NumRecordedValues = 2000;
FCogDebugEvent FCogDebugTracker::DefaultEvent;

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugTracker::Reset()
{
    Values.Empty();
    Events.Empty();
    OccupationMap.Empty();
    Pause = false;
    ResetLastAddedEvent();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugTracker::Clear()
{
    for (auto& kv : Values)
    {
        kv.Value.Clear();
    }

    for (auto& kv : Events)
    {
        kv.Value.Clear();
    }

    OccupationMap.Empty();
    ResetLastAddedEvent();
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugTracker::CanCreateTrack(const UObject* WorldContextObject, const UWorld*& World) const
{
    //----------------------------------------------------------
    // When not visible, we don't go further for performances.
    //----------------------------------------------------------
    if (IsVisible == false)
    {
        return false;
    }

    World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
    if (World == nullptr)
    {
        return false;
    }

    if (FCogDebug::IsDebugActiveForObject(WorldContextObject) == false)
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugTracker::InitializeTrack(FCogDebugTrack& OutTrack, const UWorld* InWorld, const FCogDebugTrackId& InTrackId)
{
    const float Time = InWorld->GetTimeSeconds();
    if (Time < OutTrack.Time)
    {
        OutTrack.Clear();
    }

    OutTrack.Id = InTrackId;
    OutTrack.Time = InWorld->GetTimeSeconds();
    OutTrack.Frame = GFrameCounter;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugTracker::OccupyViewRow(const int32 InViewIndex, const int32 InRow)
{
    TMap<int32, int32>& GraphOccupation = OccupationMap.FindOrAdd(InViewIndex);

    if (int32* RowOccupation = GraphOccupation.Find(InRow))
    {
        (*RowOccupation)++;
    }
    else
    {
        GraphOccupation.Add(InRow, 1);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugTracker::FreeViewRow(const int32 InViewIndex, const int32 Row)
{
    TMap<int32, int32>* GraphOccupation = OccupationMap.Find(InViewIndex);
    if (GraphOccupation == nullptr)
    { return; }

    int32* RowOccupation = GraphOccupation->Find(Row);
    if (RowOccupation == nullptr)
    { return; }

    (*RowOccupation)--;
}

//--------------------------------------------------------------------------------------------------------------------------
int32 FCogDebugTracker::FindFreeViewRow(const int32 InViewIndex)
{
    constexpr int32 MaxRows = 100;

    int32 FreeRow = 0;

    TMap<int32, int32>* GraphOccupation = OccupationMap.Find(InViewIndex);
    if (GraphOccupation == nullptr)
    {
        return FreeRow;
    }

    for (; FreeRow < MaxRows; ++FreeRow)
    {
        const int32* Occupation = GraphOccupation->Find(FreeRow);
        if (Occupation == nullptr || *Occupation == 0)
        {
            break;
        }
    }

    return FreeRow;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugTrack* FCogDebugTracker::FindTrack(const FCogDebugTrackId& InTrackId)
{
    FCogDebugTrack* Track = Events.Find(InTrackId);
    if (Track != nullptr)
    {
        return Track;
    }

    Track = Values.Find(InTrackId);
    if (Track != nullptr)
    {
        return Track;
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugTracker::SetNumRecordedValues(const int32 InValue)
{
    NumRecordedValues = InValue;

    for (auto& kv : Values)
    {
        kv.Value.SetNumPlots(InValue);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlotTrack::SetNumPlots(const int32 Value)
{
    Values.reserve(Value);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlotTrack::Plot(float X, float Y)
{
    if (Values.Capacity == 0)
    {
        Values.reserve(FCogDebugTracker::NumRecordedValues);
    }

    if (Values.size() < Values.Capacity)
    {
        Values.push_back(ImVec2(X, Y));
    }
    else
    {
        Values[ValueOffset] = ImVec2(X, Y);
        ValueOffset = (ValueOffset + 1) % Values.size();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlotTrack::Clear()
{
    FCogDebugTrack::Clear();

    if (Values.size() > 0)
    {
        Values.shrink(0);
        ValueOffset = 0;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugPlotTrack::FindValue(float InX, float& OutY) const
{
    OutY = 0.0f;

    bool FoundAfter = false;
    bool FoundBefore = false;

    for (int32 i = Values.size() - 1; i >= 0; --i)
    {
        //--------------------------------------------------
        // The array cycle so we must offset the index
        //--------------------------------------------------
        int32 Index = i;
        if (ValueOffset != 0)
        {
            Index = (i + ValueOffset) % Values.size();
        }

        const ImVec2 Point = Values[Index];
        if (Point.x > InX)
        {
            FoundAfter = true;
        }

        if (Point.x < InX)
        {
            FoundBefore = true;
        }

        if (FoundAfter && FoundBefore)
        {
            OutY = Point.y;
            return true;
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// Plot Track Creation
//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotTrack* FCogDebugTracker::GetOrCreatePlotTrack(const UObject* InWorldContextObject, const FCogDebugTrackId& InTrackId)
{
    const UWorld* World;
    if (CanCreateTrack(InWorldContextObject, World) == false) 
    {
        return nullptr;
    }

    FCogDebugPlotTrack& Track = Values.FindOrAdd(InTrackId);
    Track.Owner = this; 
    Track.Type = ECogDebugTrackType::Value;

    InitializeTrack(Track, World, InTrackId);

    return &Track;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugTracker::Plot(const UObject* InWorldContextObject, const FName InTrackId, const float Value)
{
    if (Pause && RecordValuesWhenPause == false)
    { return; }
    
    FCogDebugPlotTrack* Track = GetOrCreatePlotTrack(InWorldContextObject, InTrackId);
    if (Track == nullptr)
    { return; }

    Track->Plot(Track->Time, Value);
}

//--------------------------------------------------------------------------------------------------------------------------
// Event Track Creation
//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent& FCogDebugTracker::StartEvent(const UObject* InWorldContextObject, const FCogDebugEventId& InTrackId, const FCogDebugEventId& InEventId, bool IsInstant, const int32 Row, const FColor& Color)
{
    FCogDebugEventTrack* Track = GetOrCreateEventTrack(InWorldContextObject, InTrackId);
    if (Track == nullptr)
    {
        ResetLastAddedEvent();
        return DefaultEvent;
    }

    FCogDebugEvent& Event = Track->AddEvent(GetNameSafe(InWorldContextObject), IsInstant, InEventId, Row, Color);
    return Event;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent& FCogDebugTracker::InstantEvent(const UObject* InWorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId, const int32 Row, const FColor& Color)
{
    FCogDebugEvent& Event = StartEvent(InWorldContextObject, InTrackId, InEventId, true, Row, Color);
    return Event;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent& FCogDebugTracker::StartEvent(const UObject* InWorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId, const int32 Row, const FColor& Color)
{
    FCogDebugEvent& Event = StartEvent(InWorldContextObject, InTrackId, InEventId, false, Row, Color);
    return Event;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent& FCogDebugTracker::StopEvent(const UObject* InWorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId)
{
    FCogDebugEventTrack* EventHistory = GetOrCreateEventTrack(InWorldContextObject, InTrackId);
    if (EventHistory == nullptr)
    {
        return DefaultEvent;
    }

    FCogDebugEvent& Event = EventHistory->StopEvent(InEventId);
    return Event;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent& FCogDebugTracker::ToggleEvent(const UObject* InWorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId, const bool ToggleValue, const int32 Row, const FColor& Color)
{
    if (ToggleValue)
    {
        return StartEvent(InWorldContextObject, InTrackId, InEventId, Row, Color);
    }
    else
    {
        return StopEvent(InWorldContextObject, InTrackId, InEventId);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEventTrack* FCogDebugTracker::GetOrCreateEventTrack(const UObject* InWorldContextObject, const FCogDebugEventId& InTrackId)
{
    const UWorld* World;
    if (CanCreateTrack(InWorldContextObject, World) == false)
    { return nullptr; }

    FCogDebugEventTrack& Track = Events.FindOrAdd(InTrackId);
    Track.Type = ECogDebugTrackType::Event;
    Track.Owner = this;
    
    InitializeTrack(Track, World, InTrackId);

    return &Track;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugTracker::ResetLastAddedEvent()
{
    LastAddedEventTrackId = NAME_None;
    LastAddedEventIndex = INDEX_NONE;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent* FCogDebugTracker::GetLastAddedEvent()
{
    FCogDebugEventTrack* EventHistory = Events.Find(LastAddedEventTrackId);
    if (EventHistory == nullptr)
    {
        return nullptr;
    }

    return EventHistory->GetLastEvent();
}
