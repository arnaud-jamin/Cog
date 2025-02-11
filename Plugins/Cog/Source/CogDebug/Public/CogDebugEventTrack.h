#pragma once

#include "CoreMinimal.h"
#include "CogDebugEvent.h"
#include "CogDebugTrack.h"

struct COGDEBUG_API FCogDebugEventTrack : FCogDebugTrack
{
    FCogDebugEvent& AddEvent(const FString& OwnerName, bool IsInstant, const FCogDebugEventId EventId, const int32 Row, const FColor& Color);

    FCogDebugEvent& StopEvent(const FCogDebugEventId EventId);

    FCogDebugEvent* GetLastEvent();

    FCogDebugEvent* FindLastEventByName(FCogDebugEventId EventId);

    virtual void Clear() override;

    int32 EventOffset = 0;

    int32 MaxRow = 1;

    TArray<FCogDebugEvent> Events;
};
