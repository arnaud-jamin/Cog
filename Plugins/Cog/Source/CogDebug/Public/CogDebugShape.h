#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCogServerVLOG, Verbose, All);

struct FCollisionShape;

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
    FColor Color = FColor::White;
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
    static FCogDebugShape MakeCollisionShape(const FCollisionShape& Shape, const FVector& Location, const FQuat& Rotation, const FVector& Extent, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority);

    void DrawPoint(const UWorld* World) const;
    void DrawSegment(const UWorld* World) const;
    void DrawBone(const UWorld* World) const;
    void DrawArrow(const UWorld* World) const;
    void DrawAxes(const UWorld* World) const;
    void DrawBox(const UWorld* World) const;
    void DrawSolidBox(const UWorld* World) const;
    void DrawCone(const UWorld* World) const;
    void DrawCylinder(const UWorld* World) const;
    void DrawCircle(const UWorld* World) const;
    void DrawCircleArc(const UWorld* World) const;
    void DrawCapsule(const UWorld* World) const;
    void DrawFlatCapsule(const UWorld* World) const;
    void DrawPolygon(const UWorld* World) const;

    void Draw(const UWorld* World) const;
};

FArchive& operator<<(FArchive& Ar, FCogDebugShape& Shape);

