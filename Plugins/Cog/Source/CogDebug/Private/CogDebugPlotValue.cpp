#include "CogDebugPlot.h"

#include "CogImguiHelper.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugValueHistory::SetNumRecordedValues(const int32 Value)
{
    Values.reserve(Value);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugValueHistory::AddPoint(float X, float Y)
{
    if (Values.Capacity == 0)
    {
        Values.reserve(FCogDebugPlot::NumRecordedValues);
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
void FCogDebugValueHistory::Clear()
{
    FCogDebugHistory::Clear();

    if (Values.size() > 0)
    {
        Values.shrink(0);
        ValueOffset = 0;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugValueHistory::FindValue(float x, float& y) const
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
FCogDebugValueHistory* FCogDebugPlot::RegisterValue(const UObject* InWorldContextObject, const FName InName)
{
    const UWorld* World;
    if (ShouldRegisterEntry(InWorldContextObject, World) == false) 
    {
        return nullptr;
    }

    FCogDebugValueHistory& Value = Values.FindOrAdd(InName);
    Value.Type = FCogDebugHistoryType::Value;

    InitializeEntry(Value, World, InName);

    return &Value;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugPlot::PlotValue(const UObject* WorldContextObject, const FName PlotName, const float Value)
{
    if (Pause && RecordValuesWhenPause == false)
    { return; }
    
    FCogDebugValueHistory* ValueHistory = RegisterValue(WorldContextObject, PlotName);
    if (ValueHistory == nullptr)
    { return; }

    ValueHistory->AddPoint(ValueHistory->Time, Value);
}
