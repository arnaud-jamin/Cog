#include "CogDebugEvent.h"

#include "CogDebugTrack.h"
#include "CogDebugTracker.h"

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugEvent::GetActualEndTime(const UWorld& World) const
{
    if (EndTime != 0.0f)
    { return EndTime; }
    
    const float WorldTime = World.GetTimeSeconds();
    return WorldTime;
}

//--------------------------------------------------------------------------------------------------------------------------
uint64 FCogDebugEvent::GetActualEndFrame() const
{
    const float ActualEndFame = EndFrame != 0.0f ? EndFrame : GFrameCounter;
    return ActualEndFame;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent& FCogDebugEvent::AddParam(const FCogDebugEventParamId& InParamId, bool InValue)
{
    if (Track == nullptr || Track->Owner == nullptr || Track->Owner->IsVisible == false)
    { return *this; }

    return AddParam(InParamId, FString::Printf(TEXT("%s"), InValue ? TEXT("True") : TEXT("False")));
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent& FCogDebugEvent::AddParam(const FCogDebugEventParamId& InParamId, int InValue)
{
    if (Track == nullptr || Track->Owner == nullptr || Track->Owner->IsVisible == false)
    { return *this; }
    
    return AddParam(InParamId, FString::Printf(TEXT("%d"), InValue));
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent& FCogDebugEvent::AddParam(const FCogDebugEventParamId& InParamId, float InValue)
{
    if (Track == nullptr || Track->Owner == nullptr || Track->Owner->IsVisible == false)
    { return *this; }
    
    return AddParam(InParamId, FString::Printf(TEXT("%0.2f"), InValue));
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent& FCogDebugEvent::AddParam(const FCogDebugEventParamId& InParamId, FName InValue)
{
    if (Track == nullptr || Track->Owner == nullptr || Track->Owner->IsVisible == false)
    { return *this; }
    
    return AddParam(InParamId, InValue.ToString());
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugEvent& FCogDebugEvent::AddParam(const FCogDebugEventParamId& InParamId, const FString& InValue)
{
    if (Track == nullptr || Track->Owner == nullptr || Track->Owner->IsVisible == false)
    { return *this; }
    
    if (InParamId == "Name")
    {
        DisplayName = InValue;
    }
    else
    {
        FCogDebugEventParams& Param = Params.AddDefaulted_GetRef();
        Param.Name = InParamId;
        Param.Value = InValue;
    }

    return *this;
}
