#include "CogDebugPlot.h"

#include "CogDebugHelper.h"
#include "CogImguiHelper.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugPlotEvent::GetActualEndTime(const FCogDebugHistory& Plot) const
{
    const UWorld* World = Plot.World.Get();
    const float WorldTime = World != nullptr ? World->GetTimeSeconds() : 0.0f;
    const float ActualEndTime = EndTime != 0.0f ? EndTime : WorldTime;
    return ActualEndTime;
}

//--------------------------------------------------------------------------------------------------------------------------
uint64 FCogDebugPlotEvent::GetActualEndFrame(const FCogDebugHistory& Plot) const
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
FCogDebugPlotEvent& FCogDebugEventHistory::AddEvent(
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

    FCogDebugPlotEvent* Event;
    int32 AddedIndex;
    
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
FCogDebugPlotEvent& FCogDebugEventHistory::StopEvent(const FName EventId)
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
FCogDebugPlotEvent* FCogDebugEventHistory::GetLastEvent()
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
FCogDebugPlotEvent* FCogDebugEventHistory::FindLastEventByName(FName EventId)
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
void FCogDebugEventHistory::Clear()
{
    FCogDebugHistory::Clear();

    FCogDebugPlot::ResetLastAddedEvent();

    MaxRow = 0;

    if (Events.Num() > 0)
    {
        Events.Empty();
        Events.Shrink();
        EventOffset = 0;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugPlotEvent& FCogDebugPlot::PlotEvent(const UObject* WorldContextObject, const FName PlotName, const FName EventId, bool IsInstant, const int32 Row, const FColor& Color)
{
    FCogDebugEventHistory* EventHistory = RegisterEvent(WorldContextObject, PlotName);
    if (EventHistory == nullptr)
    {
        ResetLastAddedEvent();
        return DefaultEvent;
    }

    FCogDebugPlotEvent& Event = EventHistory->AddEvent(GetNameSafe(WorldContextObject), IsInstant, EventId, Row, Color);
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
    FCogDebugEventHistory* EventHistory = RegisterEvent(WorldContextObject, PlotName);
    if (EventHistory == nullptr)
    {
        return DefaultEvent;
    }

    FCogDebugPlotEvent& Event = EventHistory->StopEvent(EventId);
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
FCogDebugEventHistory* FCogDebugPlot::RegisterEvent(const UObject* InWorldContextObject, const FName InName)
{

    const UWorld* World;
    if (ShouldRegisterEntry(InWorldContextObject, World) == false)
    {
        return nullptr;
    }

    FCogDebugEventHistory& Event = Events.FindOrAdd(InName);
    Event.Type = FCogDebugHistoryType::Event;

    InitializeEntry(Event, World, InName);

    return &Event;
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
    FCogDebugEventHistory* EventHistory = Events.Find(LastAddedEventPlotName);
    if (EventHistory == nullptr)
    {
        return nullptr;
    }

    return EventHistory->GetLastEvent();
}