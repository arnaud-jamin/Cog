#include "CogDebugShape.h"

#include "CogCommon.h"
#include "CogDebugDrawHelper.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "VisualLogger/VisualLogger.h"

DEFINE_LOG_CATEGORY(LogCogServerVLOG);

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
void FCogDebugShape::DrawPoint(const UWorld* World) const
{
#if ENABLE_COG

    if (ShapeData.Num() == 1)
    {
        const FVector Location = ShapeData[0];
        const float ServerThickness = FCogDebug::GetDebugServerThickness(Thickness);

        DrawDebugPoint(
            World,
            Location,
            ServerThickness,
            FCogDebug::ModulateServerColor(Color),
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority));

        UE_VLOG_LOCATION(World, LogCogServerVLOG, Verbose, Location, ServerThickness, Color, TEXT(""));
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
void FCogDebugShape::DrawSegment(const UWorld* World) const
{
#if ENABLE_COG

    if (ShapeData.Num() == 2)
    {
        const FVector Start = ShapeData[0];
        const FVector End = ShapeData[1];
        const FColor ServerColor = FCogDebug::ModulateServerColor(Color);
        const float ServerThickness = FCogDebug::GetDebugServerThickness(Thickness);

        DrawDebugLine(
            World,
            Start,
            End,
            ServerColor,
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority),
            ServerThickness);

        UE_VLOG_SEGMENT_THICK(World, LogCogServerVLOG, Verbose, Start, End, ServerColor, ServerThickness, TEXT(""));
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
void FCogDebugShape::DrawArrow(const UWorld* World) const
{
#if ENABLE_COG

    if (ShapeData.Num() == 3)
    {
        const FVector Start = ShapeData[0];
        const FVector End = ShapeData[1];
        const float Size = ShapeData[2].X;
        const FColor ServerColor = FCogDebug::ModulateServerColor(Color);

        DrawDebugDirectionalArrow(
            World,
            Start,
            End,
            Size,
            ServerColor,
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority),
            FCogDebug::GetDebugServerThickness(Thickness));

        UE_VLOG_ARROW(World, LogCogServerVLOG, Verbose, Start, End, ServerColor, TEXT(""));
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
void FCogDebugShape::DrawAxes(const UWorld* World) const
{
#if ENABLE_COG

    if (ShapeData.Num() == 3)
    {
        const FVector Location = ShapeData[0];
        const FRotator Rotation(ShapeData[1].X, ShapeData[1].Y, ShapeData[1].Z);
        const float Scale = ShapeData[2].X;

        DrawDebugCoordinateSystem(
            World,
            Location,
            Rotation,
            Scale,
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority),
            FCogDebug::GetDebugServerThickness(Thickness));

        const FRotationMatrix Matrix(Rotation);
        UE_VLOG_ARROW(World, LogCogServerVLOG, Verbose, Location, Location + Matrix.GetScaledAxis(EAxis::X) * Scale, FColor::Red, TEXT(""));
        UE_VLOG_ARROW(World, LogCogServerVLOG, Verbose, Location, Location + Matrix.GetScaledAxis(EAxis::Y) * Scale, FColor::Green, TEXT(""));
        UE_VLOG_ARROW(World, LogCogServerVLOG, Verbose, Location, Location + Matrix.GetScaledAxis(EAxis::Z) * Scale, FColor::Blue, TEXT(""));
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
void FCogDebugShape::DrawBox(const UWorld* World) const
{
#if ENABLE_COG

    if (ShapeData.Num() == 3)
    {
        const FVector Location = ShapeData[0];
        const FVector Extent = ShapeData[1];
        const FRotator Rotation(ShapeData[2].X, ShapeData[2].Y, ShapeData[2].Z);
        const FColor ServerColor = FCogDebug::ModulateServerColor(Color);

        DrawDebugBox(
            World, Location, Extent, FQuat(Rotation),
            ServerColor,
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority),
            FCogDebug::GetDebugServerThickness(Thickness));

        const FBox Box(-Extent, Extent);
        UE_VLOG_OBOX(World, LogCogServerVLOG, Verbose, Box, FRotationTranslationMatrix(Rotation, Location), ServerColor, TEXT(""));
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


void FCogDebugShape::DrawSolidBox(const UWorld* World) const
{
#if ENABLE_COG

    if (ShapeData.Num() == 12)
    {
        const FVector Location = ShapeData[0];
        const FVector Extent = ShapeData[1];
        const FRotator Rotation(ShapeData[2].X, ShapeData[2].Y, ShapeData[2].Z);
        const FColor ServerColor = FCogDebug::ModulateServerColor(Color);

        DrawDebugSolidBox(
            World,
            Location,
            Extent,
            FQuat(Rotation),
            ServerColor,
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority));

        const FBox Box(-Extent, Extent);
        UE_VLOG_OBOX(World, LogCogServerVLOG, Verbose, Box, FRotationTranslationMatrix(Rotation, Location), ServerColor, TEXT(""));
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
void FCogDebugShape::DrawCone(const UWorld* World) const
{
#if ENABLE_COG

    if (ShapeData.Num() == 3 && ShapeData[2].X > 0)
    {
        const FVector Location = ShapeData[0];
        const FVector Direction = ShapeData[1];
        const float Length = ShapeData[2].X;
        const float DefaultConeAngle = 0.25f; // ~ 15 degrees
        const FColor ServerColor = FCogDebug::ModulateServerColor(Color);

        DrawDebugCone(
            World,
            Location,
            Direction,
            Length,
            DefaultConeAngle,
            DefaultConeAngle,
            FCogDebug::GetCircleSegments(),
            ServerColor,
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority),
            FCogDebug::GetDebugServerThickness(Thickness));

        UE_VLOG_CONE(World, LogCogServerVLOG, Verbose, Location, Direction, Length, DefaultConeAngle, ServerColor, TEXT(""));
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

void FCogDebugShape::DrawCylinder(const UWorld* World) const
{
#if ENABLE_COG

    if (ShapeData.Num() == 2)
    {
        DrawDebugCylinder(
            World,
            ShapeData[0] - FVector(0, 0, ShapeData[1].Z),
            ShapeData[0] + FVector(0, 0, ShapeData[1].Z),
            ShapeData[1].X,
            FCogDebug::GetCircleSegments(),
            FCogDebug::ModulateServerColor(Color),
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority),
            FCogDebug::GetDebugServerThickness(Thickness));
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
void FCogDebugShape::DrawCircle(const UWorld* World) const
{
#if ENABLE_COG

    if (ShapeData.Num() == 3)
    {
        const FVector Location = ShapeData[0];
        const FRotator Rotation (ShapeData[1].X, ShapeData[1].Y, ShapeData[1].Z);
        const FRotationTranslationMatrix Matrix(Rotation, Location);
        const float Radius = ShapeData[2].X;
        const FColor ServerColor = FCogDebug::ModulateServerColor(Color);
        const float ServerThickness = FCogDebug::GetDebugServerThickness(Thickness);

        DrawDebugCircle(
            World,
            Matrix,
            Radius,
            FCogDebug::GetCircleSegments(),
            ServerColor,
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority),
            ServerThickness,
            false);

        UE_VLOG_CIRCLE_THICK(World, LogCogServerVLOG, Verbose, Location, Matrix.GetUnitAxis(EAxis::X), Radius, ServerColor, ServerThickness, TEXT(""));

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
void FCogDebugShape::DrawCircleArc(const UWorld* World) const
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
            FCogDebug::GetDebugSegments(),
            FCogDebug::ModulateServerColor(Color),
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority),
            FCogDebug::GetDebugServerThickness(Thickness));
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
void FCogDebugShape::DrawCapsule(const UWorld* World) const
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
            FCogDebug::ModulateServerColor(Color),
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority),
            FCogDebug::GetDebugServerThickness(Thickness));
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

void FCogDebugShape::DrawFlatCapsule(const UWorld* World) const
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
            FCogDebug::GetCircleSegments(),
            FCogDebug::ModulateServerColor(Color),
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority),
            FCogDebug::GetDebugServerThickness(Thickness));
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
void FCogDebugShape::DrawBone(const UWorld* World) const
{
#if ENABLE_COG

    if (ShapeData.Num() == 2)
    {
        DrawDebugLine(
            World,
            ShapeData[0],
            ShapeData[1],
            FCogDebug::ModulateServerColor(Color),
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority),
            FCogDebug::GetDebugServerThickness(Thickness));

        DrawDebugPoint(
            World,
            ShapeData[0],
            FCogDebug::GetDebugServerThickness(Thickness) + 4.0f,
            FCogDebug::ModulateServerColor(Color),
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority));
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
FCogDebugShape FCogDebugShape::MakeCollisionShape(const FCollisionShape& Shape, const FVector& Location, const FQuat& Rotation, const FVector& Extent, const FColor& Color, const float Thickness, const bool bPersistent, const uint8 DepthPriority)
{
    switch (Shape.ShapeType)
    {
        case ECollisionShape::Box:
            return MakeBox(Location, Rotation.Rotator(), Extent, Color, Thickness, bPersistent, DepthPriority);
        case ECollisionShape::Capsule:
            return MakeCapsule(Location, Rotation, Extent.X, Extent.Z, Color, Thickness, bPersistent, DepthPriority);

        default:
        case ECollisionShape::Sphere:
            return MakeCapsule(Location, Rotation, Extent.X, 0.0f, Color, Thickness, bPersistent, DepthPriority);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugShape::DrawPolygon(const UWorld* World) const
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
            FCogDebug::ModulateServerColor(Color),
            FCogDebug::GetDebugPersistent(bPersistent),
            FCogDebug::GetDebugDuration(bPersistent),
            FCogDebug::GetDebugDepthPriority(DepthPriority));
    }

#endif //ENABLE_COG
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugShape::Draw(const UWorld* World) const
{
    switch (Type)
    {
    case ECogDebugShape::Arrow:          DrawArrow(World); break;
    case ECogDebugShape::Axes:           DrawAxes(World); break;
    case ECogDebugShape::Bone:           DrawBone(World); break;
    case ECogDebugShape::Box:            DrawBox(World); break;
    case ECogDebugShape::Capsule:        DrawCapsule(World); break;
    case ECogDebugShape::Circle:         DrawCircle(World); break;
    case ECogDebugShape::CircleArc:      DrawCircleArc(World); break;
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
