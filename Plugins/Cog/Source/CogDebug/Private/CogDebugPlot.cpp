#include "CogDebugPlot.h"

#include "CogDebug.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

FCogDebugPlotEvent FCogDebugPlot::DefaultEvent;
TMap<FName, FCogDebugValueHistory> FCogDebugPlot::Values;
TMap<FName, FCogDebugEventHistory> FCogDebugPlot::Events;
int32 FCogDebugPlot::NumRecordedValues = 2000;
bool FCogDebugPlot::IsVisible = false;
bool FCogDebugPlot::Pause = false;
bool FCogDebugPlot::RecordValuesWhenPause = true;
FName FCogDebugPlot::LastAddedEventPlotName = NAME_None;
int32 FCogDebugPlot::LastAddedEventIndex = INDEX_NONE;
TMap<int32, TMap<int32, int32>> FCogDebugPlot::OccupationMap;

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlot::Reset()
{
    Values.Empty();
    Events.Empty();
    OccupationMap.Empty();
    Pause = false;
    ResetLastAddedEvent();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlot::Clear()
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
bool FCogDebugPlot::ShouldRegisterEntry(const UObject* WorldContextObject, const UWorld*& World)
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
void FCogDebugPlot::InitializeEntry(FCogDebugHistory& OutValue, const UWorld* InWorld, const FName InName)
{
    const float Time = InWorld->GetTimeSeconds();
    if (Time < OutValue.Time)
    {
        OutValue.Clear();
    }

    OutValue.Name = InName;
    OutValue.World = InWorld;
    OutValue.Time = InWorld->GetTimeSeconds();
    OutValue.Frame = GFrameCounter;
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

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugHistory* FCogDebugPlot::FindEntry(const FName InName)
{
    FCogDebugHistory* Entry = Events.Find(InName);
    if (Entry != nullptr)
    {
        return Entry;
    }

    Entry = Values.Find(InName);
    if (Entry != nullptr)
    {
        return Entry;
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlot::SetNumRecordedValues(int32 InValue)
{
    NumRecordedValues = InValue;

    for (auto& kv : Values)
    {
       kv.Value.SetNumRecordedValues(InValue);
    }
}
