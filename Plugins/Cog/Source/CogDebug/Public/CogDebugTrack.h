#pragma once

#include "CoreMinimal.h"

struct FCogDebugTracker;

typedef FName FCogDebugTrackId;

//--------------------------------------------------------------------------------------------------------------------------
enum class ECogDebugTrackType
{
    Value,
    Event,
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugTrack
{
    virtual ~FCogDebugTrack() {}

    virtual void Clear() {}

    FCogDebugTrackId Id;

    float Time = 0;

    uint64 Frame = 0;

    int32 GraphIndex = 0;

    ECogDebugTrackType Type = ECogDebugTrackType::Value;

    FCogDebugTracker* Owner = nullptr;
};
