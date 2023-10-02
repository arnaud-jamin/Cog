#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CogDebugDrawBlueprint.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(meta = (ScriptName = "CogDebugDrawBlueprint"))
class COGDEBUG_API UCogDebugDrawBlueprint : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogString(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FString& Text, const FVector Location, const FLinearColor Color, bool Persistent);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogPoint(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Location, float size, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogSegment(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector SegmentStart, const FVector SegmentEnd, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogArrow(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector SegmentStart, const FVector SegmentEnd, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogAxis(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Location, const FRotator Rotation, float Scale, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogSphere(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Location, float Radius, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogBox(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Center, const FVector Extent, const FQuat Rotation, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogSolidBox(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Center, const FVector Extent, const FQuat Rotation, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogCapsule(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Center, const float HalfHeight, const float Radius, const FQuat Rotation, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogCircle(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FMatrix& Matrix, float Radius, const FLinearColor Color, bool Persistent, uint8 DepthPriority);
    
    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogCircleArc(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FMatrix& Matrix, float InnerRadius, float OuterRadius, float Angle, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogPoints(const UObject* WorldContextObject, FCogLogCategory LogCategory, const TArray<FVector>& Points, float Radius, const FLinearColor StartColor, const FLinearColor EndColor, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogPath(const UObject* WorldContextObject, FCogLogCategory LogCategory, const TArray<FVector>& Points, float PointSize, const FLinearColor StartColor, const FLinearColor EndColor, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogString2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FString& Text, const FVector2D Location, const FLinearColor Color, bool Persistent);
    
    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogSegment2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector2D SegmentStart, const FVector2D SegmentEnd, const FLinearColor Color, bool Persistent);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogCircle2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector2D Location, float Radius, const FLinearColor Color, bool Persistent);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugLogRect2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector2D Min, const FVector2D Max, const FLinearColor Color, bool Persistent);

};