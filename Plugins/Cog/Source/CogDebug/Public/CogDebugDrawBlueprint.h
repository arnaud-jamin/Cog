// ReSharper disable CppUEBlueprintCallableFunctionUnused
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CogDebugDrawBlueprint.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(meta = (ScriptName = "CogDebugDrawBlueprint"))
class COGDEBUG_API UCogDebugDrawBlueprint : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawString(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FString& Text, const FVector Location, const FLinearColor Color, bool Persistent);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawPoint(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Location, float size, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawSegment(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector SegmentStart, const FVector SegmentEnd, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawArrow(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector SegmentStart, const FVector SegmentEnd, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawAxis(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Location, const FRotator Rotation, float Scale, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawSphere(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Location, float Radius, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawBox(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Center, const FVector Extent, const FQuat Rotation, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawSolidBox(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Center, const FVector Extent, const FQuat Rotation, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawCapsule(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Center, const float HalfHeight, const float Radius, const FQuat Rotation, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawCircle(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FMatrix& Matrix, float Radius, const FLinearColor Color, bool Persistent, uint8 DepthPriority);
    
    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawCircleArc(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FMatrix& Matrix, float InnerRadius, float OuterRadius, float Angle, const FLinearColor Color, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawPoints(const UObject* WorldContextObject, FCogLogCategory LogCategory, const TArray<FVector>& Points, float Radius, const FLinearColor StartColor, const FLinearColor EndColor, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawPath(const UObject* WorldContextObject, FCogLogCategory LogCategory, const TArray<FVector>& Points, float PointSize, const FLinearColor StartColor, const FLinearColor EndColor, bool Persistent, uint8 DepthPriority);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawString2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FString& Text, const FVector2D Location, const FLinearColor Color, bool Persistent);
    
    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawSegment2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector2D SegmentStart, const FVector2D SegmentEnd, const FLinearColor Color, bool Persistent);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawCircle2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector2D Location, float Radius, const FLinearColor Color, bool Persistent);

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void DebugDrawRect2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector2D Min, const FVector2D Max, const FLinearColor Color, bool Persistent);

};