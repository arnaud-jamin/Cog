#pragma once

#include "CoreMinimal.h"
#include "imgui.h"

typedef FName FCogDebugEventId;
typedef FName FCogDebugEventParamId;
struct FCogDebugTrack;

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugEventParams
{
    FCogDebugEventParamId Name;
    FString Value;
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugEvent
{
    float GetActualEndTime(const UWorld& World) const;

    uint64 GetActualEndFrame() const;

    FCogDebugEvent& AddParam(const FCogDebugEventParamId& InParamId, bool InValue);

    FCogDebugEvent& AddParam(const FCogDebugEventParamId& InParamId, int InValue);

    FCogDebugEvent& AddParam(const FCogDebugEventParamId& InParamId, float InValue);

    FCogDebugEvent& AddParam(const FCogDebugEventParamId& InParamId, FName InValue);

    FCogDebugEvent& AddParam(const FCogDebugEventParamId& InParamId, const FString& InValue);

    FCogDebugTrack* Track = nullptr;
    
    FCogDebugEventParamId Id;
    
    float StartTime = 0.0f;
    
    float EndTime = 0.0f;
    
    uint64 StartFrame = 0;
    
    uint64 EndFrame = 0;
    
    ImU32 BorderColor;
    
    ImU32 FillColor;
    
    int32 Row;
    
    FString OwnerName;
    
    FString DisplayName;
    
    TArray<FCogDebugEventParams> Params;
};
