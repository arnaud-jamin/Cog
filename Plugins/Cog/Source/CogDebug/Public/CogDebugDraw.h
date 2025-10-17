#pragma once

#include "CoreMinimal.h"
#include "CogCommon.h"

class USkeletalMeshComponent;
struct FCogDebugShape;
struct FCollisionShape;
struct FHitResult;
struct FOverlapResult;

#if ENABLE_COG

struct COGDEBUG_API FCogDebugDraw
{
    static void String2D(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FString& Text, const FVector2D& Location, const FColor& Color, bool Persistent);

    static void Segment2D(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector2D& SegmentStart, const FVector2D& SegmentEnd, const FColor& Color, bool Persistent);
    
    static void Circle2D(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector2D& Location, float Radius, const FColor& Color, const bool Persistent);
    
    static void Rect2D(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector2D& Min, const FVector2D& Max, const FColor& Color, const bool Persistent);

    static void String(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FString& Text, const FVector& Location, const FColor& Color, const bool Persistent);
    
    static void Point(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector& Location, float Size, const FColor& Color, const bool Persistent, const uint8 DepthPriority = 0U);
    
    static void Segment(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector& SegmentStart, const FVector& SegmentEnd, const FColor& Color, const bool Persistent, const uint8 DepthPriority = 0U);
    
    static void Bone(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector& BoneLocation, const FVector& ParentLocation, const FColor& Color, const bool Persistent, const uint8 DepthPriority = 0U);
    
    static void Arrow(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector& SegmentStart, const FVector& SegmentEnd, const FColor& Color, const bool Persistent, const uint8 DepthPriority = 0U);
    
    static void Axis(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector& AxisLoc, const FRotator& AxisRot, float Scale, const bool Persistent, const uint8 DepthPriority = 0U);
    
    static void Circle(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FMatrix& Matrix, float Radius, const FColor& Color, const bool Persistent, const uint8 DepthPriority = 0U);
    
    static void CircleArc(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FMatrix& Matrix, float InnerRadius, float OuterRadius, float Angle, const FColor& Color, bool Persistent, const uint8 DepthPriority = 0U);
    
    static void FlatCapsule(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector2D& Start, const FVector2D& End, const float Radius, const float Z, const FColor& Color, const bool Persistent, const uint8 DepthPriority = 0U);
    
    static void Sphere(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector& Location, float Radius, const FColor& Color, bool Persistent, const uint8 DepthPriority = 0U);
    
    static void Box(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector& Center, const FVector& Extent, const FQuat& Rotation, const FColor& Color, const bool Persistent, const uint8 DepthPriority = 0U);
    
    static void SolidBox(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector& Center, const FVector& Extent, const FQuat& Rotation, const FColor& Color, const bool Persistent, const uint8 DepthPriority = 0U);
    
    static void Capsule(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector& Center, const float HalfHeight, const float Radius, const FQuat& Rotation, const FColor& Color, const bool Persistent, const uint8 DepthPriority = 0U);
    
    static void Points(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const TArray<FVector>& Points, float Radius, const FColor& StartColor, const FColor& EndColor, const bool Persistent, const uint8 DepthPriority = 0U);
    
    static void Path(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const TArray<FVector>& Points, float PointSize, const FColor& StartColor, const FColor& EndColor, const bool Persistent, const uint8 DepthPriority = 0U);
    
    static void Frustum(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FMatrix& Matrix, const float Angle, const float AspectRatio, const float NearPlane, const float FarPlane, const FColor& Color, const bool Persistent, const uint8 DepthPriority = 0U);
    
    static void Skeleton(const FCogLogCategoryAlias& LogCategory, const USkeletalMeshComponent* Skeleton, const FColor& Color, bool DrawSecondaryBones = false, const uint8 DepthPriority = 1);

    static void LineTrace(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FVector& Start, const FVector& End, const bool HasHits, TArray<FHitResult>& HitResults, const FCogDebugDrawLineTraceParams& Settings);

    static void Sweep(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FCollisionShape& Shape, const FVector& Start, const FVector& End, const FQuat& Rotation, const bool HasHits, TArray<FHitResult>& HitResults, const FCogDebugDrawSweepParams& Settings);

    static void Overlap(const FCogLogCategoryAlias& LogCategory, const UObject* WorldContextObject, const FCollisionShape& Shape, const FVector& Location, const FQuat& Rotation, const bool HasHits, TArray<FOverlapResult>& OverlapResults, const FCogDebugDrawOverlapParams& Settings);

    static void ReplicateShape(const UObject* WorldContextObject, const FCogDebugShape& Shape);

    static void ReplicateHitResults(const UObject* WorldContextObject, const TArray<FHitResult>& HitResults, const FCogDebugDrawLineTraceParams& Settings);
};

#endif //ENABLE_COG