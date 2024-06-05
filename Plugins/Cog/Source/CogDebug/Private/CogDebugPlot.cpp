#include "CogDebugPlot.h"

#include "CogDebug.h"
#include "CogDebugDraw.h"
#include "CogDebugHelper.h"
#include "CogImguiHelper.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

FCogDebugPlotEvent FCogDebugPlot::DefaultEvent;
TArray<FCogDebugPlotEntry> FCogDebugPlot::Plots;
bool FCogDebugPlot::IsVisible = false;
bool FCogDebugPlot::Pause = false;
FName FCogDebugPlot::LastAddedEventPlotName = NAME_None;
int32 FCogDebugPlot::LastAddedEventIndex = INDEX_NONE;

//--------------------------------------------------------------------------------------------------------------------------
// FCogPlotEvent
//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugPlotEvent::GetActualEndTime(const FCogDebugPlotEntry& Plot) const
{
    const float ActualEndTime = EndTime == 0.0f ? Plot.Time : EndTime;
    return ActualEndTime;
}

//--------------------------------------------------------------------------------------------------------------------------
uint64 FCogDebugPlotEvent::GetActualEndFrame(const FCogDebugPlotEntry& Plot) const
{
    const float ActualEndFame = EndFrame == 0.0f ? Plot.Frame : EndFrame;
    return ActualEndFame;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent& FCogDebugPlotEvent::AddParam(const FName Name, bool Value)
{
    if (FCogDebugPlot::IsVisible)
    {
        AddParam(Name, FString::Printf(TEXT("%s"), Value ? TEXT("True") : TEXT("False")));
    }

    return *this;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent& FCogDebugPlotEvent::AddParam(const FName Name, int Value)
{
    if (FCogDebugPlot::IsVisible)
    {
        AddParam(Name, FString::Printf(TEXT("%d"), Value));
    }

    return *this;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent& FCogDebugPlotEvent::AddParam(const FName Name, float Value)
{
    if (FCogDebugPlot::IsVisible)
    {
        AddParam(Name, FString::Printf(TEXT("%0.2f"), Value));
    }

    return *this;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent& FCogDebugPlotEvent::AddParam(const FName Name, FName Value)
{
    if (FCogDebugPlot::IsVisible)
    {
        AddParam(Name, Value.ToString());
    }

    return *this;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent& FCogDebugPlotEvent::AddParam(const FName Name, const FString& Value)
{
    if (FCogDebugPlot::IsVisible)
    {

        if (Name == "Name")
        {
            DisplayName = Value;
        }
        else
        {
            FCogDebugPlotEventParams& Param = Params.AddDefaulted_GetRef();
            Param.Name = Name;
            Param.Value = Value;
        }
    }

    return *this;
}


//--------------------------------------------------------------------------------------------------------------------------
// FCogPlotEntry
//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlotEntry::AddPoint(float X, float Y)
{
    if (Values.Capacity == 0)
    {
        Values.reserve(2000);
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
FCogDebugPlotEvent& FCogDebugPlotEntry::AddEvent(
    const FCogDebugPlotEntry& OwnwePlot,
    FString OwnerName,
    bool IsInstant,
    const FName EventId,
    const int32 Row,
    const FColor& Color)
{
    if (Events.Max() < 200)
    {
        Events.Reserve(200);
    }

    //-----------------------------------------------------------------------
    // We currently having two events with the same name at the same time.
    // So we stop the current one if any exist.
    //-----------------------------------------------------------------------
    StopEvent(EventId);

    FCogDebugPlotEvent* Event = nullptr;

    int32 AddedIndex = 0;
    if (Events.Num() < Events.Max())
    {
        Event = &Events.AddDefaulted_GetRef();
        AddedIndex = Events.Num() - 1;
    }
    else
    {
        Event = &Events[EventOffset];
        AddedIndex = EventOffset;
        EventOffset = (EventOffset + 1) % Events.Num();
    }

    Event->Id = EventId;
    Event->OwnerName = OwnerName;
    Event->DisplayName = EventId.ToString();
    Event->StartTime = OwnwePlot.Time;
    Event->EndTime = IsInstant ? OwnwePlot.Time : 0.0f;
    Event->StartFrame = OwnwePlot.Frame;
    Event->EndFrame = IsInstant ? OwnwePlot.Frame : 0.0f;
    Event->Row = (Row == FCogDebugPlot::AutoRow) ? OwnwePlot.FindFreeRow() : Row;

    MaxRow = FMath::Max(Event->Row, MaxRow);

    const FColor BorderColor = FCogDebugHelper::GetAutoColor(EventId, Color).WithAlpha(200);
    const FColor FillColor = BorderColor.WithAlpha(100);
    Event->BorderColor = FCogImguiHelper::ToImColor(BorderColor);
    Event->FillColor = FCogImguiHelper::ToImColor(FillColor);

    FCogDebugPlot::LastAddedEventPlotName = OwnwePlot.Name;
    FCogDebugPlot::LastAddedEventIndex = AddedIndex;

    return *Event;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent& FCogDebugPlotEntry::StopEvent(const FName EventId)
{
    FCogDebugPlotEvent* Event = FindLastEventByName(EventId);
    if (Event == nullptr)
    {
        return FCogDebugPlot::DefaultEvent;
    }

    if (Event->EndTime == 0.0f)
    {
        Event->EndTime = Time;
        Event->EndFrame = Frame;
    }

    return *Event;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlotEntry::UpdateTime(const UWorld* World)
{
    Time = World ? World->GetTimeSeconds() : 0.0;
    Frame = GFrameCounter;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent* FCogDebugPlotEntry::GetLastEvent()
{
    if (Events.Num() == 0)
    {
        return nullptr;
    }

    int32 Index = Events.Num() - 1;
    if (EventOffset != 0)
    {
        Index = (Index + EventOffset) % Events.Num();
    }

    return &Events[Index];
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent* FCogDebugPlotEntry::FindLastEventByName(FName EventId)
{
    for (int32 i = Events.Num() - 1; i >= 0; --i)
    {
        //--------------------------------------------------
        // The array cycle so we must offset the index
        //--------------------------------------------------
        int32 Index = i;
        if (EventOffset != 0)
        {
            Index = (i + EventOffset) % Events.Num();
        }

        if (Events[Index].Id == EventId)
        {
            return &Events[Index];
        }
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
int32 FCogDebugPlotEntry::FindFreeRow() const
{
    static float InstantTimeThreshold = 1.0f;
    static float TotalTimeThreshold = 10.0f;
    TSet<int32> OccupiedRows;

    for (int32 i = Events.Num() - 1; i >= 0; --i)
    {
        int32 Index = i;
        if (EventOffset != 0)
        {
            Index = (i + EventOffset) % Events.Num();
        }
        const FCogDebugPlotEvent& Event = Events[Index];

        if (Event.EndTime != 0.0f && Time > Event.EndTime + TotalTimeThreshold)
        {
            break;
        }

        if (Event.StartTime == Event.EndTime && Time > Event.EndTime + InstantTimeThreshold)
        {
            continue;
        }

        if (Event.EndTime != 0.0f)
        {
            continue;
        }

        OccupiedRows.Add(Event.Row);
    }

    int32 FreeRow = 0;
    while (true)
    {
        if (OccupiedRows.Contains(FreeRow) == false)
        {
            break;
        }

        FreeRow++;
    }

    return FreeRow;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlotEntry::AssignAxis(int32 Row, ImAxis YAxis)
{
    CurrentRow = Row;
    CurrentYAxis = YAxis;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlotEntry::ResetAxis()
{
    CurrentRow = INDEX_NONE;
    CurrentYAxis = ImAxis_COUNT;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlotEntry::Clear()
{
    FCogDebugPlot::ResetLastAddedEvent();

    MaxRow = 0;

    if (Values.size() > 0)
    {
        Values.shrink(0);
        ValueOffset = 0;
    }

    if (Events.Num() > 0)
    {
        Events.Empty();
        Events.Shrink();
        EventOffset = 0;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugPlotEntry::FindValue(float x, float& y) const
{
    y = 0.0f;

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

        ImVec2 Point = Values[Index];
        if (Point.x > x)
        {
            FoundAfter = true;
        }

        if (Point.x < x)
        {
            FoundBefore = true;
        }

        if (FoundAfter && FoundBefore)
        {
            y = Point.y;
            return true;
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// FCogPlot
//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlot::Reset()
{
    Plots.Empty();
    Pause = false;
    ResetLastAddedEvent();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlot::Clear()
{
    for (FCogDebugPlotEntry& Entry : FCogDebugPlot::Plots)
    {
        Entry.Clear();
    }

    ResetLastAddedEvent();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlot::ResetLastAddedEvent()
{
    LastAddedEventPlotName = NAME_None;
    LastAddedEventIndex = INDEX_NONE;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent* FCogDebugPlot::GetLastAddedEvent()
{
    FCogDebugPlotEntry* Plot = FindPlot(LastAddedEventPlotName);
    if (Plot == nullptr)
    {
        return nullptr;
    }

    return Plot->GetLastEvent();
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEntry* FCogDebugPlot::FindPlot(const FName Name)
{
    FCogDebugPlotEntry* Plot = Plots.FindByPredicate([Name](const FCogDebugPlotEntry& P) { return P.Name == Name; });
    return Plot;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEntry* FCogDebugPlot::RegisterPlot(const UObject* WorldContextObject, const FName PlotName, bool IsEventPlot)
{
    //----------------------------------------------------------
    // When not visible, we don't go further for performances.
    //----------------------------------------------------------
    if (IsVisible == false)
    {
        return nullptr;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
    if (World == nullptr)
    {
        return nullptr;
    }

    if (FCogDebug::IsDebugActiveForObject(WorldContextObject) == false)
    {
        return nullptr;
    }

    FCogDebugPlotEntry* EntryPtr = FindPlot(PlotName);
    if (EntryPtr == nullptr)
    {
        EntryPtr = &Plots.AddDefaulted_GetRef();
        EntryPtr->Name = PlotName;
        EntryPtr->IsEventPlot = IsEventPlot;
        Plots.Sort([](const FCogDebugPlotEntry& A, const FCogDebugPlotEntry& B) { return A.Name.ToString().Compare(B.Name.ToString()) < 0; });
    }

    if (EntryPtr->CurrentYAxis == ImAxis_COUNT)
    {
        return nullptr;
    }

    const float Time = World->GetTimeSeconds();
    if (Time < EntryPtr->Time)
    {
        EntryPtr->Clear();
    }

    EntryPtr->Time = World->GetTimeSeconds();
    EntryPtr->Frame = GFrameCounter;

    return EntryPtr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlot::PlotValue(const UObject* WorldContextObject, const FName PlotName, const float Value)
{
    FCogDebugPlotEntry* Plot = RegisterPlot(WorldContextObject, PlotName, false);
    if (Plot == nullptr)
    {
        return;
    }

    Plot->AddPoint(Plot->Time, Value);
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent& FCogDebugPlot::PlotEvent(const UObject* WorldContextObject, const FName PlotName, const FName EventId, bool IsInstant, const int32 Row, const FColor& Color)
{
    FCogDebugPlotEntry* Plot = RegisterPlot(WorldContextObject, PlotName, true);
    if (Plot == nullptr)
    {
        ResetLastAddedEvent();
        return DefaultEvent;
    }

    FCogDebugPlotEvent& Event = Plot->AddEvent(*Plot, GetNameSafe(WorldContextObject), IsInstant, EventId, Row, Color);
    return Event;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent& FCogDebugPlot::PlotEventInstant(const UObject* WorldContextObject, const FName PlotName, const FName EventId, const int32 Row, const FColor& Color)
{
    FCogDebugPlotEvent& Event = PlotEvent(WorldContextObject, PlotName, EventId, true, Row, Color);
    return Event;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent& FCogDebugPlot::PlotEventStart(const UObject* WorldContextObject, const FName PlotName, const FName EventId, const int32 Row, const FColor& Color)
{
    FCogDebugPlotEvent& Event = PlotEvent(WorldContextObject, PlotName, EventId, false, Row, Color);
    return Event;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent& FCogDebugPlot::PlotEventStop(const UObject* WorldContextObject, const FName PlotName, const FName EventId)
{
    FCogDebugPlotEntry* Plot = RegisterPlot(WorldContextObject, PlotName, true);
    if (Plot == nullptr)
    {
        return DefaultEvent;
    }

    FCogDebugPlotEvent& Event = Plot->StopEvent(EventId);
    return Event;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent& FCogDebugPlot::PlotEventToggle(const UObject* WorldContextObject, const FName PlotName, const FName EventId, const bool ToggleValue, const int32 Row, const FColor& Color)
{
    if (ToggleValue)
    {
        return PlotEventStart(WorldContextObject, PlotName, EventId, Row, Color);
    }
    else
    {
        return PlotEventStop(WorldContextObject, PlotName, EventId);
    }
}
