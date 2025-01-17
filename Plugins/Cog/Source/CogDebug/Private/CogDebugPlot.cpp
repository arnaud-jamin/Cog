#include "CogDebugPlot.h"

#include "CogDebug.h"
#include "CogDebugHelper.h"
#include "CogImguiHelper.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

FCogDebugPlotEvent FCogDebugPlot::DefaultEvent;
TArray<FCogDebugPlotEntry> FCogDebugPlot::Plots;
TArray<FCogDebugPlotEntry> FCogDebugPlot::Events;
bool FCogDebugPlot::IsVisible = false;
bool FCogDebugPlot::Pause = false;
FName FCogDebugPlot::LastAddedEventPlotName = NAME_None;
int32 FCogDebugPlot::LastAddedEventIndex = INDEX_NONE;
TMap<int32, TMap<int32, int32>> FCogDebugPlot::OccupationMap;

//--------------------------------------------------------------------------------------------------------------------------
// FCogPlotEvent
//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugPlotEvent::GetActualEndTime(const FCogDebugPlotEntry& Plot) const
{
    const UWorld* World = Plot.World.Get();
    const float WorldTime = World != nullptr ? World->GetTimeSeconds() : 0.0f;
    const float ActualEndTime = EndTime != 0.0f ? EndTime : WorldTime;
    return ActualEndTime;
}

//--------------------------------------------------------------------------------------------------------------------------
uint64 FCogDebugPlotEvent::GetActualEndFrame(const FCogDebugPlotEntry& Plot) const
{
    const float ActualEndFame = EndFrame != 0.0f ? EndFrame : GFrameCounter;
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
    const FString& OwnerName,
    const bool IsInstant,
    const FName EventId,
    const int32 Row,
    const FColor& Color)
{
    if (Events.Max() < 200)
    {
        Events.Reserve(200);
    }

    //----------------------------
    // Stop if it already exist.
    //----------------------------
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
    Event->StartTime = Time;
    Event->EndTime = IsInstant ? Time : 0.0f;
    Event->StartFrame = Frame;
    Event->EndFrame = IsInstant ? Frame : 0.0f;
    Event->Row = (Row == FCogDebugPlot::AutoRow) ? FCogDebugPlot::FindFreeGraphRow(GraphIndex) : Row;

    if (IsInstant == false)
    {
        FCogDebugPlot::OccupyGraphRow(GraphIndex, Event->Row);
    }

    MaxRow = FMath::Max(Event->Row, MaxRow);

    const FColor BorderColor = FCogDebugHelper::GetAutoColor(EventId, Color).WithAlpha(200);
    const FColor FillColor = BorderColor.WithAlpha(100);
    Event->BorderColor = FCogImguiHelper::ToImColor(BorderColor);
    Event->FillColor = FCogImguiHelper::ToImColor(FillColor);

    FCogDebugPlot::LastAddedEventPlotName = Name;
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

		FCogDebugPlot::FreeGraphRow(GraphIndex, Event->Row);
    }

    return *Event;
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
void FCogDebugPlotEntry::AssignGraphAndAxis(int32 InGraph, ImAxis InYAxis)
{
    GraphIndex = InGraph;
    YAxis = InYAxis;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlotEntry::ResetGraphAndAxis()
{
    GraphIndex = INDEX_NONE;
    YAxis = ImAxis_COUNT;
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

        const ImVec2 Point = Values[Index];
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
    Events.Empty();
    OccupationMap.Empty();
    Pause = false;
    ResetLastAddedEvent();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlot::Clear()
{
    for (FCogDebugPlotEntry& Entry : Plots)
    {
        Entry.Clear();
    }

    for (FCogDebugPlotEntry& Entry : Events)
    {
        Entry.Clear();
    }

    OccupationMap.Empty();
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
    FCogDebugPlotEntry* Plot = FindEntry(true, LastAddedEventPlotName);
    if (Plot == nullptr)
    {
        return nullptr;
    }

    return Plot->GetLastEvent();
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEntry* FCogDebugPlot::FindEntry(const FName Name)
{
    if (FCogDebugPlotEntry* Event = Events.FindByPredicate([Name](const FCogDebugPlotEntry& P) { return P.Name == Name; }))
    {
        return Event;
    }

    if (FCogDebugPlotEntry* Plot = Plots.FindByPredicate([Name](const FCogDebugPlotEntry& P) { return P.Name == Name; }))
    {
        return Plot;
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEntry* FCogDebugPlot::FindEntry(bool IsEvent, const FName Name)
{
    TArray<FCogDebugPlotEntry>* Entries = IsEvent ? &Events : &Plots;
    FCogDebugPlotEntry* Entry = Entries->FindByPredicate([Name](const FCogDebugPlotEntry& P) { return P.Name == Name; });
	return Entry;
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

    FCogDebugPlotEntry* EntryPtr = FindEntry(IsEventPlot, PlotName);
    if (EntryPtr == nullptr)
    {
        TArray<FCogDebugPlotEntry>* Entries = IsEventPlot ? &Events : &Plots;
        EntryPtr = &Entries->AddDefaulted_GetRef();
        EntryPtr->Name = PlotName;
        EntryPtr->IsEventPlot = IsEventPlot;
        Entries->Sort([](const FCogDebugPlotEntry& A, const FCogDebugPlotEntry& B) { return A.Name.ToString().Compare(B.Name.ToString()) < 0; });
    }

    //if (EntryPtr->YAxis == ImAxis_COUNT)
    //{
    //    return nullptr;
    //}

    const float Time = World->GetTimeSeconds();
    if (Time < EntryPtr->Time)
    {
        EntryPtr->Clear();
    }

    EntryPtr->World = World;
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

    FCogDebugPlotEvent& Event = Plot->AddEvent(GetNameSafe(WorldContextObject), IsInstant, EventId, Row, Color);
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


//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlot::OccupyGraphRow(const int32 InGraphIndex, const int32 InRow)
{
	TMap<int32, int32>& GraphOccupation = OccupationMap.FindOrAdd(InGraphIndex);

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
void FCogDebugPlot::FreeGraphRow(const int32 InGraphIndex, const int32 Row)
{
    TMap<int32, int32>* GraphOccupation = OccupationMap.Find(InGraphIndex);
    if (GraphOccupation == nullptr)
    { return; }

    int32* RowOccupation = GraphOccupation->Find(Row);
    if (RowOccupation == nullptr)
    { return; }

	(*RowOccupation)--;
}

//--------------------------------------------------------------------------------------------------------------------------
int32 FCogDebugPlot::FindFreeGraphRow(const int32 InGraphIndex)
{
    constexpr int32 MaxRows = 100;

    int32 FreeRow = 0;

    TMap<int32, int32>* GraphOccupation = OccupationMap.Find(InGraphIndex);
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

