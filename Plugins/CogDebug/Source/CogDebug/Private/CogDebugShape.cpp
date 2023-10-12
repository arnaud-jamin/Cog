#include "CogDebugShape.h"

#include "CogCommon.h"
#include "CogDebugDrawHelper.h"
#include "DrawDebugHelpers.h"

//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakePoint(const FVector& Location, const float Size, const FColor& Color, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;

    NewElement.ShapeData.Add(Location);

    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::Point;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    NewElement.Thickness = Size;
    return NewElement;
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::DrawPoint(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() == 1)
    {
        DrawDebugPoint(
            World,
            ShapeData[0],
            FCogDebugSettings::GetDebugServerThickness(Thickness),
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority));
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakeSegment(const FVector& StartLocation, const FVector& EndLocation, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;

    NewElement.ShapeData.Add(StartLocation);
    NewElement.ShapeData.Add(EndLocation);

    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::Segment;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    NewElement.Thickness = Thickness;
    return NewElement;
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::DrawSegment(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() == 2)
    {
        DrawDebugLine(
            World,
            ShapeData[0],
            ShapeData[1],
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugServerThickness(Thickness));
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakeArrow(const FVector& StartLocation, const FVector& EndLocation, const float HeadSize, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;

    NewElement.ShapeData.Add(StartLocation);
    NewElement.ShapeData.Add(EndLocation);
    NewElement.ShapeData.Add(FVector(HeadSize, 0, 0));

    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::Arrow;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    NewElement.Thickness = Thickness;
    return NewElement;
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::DrawArrow(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() == 3)
    {
        DrawDebugDirectionalArrow(
            World,
            ShapeData[0],
            ShapeData[1],
            ShapeData[2].X,
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugServerThickness(Thickness));
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakeAxes(const FVector& Location, const FRotator& Rotation, const float HeadSize, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;

    NewElement.ShapeData.Add(Location);
    NewElement.ShapeData.Add(FVector(Rotation.Pitch, Rotation.Yaw, Rotation.Roll));
    NewElement.ShapeData.Add(FVector(HeadSize, 0, 0));

    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::Axes;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    NewElement.Thickness = Thickness;
    return NewElement;
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::DrawAxes(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() == 3)
    {
        DrawDebugCoordinateSystem(
            World,
            ShapeData[0],
            FRotator(ShapeData[1].X, ShapeData[1].Y, ShapeData[1].Z),
            ShapeData[2].X,
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugServerThickness(Thickness));
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakeBox(const FVector& Center, const FRotator& Rotation, const FVector& Extent, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;

    NewElement.ShapeData.Add(Center);
    NewElement.ShapeData.Add(Extent);
    NewElement.ShapeData.Add(FVector(Rotation.Pitch, Rotation.Yaw, Rotation.Roll));

    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::Box;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    NewElement.Thickness = Thickness;
    return NewElement;
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::DrawBox(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() == 3)
    {
        DrawDebugBox(
            World,
            ShapeData[0],
            ShapeData[1],
            FQuat(FRotator(ShapeData[2].X, ShapeData[2].Y, ShapeData[2].Z)),
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugServerThickness(Thickness));
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakeSolidBox(const FVector& Center, const FRotator& Rotation, const FVector& Extent, const FColor& Color, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;

    NewElement.ShapeData.Add(Center);
    NewElement.ShapeData.Add(FVector(Rotation.Pitch, Rotation.Yaw, Rotation.Roll));
    NewElement.ShapeData.Add(Extent);

    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::SolidBox;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    return NewElement;
}


void FCogDebugShape::DrawSolidBox(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() == 12)
    {
        DrawDebugSolidBox(
            World,
            ShapeData[0],
            ShapeData[1],
            FQuat(FRotator(ShapeData[1].X, ShapeData[1].Y, ShapeData[1].Z)),
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority));
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakeCone(const FVector& Location, const FVector& Direction, const float Length, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;

    NewElement.ShapeData.Add(Location);
    NewElement.ShapeData.Add(Direction);
    NewElement.ShapeData.Add(FVector(Length, 0, 0));

    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::Cone;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    NewElement.Thickness = Thickness;
    return NewElement;
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::DrawCone(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() == 3 && ShapeData[2].X > 0)
    {
        const float DefaultConeAngle = 0.25f; // ~ 15 degrees
        DrawDebugCone(
            World,
            ShapeData[0],
            ShapeData[1],
            ShapeData[2].X,
            DefaultConeAngle,
            DefaultConeAngle,
            FCogDebugSettings::GetCircleSegments(),
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugServerThickness(Thickness));
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakeCylinder(const FVector& Center, const float Radius, const float HalfHeight, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;

    NewElement.ShapeData.Add(Center);
    NewElement.ShapeData.Add(FVector(Radius, 0, HalfHeight));

    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::Cylinder;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    NewElement.Thickness = Thickness;
    return NewElement;
}
//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::DrawCylinder(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() == 2)
    {
        DrawDebugCylinder(
            World,
            ShapeData[0] - FVector(0, 0, ShapeData[1].Z),
            ShapeData[0] + FVector(0, 0, ShapeData[1].Z),
            ShapeData[1].X,
            FCogDebugSettings::GetCircleSegments(),
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugServerThickness(Thickness));
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakeCircle(const FVector& Center, const FRotator& Rotation, const float Radius, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;

    NewElement.ShapeData.Add(Center);
    NewElement.ShapeData.Add(FVector(Rotation.Pitch, Rotation.Yaw, Rotation.Roll));
    NewElement.ShapeData.Add(FVector(Radius, 0, 0));

    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::Circle;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    NewElement.Thickness = Thickness;
    return NewElement;
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::DrawCicle(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() == 3)
    {
        DrawDebugCircle(
            World,
            FRotationTranslationMatrix(FRotator(ShapeData[1].X, ShapeData[1].Y, ShapeData[1].Z), ShapeData[0]),
            ShapeData[2].X,
            FCogDebugSettings::GetCircleSegments(),
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugServerThickness(Thickness),
            false);
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakeCircleArc(const FVector& Center, const FRotator& Rotation, const float InnerRadius, const float OuterRadius, const float Angle, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;

    NewElement.ShapeData.Add(Center);
    NewElement.ShapeData.Add(FVector(Rotation.Pitch, Rotation.Yaw, Rotation.Roll));
    NewElement.ShapeData.Add(FVector(InnerRadius, OuterRadius, Angle));

    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::CircleArc;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    NewElement.Thickness = Thickness;
    return NewElement;
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::DrawCicleArc(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() == 3)
    {
        FCogDebugDrawHelper::DrawArc(
            World,
            FRotationTranslationMatrix(FRotator(ShapeData[1].X, ShapeData[1].Y, ShapeData[1].Z), ShapeData[0]),
            ShapeData[2].X,
            ShapeData[2].Y,
            ShapeData[2].Z,
            FCogDebugSettings::GetDebugSegments(),
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugServerThickness(Thickness));
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakeCapsule(const FVector& Center, const FQuat& Rotation, const float Radius, const float HalfHeight, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;

    NewElement.ShapeData.Add(Center);
    NewElement.ShapeData.Add(FVector(Radius, HalfHeight, 0));
    NewElement.ShapeData.Add(Rotation.Euler());

    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::Capsule;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    NewElement.Thickness = Thickness;
    return NewElement;
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::DrawCapsule(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() == 3)
    {
        DrawDebugCapsule(
            World,
            ShapeData[0],
            ShapeData[1].Y,
            ShapeData[1].X,
            FQuat::MakeFromEuler(ShapeData[2]),
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugServerThickness(Thickness));
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakeFlatCapsule(const FVector2D& Start, const FVector2D& End, const float Radius, const float Z, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;

    NewElement.ShapeData.Add(FVector(Start.X, Start.Y, 0));
    NewElement.ShapeData.Add(FVector(End.X, End.Y, 0));
    NewElement.ShapeData.Add(FVector(Radius, Z, 0));

    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::FlatCapsule;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    NewElement.Thickness = Thickness;
    return NewElement;
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::DrawFlatCapsule(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() > 0)
    {
        FCogDebugDrawHelper::DrawFlatCapsule(
            World,
            FVector2D(ShapeData[0].X, ShapeData[0].Y),
            FVector2D(ShapeData[1].X, ShapeData[1].Y),
            ShapeData[2].X,
            ShapeData[2].Y,
            FCogDebugSettings::GetCircleSegments(),
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugServerThickness(Thickness));
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakeBone(const FVector& BoneLocation, const FVector& ParentLocation, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;

    NewElement.ShapeData.Add(BoneLocation);
    NewElement.ShapeData.Add(ParentLocation);

    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::Bone;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    NewElement.Thickness = Thickness;
    return NewElement;
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::DrawBone(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() == 2)
    {
        DrawDebugLine(
            World,
            ShapeData[0],
            ShapeData[1],
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugServerThickness(Thickness));

        DrawDebugPoint(
            World,
            ShapeData[0],
            FCogDebugSettings::GetDebugServerThickness(Thickness) + 4.0f,
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority));
    }

#endif //ENABLE_COG
}


//--------------------------------------------------------------------------------------------------------------------------

FCogDebugShape FCogDebugShape::MakePolygon(const TArray<FVector>& Verts, const FColor& Color, const bool bPersistent, const uint8 DepthPriority)
{
    FCogDebugShape NewElement;
    NewElement.ShapeData = Verts;
    NewElement.Color = Color;
    NewElement.Type = ECogDebugShape::Polygon;
    NewElement.bPersistent = bPersistent;
    NewElement.DepthPriority = DepthPriority;
    NewElement.Thickness = 0.0f;
    return NewElement;
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::DrawPolygon(UWorld* World)
{
#if ENABLE_COG

    if (ShapeData.Num() > 0)
    {
        FVector MidPoint = FVector::ZeroVector;
        TArray<int32> Indices;
        for (int32 Idx = 0; Idx < ShapeData.Num(); Idx++)
        {
            Indices.Add(Idx);
            MidPoint += ShapeData[Idx];
        }

        DrawDebugMesh(
            World,
            ShapeData,
            Indices,
            FCogDebugSettings::ModulateServerColor(Color),
            FCogDebugSettings::GetDebugPersistent(bPersistent),
            FCogDebugSettings::GetDebugDuration(bPersistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority));
    }

#endif //ENABLE_COG
}


//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugShape::Draw(UWorld* World)
{
    switch (Type)
    {
    case ECogDebugShape::Arrow:          DrawArrow(World); break;
    case ECogDebugShape::Axes:           DrawAxes(World); break;
    case ECogDebugShape::Bone:           DrawBone(World); break;
    case ECogDebugShape::Box:            DrawBox(World); break;
    case ECogDebugShape::Capsule:        DrawCapsule(World); break;
    case ECogDebugShape::Circle:         DrawCicle(World); break;
    case ECogDebugShape::CircleArc:      DrawCicleArc(World); break;
    case ECogDebugShape::Cone:           DrawCone(World); break;
    case ECogDebugShape::Cylinder:       DrawCylinder(World); break;
    case ECogDebugShape::FlatCapsule:    DrawFlatCapsule(World); break;
    case ECogDebugShape::Point:          DrawPoint(World); break;
    case ECogDebugShape::Polygon:        DrawPolygon(World); break;
    case ECogDebugShape::Segment:        DrawSegment(World); break;
    default: break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------

FArchive& operator<<(FArchive& Ar, FCogDebugShape& Shape)
{
    Ar << Shape.ShapeData;
    Ar << Shape.Color;
    Ar << Shape.bPersistent;
    Ar << Shape.DepthPriority;
    Ar << Shape.Thickness;

    uint8 TypeNum = static_cast<uint8>(Shape.Type);
    Ar << TypeNum;
    Shape.Type = static_cast<ECogDebugShape>(TypeNum);

    return Ar;
}
