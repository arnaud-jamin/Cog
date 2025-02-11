#include "CogDebugEventTrack.h"

#include "CogDebugTracker.h"
#include "CogDebugHelper.h"
#include "CogImguiHelper.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent& FCogDebugEventTrack::AddEvent(
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

    FCogDebugEvent* Event;
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
    Event->Row = (Row == FCogDebugTracker::AutoRow) ? Owner->FindFreeViewRow(GraphIndex) : Row;

    if (IsInstant == false)
    {
        Owner->OccupyViewRow(GraphIndex, Event->Row);
    }

    MaxRow = FMath::Max(Event->Row, MaxRow);

    const FColor BorderColor = FCogDebugHelper::GetAutoColor(EventId, Color).WithAlpha(200);
    const FColor FillColor = BorderColor.WithAlpha(100);
    Event->BorderColor = FCogImguiHelper::ToImColor(BorderColor);
    Event->FillColor = FCogImguiHelper::ToImColor(FillColor);

    Owner->LastAddedEventTrackId = Id;
    Owner->LastAddedEventIndex = AddedIndex;

    return *Event;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent& FCogDebugEventTrack::StopEvent(const FCogDebugEventId EventId)
{
    FCogDebugEvent* Event = FindLastEventByName(EventId);
    if (Event == nullptr)
    {
        return Owner->DefaultEvent;
    }

    if (Event->EndTime == 0.0f)
    {
        Event->EndTime = Time;
        Event->EndFrame = Frame;

        Owner->FreeViewRow(GraphIndex, Event->Row);
    }

    return *Event;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent* FCogDebugEventTrack::GetLastEvent()
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
FCogDebugEvent* FCogDebugEventTrack::FindLastEventByName(FCogDebugEventId EventId)
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
void FCogDebugEventTrack::Clear()
{
    FCogDebugTrack::Clear();

    Owner->ResetLastAddedEvent();

    MaxRow = 0;

    if (Events.Num() > 0)
    {
        Events.Empty();
        Events.Shrink();
        EventOffset = 0;
    }
}
