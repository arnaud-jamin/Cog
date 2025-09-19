#pragma once

#include "CoreMinimal.h"
#include "CogDebugEvent.h"
#include "CogDebugSettings.h"
#include "CogDebugTracker.h"
#include "Engine/EngineTypes.h"
#include "UObject/WeakObjectPtrTemplates.h"

class AActor;
class UObject;
class UWorld;
struct FCogDebugDrawLineTraceParams;
struct FCogDebugDrawOverlapParams;
struct FCogDebugDrawSweepParams;

//--------------------------------------------------------------------------------------------------------------------------
struct FCogDebugContext
{
    FCogDebugTracker Tracker;

    TWeakObjectPtr<AActor> Selection;
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebug
{
    static bool IsDebugActiveForObject(const UObject* WorldContextObject);
    
    static bool IsReplicatedDebugActiveForObject(const UObject* WorldContextObject, const AActor* ServerSelection, bool IsServerFilteringBySelection);

    static AActor* GetSelection();

    static void SetSelection(AActor* InValue);

    static bool GetIsFilteringBySelection();

    static void SetIsFilteringBySelection(const UWorld* World, bool Value);

    static FCogDebugTracker& GetTracker();

    static bool GetDebugPersistent(bool bPersistent);
    
    static float GetDebugDuration(bool bPersistent);
    
    static float GetDebugTextDuration(bool bPersistent);
    
    static int GetCircleSegments();
    
    static int GetDebugSegments();
    
    static float GetDebugThickness(float Thickness);
    
    static float GetDebugServerThickness(float Thickness);
    
    static uint8 GetDebugDepthPriority(float DepthPriority);
    
    static FColor ModulateDebugColor(const UWorld* World, const FColor& Color, bool bPersistent = true);
    
    static FColor ModulateServerColor(const FColor& Color);
    
    static bool IsSecondarySkeletonBone(FName BoneName);

    static void Reset();

    static void GetDebugChannelColors(FColor ChannelColors[ECC_MAX]);

    static void GetDebugDrawOverlapSettings(FCogDebugDrawOverlapParams& Params);

    static void GetDebugDrawLineTraceSettings(FCogDebugDrawLineTraceParams& Params);

    static void GetDebugDrawSweepSettings(FCogDebugDrawSweepParams& Params);

    static FCogDebugContext& Get(int32 InPieId);

    static FCogDebugContext& Get();

    static int32 GetPieSessionId();

    static void Plot(const UObject* WorldContextObject, const FCogDebugTrackId& InTrackId, const float Value);

    static FCogDebugEvent& StartEvent(const UObject* WorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId, bool IsInstant, const int32 Row = -1, const FColor& Color = FColor::Transparent);

    static FCogDebugEvent& InstantEvent(const UObject* WorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId, const int32 Row = -1, const FColor& Color = FColor::Transparent);

    static FCogDebugEvent& StartEvent(const UObject* WorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId, const int32 Row = -1, const FColor& Color = FColor::Transparent);

    static FCogDebugEvent& StopEvent(const UObject* WorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId);

    static FCogDebugEvent& ToggleEvent(const UObject* WorldContextObject, const FCogDebugTrackId& InTrackId, const FCogDebugEventId& InEventId, const bool ToggleValue, const int32 Row = -1, const FColor& Color = FColor::Transparent);

    static FCogDebugSettings Settings;

private:

    static void ReplicateSelection(const UWorld* World, AActor* Value);

    static bool IsDebugActiveForObject_Internal(const UObject* WorldContextObject, const AActor* InSelection, bool InIsFilteringBySelection);

    static TMap<int32, FCogDebugContext> DebugContexts;
};

