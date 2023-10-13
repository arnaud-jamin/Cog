#pragma once

#include "CoreMinimal.h"

enum class ECogDebugShape : uint8
{
    Invalid,

    Arrow,
    Axes,
    Bone,
    Box,
    Capsule,
    Circle,
    CircleArc,
    Cone,
    Cylinder,
    FlatCapsule,
    Point,
    Polygon,
    Segment,
    SolidBox
};

struct COGDEBUG_API FCogDebugShape
{
    ECogDebugShape Type = ECogDebugShape::Invalid;
    TArray<FVector> ShapeData;
    FColor Color;
    bool bPersistent = false;
    float Thickness = 0.0f;
    uint8 DepthPriority = 0;

    FCogDebugShape() {}

    bool operator==(const FCogDebugShape& Other) const
    {
        return (Type == Other.Type)
            && (Color == Other.Color)
            && (ShapeData == Other.ShapeData)
            && (bPersistent == Other.bPersistent)
            && (Thickness == Other.Thickness)
            && (DepthPriority == Other.DepthPriority);
    }

    static FCogDebugShape MakePoint(const FVector& Location, const float Size, const FColor& Color, const bool bPersistent, const uint8 DepthPriority);
    static FCogDebugShape MakeSegment(const FVector& StartLocation, const FVector& EndLocation, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority);
    static FCogDebugShape MakeBone(const FVector& BoneLocation, const FVector& ParentLocation, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority);
    static FCogDebugShape MakeArrow(const FVector& StartLocation, const FVector& EndLocation, const float HeadSize, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority);
    static FCogDebugShape MakeAxes(const FVector& Location, const FRotator& Rotation, const float HeadSize, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority);
    static FCogDebugShape MakeBox(const FVector& Center, const FRotator& Rotation, const FVector& Extent, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority);
    static FCogDebugShape MakeSolidBox(const FVector& Center, const FRotator& Rotation, const FVector& Extent, const FColor& Color, const bool bPersistent, const uint8 DepthPriority);
    static FCogDebugShape MakeCone(const FVector& Location, const FVector& Direction, const float Length, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority);
    static FCogDebugShape MakeCylinder(const FVector& Center, const float Radius, const float HalfHeight, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority);
    static FCogDebugShape MakeCircle(const FVector& Center, const FRotator& Rotation, const float Radius, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority);
    static FCogDebugShape MakeCircleArc(const FVector& Center, const FRotator& Rotation, const float InnerRadius, const float OuterRadius, const float Angle, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority);
    static FCogDebugShape MakeCapsule(const FVector& Center, const FQuat& Rotation, const float Radius, const float HalfHeight, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority);
    static FCogDebugShape MakeFlatCapsule(const FVector2D& Start, const FVector2D& End, const float Radius, const float Z, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority);
    static FCogDebugShape MakePolygon(const TArray<FVector>& Verts, const FColor& Color, const bool bPersistent, const uint8 DepthPriority);

    void DrawPoint(UWorld* World);
    void DrawSegment(UWorld* World);
    void DrawBone(UWorld* World);
    void DrawArrow(UWorld* World);
    void DrawAxes(UWorld* World);
    void DrawBox(UWorld* World);
    void DrawSolidBox(UWorld* World);
    void DrawCone(UWorld* World);
    void DrawCylinder(UWorld* World);
    void DrawCicle(UWorld* World);
    void DrawCicleArc(UWorld* World);
    void DrawCapsule(UWorld* World);
    void DrawFlatCapsule(UWorld* World);
    void DrawPolygon(UWorld* World);

    void Draw(UWorld* World);
};

FArchive& operator<<(FArchive& Ar, FCogDebugShape& Shape);

