// ReSharper disable CppUEBlueprintCallableFunctionUnused
#pragma once

#include "CoreMinimal.h"
#include "CogDebugDrawBlueprint.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(meta = (ScriptName = "CogDebugDrawBlueprint"))
class COGDEBUG_API UCogDebugDrawBlueprint : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawString(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FString& Text, const FVector Location, const FLinearColor Color, bool Persistent);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawPoint(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Location, float size, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawSegment(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector SegmentStart, const FVector SegmentEnd, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawArrow(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector SegmentStart, const FVector SegmentEnd, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawAxis(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Location, const FRotator Rotation, float Scale, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawSphere(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Location, float Radius, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawBox(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Center, const FVector Extent, const FQuat Rotation, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawSolidBox(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Center, const FVector Extent, const FQuat Rotation, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawCapsule(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Center, const float HalfHeight, const float Radius, const FQuat Rotation, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawCircle(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FMatrix& Matrix, float Radius, const FLinearColor Color, bool Persistent, uint8 DepthPriority);
    
    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawCircleArc(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FMatrix& Matrix, float InnerRadius, float OuterRadius, float Angle, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawPoints(const UObject* WorldContextObject, FCogLogCategory LogCategory, const TArray<FVector>& Points, float Radius, const FLinearColor StartColor, const FLinearColor EndColor, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawPath(const UObject* WorldContextObject, FCogLogCategory LogCategory, const TArray<FVector>& Points, float PointSize, const FLinearColor StartColor, const FLinearColor EndColor, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawString2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FString& Text, const FVector2D Location, const FLinearColor Color, bool Persistent);
    
    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawSegment2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector2D SegmentStart, const FVector2D SegmentEnd, const FLinearColor Color, bool Persistent);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawCircle2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector2D Location, float Radius, const FLinearColor Color, bool Persistent);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void DebugDrawRect2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector2D Min, const FVector2D Max, const FLinearColor Color, bool Persistent);

};