#include "CogDebugDraw.h"

#include "CogDebug.h"
#include "CogDebugDrawHelper.h"
#include "CogDebugDrawImGui.h"
#include "CogDebugLog.h"
#include "CogDebugReplicator.h"
#include "CogDebugShape.h"
#include "CogImguiHelper.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "ReferenceSkeleton.h"
#include "VisualLogger/VisualLogger.h"

#if ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::String2D(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FString& Text, const FVector2D& Location, const FColor& Color, const bool Persistent)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    FCogDebugDrawImGui::AddText(
        FCogImguiHelper::ToImVec2(Location),
        Text,
        FCogImguiHelper::ToImU32(Color),
        true,
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::Settings.Fade2D);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Segment2D(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector2D& SegmentStart, const FVector2D& SegmentEnd, const FColor& Color, const bool Persistent)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    FCogDebugDrawImGui::AddLine(
        FCogImguiHelper::ToImVec2(SegmentStart),
        FCogImguiHelper::ToImVec2(SegmentEnd),
        FCogImguiHelper::ToImU32(Color),
        FCogDebug::GetDebugThickness(0),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::Settings.Fade2D);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Circle2D(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector2D& Location, const float Radius, const FColor& Color, const bool Persistent)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    FCogDebugDrawImGui::AddCircle(
        FCogImguiHelper::ToImVec2(Location),
        Radius,
        FCogImguiHelper::ToImU32(Color),
        FCogDebug::GetDebugSegments(),
        FCogDebug::GetDebugThickness(0),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::Settings.Fade2D);
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugDraw::Rect2D(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector2D& Min, const FVector2D& Max, const FColor& Color, const bool Persistent)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    FCogDebugDrawImGui::AddRect(
        FCogImguiHelper::ToImVec2(Min),
        FCogImguiHelper::ToImVec2(Max),
        FCogImguiHelper::ToImU32(Color),
        0.0f,
        FCogDebug::GetDebugThickness(0),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::Settings.Fade2D);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::String(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FString& Text, const FVector& Location, const FColor& Color, const bool Persistent)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FColor NewColor = FCogDebug::ModulateDebugColor(World, Color, Persistent);
    UE_VLOG_LOCATION(WorldContextObject, LogCategory, Verbose, Location, 10.0f, NewColor, TEXT("%s"), *Text);

    ::DrawDebugString(
        World,
        Location,
        *Text,
        nullptr,
        NewColor,
        FCogDebug::GetDebugTextDuration(Persistent),
        FCogDebug::Settings.TextShadow,
        FCogDebug::Settings.TextSize);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Point(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& Location, const float Size, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FColor NewColor = FCogDebug::ModulateDebugColor(World, Color, Persistent);
    ::DrawDebugPoint(
        World,
        Location,
        Size,
        NewColor,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority));

    ReplicateShape(WorldContextObject, FCogDebugShape::MakePoint(Location, Size, NewColor, Persistent, FCogDebug::Settings.DepthPriority));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Segment(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& SegmentStart, const FVector& SegmentEnd, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FColor NewColor = FCogDebug::ModulateDebugColor(World, Color, Persistent);
    UE_VLOG_SEGMENT(WorldContextObject, LogCategory, Verbose, SegmentStart, SegmentEnd, NewColor, TEXT_EMPTY);

    ::DrawDebugLine(
        World,
        SegmentStart,
        SegmentEnd,
        NewColor,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority),
        FCogDebug::GetDebugThickness(0));

    ReplicateShape(WorldContextObject, FCogDebugShape::MakeSegment(SegmentStart, SegmentEnd, NewColor, 0.0f, Persistent, DepthPriority));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Bone(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& BoneLocation, const FVector& ParentLocation, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FColor NewColor = FCogDebug::ModulateDebugColor(World, Color, Persistent);
    UE_VLOG_SEGMENT(WorldContextObject, LogCategory, Verbose, BoneLocation, ParentLocation, NewColor, TEXT_EMPTY);

    ::DrawDebugLine(
        World,
        BoneLocation,
        ParentLocation,
        NewColor,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority),
        FCogDebug::GetDebugThickness(0));

    ::DrawDebugPoint(
        World,
        BoneLocation,
        FCogDebug::GetDebugThickness(4.0f),
        NewColor,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority));

    ReplicateShape(WorldContextObject, FCogDebugShape::MakeBone(BoneLocation, ParentLocation, NewColor, 0.0f, Persistent, DepthPriority));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Arrow(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& SegmentStart, const FVector& SegmentEnd, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FColor NewColor = FCogDebug::ModulateDebugColor(World, Color, Persistent);
    UE_VLOG_ARROW(WorldContextObject, LogCategory, Verbose, SegmentStart, SegmentEnd, NewColor, TEXT_EMPTY);

    ::DrawDebugDirectionalArrow(
        World,
        SegmentStart,
        SegmentEnd,
        FCogDebug::Settings.ArrowSize,
        NewColor,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority),
        FCogDebug::GetDebugThickness(0));

    ReplicateShape(WorldContextObject, FCogDebugShape::MakeArrow(SegmentStart, SegmentEnd, FCogDebug::Settings.ArrowSize, NewColor, 0.0f, Persistent, DepthPriority));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Axis(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& AxisLoc, const FRotator& AxisRot, const float Scale, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FRotationMatrix R(AxisRot);
    UE_VLOG_ARROW(WorldContextObject, LogCategory, Verbose, AxisLoc, AxisLoc + R.GetScaledAxis(EAxis::X) * Scale, FColor::Red, TEXT_EMPTY);
    UE_VLOG_ARROW(WorldContextObject, LogCategory, Verbose, AxisLoc, AxisLoc + R.GetScaledAxis(EAxis::Y) * Scale, FColor::Green, TEXT_EMPTY);
    UE_VLOG_ARROW(WorldContextObject, LogCategory, Verbose, AxisLoc, AxisLoc + R.GetScaledAxis(EAxis::Z) * Scale, FColor::Blue, TEXT_EMPTY);

    ::DrawDebugCoordinateSystem(
        World,
        AxisLoc,
        AxisRot,
        Scale * FCogDebug::Settings.AxesScale,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority),
        FCogDebug::GetDebugThickness(0));

    ReplicateShape(WorldContextObject, FCogDebugShape::MakeAxes(AxisLoc, AxisRot, FCogDebug::Settings.ArrowSize, FColor::Red, 0.0f, Persistent, DepthPriority));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Circle(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FMatrix& Matrix, float Radius, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FColor NewColor = FCogDebug::ModulateDebugColor(World, Color, Persistent);
    const FVector Center = Matrix.GetOrigin();
    const FVector UpVector = Matrix.GetUnitAxis(EAxis::X);
    UE_VLOG_CIRCLE(WorldContextObject, LogCategory, Verbose, Center, UpVector, Radius, NewColor, TEXT_EMPTY);

    ::DrawDebugCircle(
        World,
        Matrix,
        Radius,
        FCogDebug::GetCircleSegments(),
        NewColor,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority),
        FCogDebug::GetDebugThickness(0),
        false);

    ReplicateShape(WorldContextObject, FCogDebugShape::MakeCircle(Center, Matrix.Rotator(), Radius, NewColor, 0.0f, Persistent, DepthPriority));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::CircleArc(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FMatrix& Matrix, const float InnerRadius, const float OuterRadius, const float Angle, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FColor NewColor = FCogDebug::ModulateDebugColor(World, Color, Persistent);

    //TODO : Add VLOG

    FCogDebugDrawHelper::DrawArc(
        World,
        Matrix,
        InnerRadius,
        OuterRadius,
        Angle,
        FCogDebug::GetCircleSegments(),
        NewColor,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority),
        FCogDebug::GetDebugThickness(0));

    ReplicateShape(WorldContextObject, FCogDebugShape::MakeCircleArc(Matrix.GetOrigin(), Matrix.Rotator(), InnerRadius, OuterRadius, Angle, NewColor, 0.0f, Persistent, DepthPriority));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::FlatCapsule(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector2D& Start, const FVector2D& End, const float Radius, const float Z, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FColor NewColor = FCogDebug::ModulateDebugColor(World, Color, Persistent);
    // TODO : Add VLOG

    FCogDebugDrawHelper::DrawFlatCapsule(
        World,
        Start,
        End,
        Radius,
        Z,
        FCogDebug::GetCircleSegments(),
        NewColor,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority),
        FCogDebug::GetDebugThickness(0));

    ReplicateShape(WorldContextObject, FCogDebugShape::MakeFlatCapsule(Start, End, Radius, Z, NewColor, 0.0f, Persistent, DepthPriority));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Sphere(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& Location, const float Radius, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FColor NewColor = FCogDebug::ModulateDebugColor(World, Color, Persistent);
    UE_VLOG_CAPSULE(WorldContextObject, LogCategory, Verbose, Location, 0.0f, Radius, FQuat::Identity, NewColor, TEXT_EMPTY);

    FCogDebugDrawHelper::DrawSphere(
        World,
        Location,
        Radius,
        FCogDebug::GetDebugSegments(),
        NewColor,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority),
        FCogDebug::GetDebugThickness(0));

    ReplicateShape(WorldContextObject, FCogDebugShape::MakeCapsule(Location, FQuat::Identity, Radius, 0.0f, NewColor, 0.0f, Persistent, DepthPriority));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Box(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& Center, const FVector& Extent, const FQuat& Rotation, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FColor NewColor = FCogDebug::ModulateDebugColor(World, Color, Persistent);
    UE_VLOG_OBOX(WorldContextObject, LogCategory, Verbose, FBox(-Extent, Extent), FQuatRotationTranslationMatrix::Make(Rotation, Center), NewColor, TEXT_EMPTY);

    ::DrawDebugBox(
        World,
        Center,
        Extent,
        Rotation,
        NewColor,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority),
        FCogDebug::GetDebugThickness(0));

    ReplicateShape(WorldContextObject, FCogDebugShape::MakeBox(Center, FRotator(Rotation), Extent, NewColor, 0.0f, Persistent, DepthPriority));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::SolidBox(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& Center, const FVector& Extent, const FQuat& Rotation, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FColor NewColor = FCogDebug::ModulateDebugColor(World, Color, Persistent);
    UE_VLOG_OBOX(WorldContextObject, LogCategory, Verbose, FBox(-Extent, Extent), FQuatRotationTranslationMatrix::Make(Rotation, Center), NewColor, TEXT_EMPTY);

    // If we make the Box Thick enough, it will be displayed as a filled box.
    // We don't use "DrawDebugSolidBox" because it produced weird result, with color being darker than what is intended
    const float NeededThickness = FMath::Min3(Extent.X, Extent.Y, Extent.Z) * 10.f;

    ::DrawDebugBox(
        World,
        Center,
        Extent,
        Rotation,
        NewColor,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority),
        NeededThickness);

    ReplicateShape(WorldContextObject, FCogDebugShape::MakeSolidBox(Center, FRotator(Rotation), Extent, NewColor, Persistent, DepthPriority));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Frustum(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FMatrix& Matrix, const float Angle, const float AspectRatio, const float NearPlane, const float FarPlane, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FColor NewColor = FCogDebug::ModulateDebugColor(World, Color, Persistent);

    FCogDebugDrawHelper::DrawFrustum(
        World,
        Matrix,
        Angle, 
        AspectRatio, 
        NearPlane, 
        FarPlane,
        NewColor,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority),
        FCogDebug::GetDebugThickness(0));

    // TODO: Replicate Shape
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Capsule(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& Center, const float HalfHeight, const float Radius, const FQuat& Rotation, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const FColor NewColor = FCogDebug::ModulateDebugColor(World, Color, Persistent);
    UE_VLOG_CAPSULE(WorldContextObject, LogCategory, Verbose, Center, HalfHeight, Radius, FQuat::Identity, NewColor, TEXT_EMPTY);

    DrawDebugCapsule(
        World,
        Center,
        HalfHeight,
        Radius,
        Rotation,
        NewColor,
        FCogDebug::GetDebugPersistent(Persistent),
        FCogDebug::GetDebugDuration(Persistent),
        FCogDebug::GetDebugDepthPriority(DepthPriority),
        FCogDebug::GetDebugThickness(0));

    ReplicateShape(WorldContextObject, FCogDebugShape::MakeCapsule(Center, Rotation, Radius, HalfHeight, NewColor, 0.0f, Persistent, DepthPriority));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Points(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const TArray<FVector>& Points, const float Radius, const FColor& StartColor, const FColor& EndColor, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        int32 Index = 0;
        for (const FVector& Point : Points)
        {
            const FLinearColor Color = FLinearColor::LerpUsingHSV(FLinearColor(StartColor), FLinearColor(EndColor), Points.Num() <= 1 ? 0.0f : Index / static_cast<float>(Points.Num() - 1));
            Sphere(LogCategory, WorldContextObject, Point, Radius, Color.ToFColor(true), Persistent, DepthPriority);
            Index++;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Path(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const TArray<FVector>& Points, const float PointSize, const FColor& StartColor, const FColor& EndColor, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    if (Points.Num() == 0)
    {
        return;
    }

    FVector LastPoint = Points[0];

    int32 Index = 0;
    for (const FVector& Position : Points)
    {
        const FLinearColor LinearColor = FLinearColor::LerpUsingHSV(FLinearColor(StartColor), FLinearColor(EndColor), Points.Num() <= 1 ? 0.0f : Index / static_cast<float>(Points.Num() - 1));
        FColor Color = LinearColor.ToFColor(true);

        Point(LogCategory, WorldContextObject, Position, PointSize, Color, Persistent, DepthPriority);

        if (Index > 0)
        {
            Segment(LogCategory, WorldContextObject, LastPoint, Position, Color, Persistent, DepthPriority);
        }
            
        Index++;
        LastPoint = Position;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Skeleton(const FLogCategoryBase& LogCategory, const USkeletalMeshComponent* Skeleton, const FColor& Color, const bool DrawSecondaryBones, const uint8 DepthPriority)
{
    if (Skeleton == nullptr)
    {
        return;
    }

    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    {
        return;
    }

    const FReferenceSkeleton& ReferenceSkeleton = Skeleton->GetSkeletalMeshAsset()->GetRefSkeleton();
    const FTransform WorldTransform = Skeleton->GetComponentTransform();
    const TArray<FTransform>& ComponentSpaceTransforms = Skeleton->GetComponentSpaceTransforms();

    for (int32 BoneIndex = 0; BoneIndex < ComponentSpaceTransforms.Num(); ++BoneIndex)
    {
        if (DrawSecondaryBones == false)
        {
            const FName BoneName = ReferenceSkeleton.GetBoneName(BoneIndex);
            if (FCogDebug::IsSecondarySkeletonBone(BoneName))
            {
                continue;
            }
        }

        const FTransform Transform = ComponentSpaceTransforms[BoneIndex] * WorldTransform;
        const FVector BoneLocation = Transform.GetLocation();
        const int32 ParentIndex = ReferenceSkeleton.GetParentIndex(BoneIndex);

        FVector ParentLocation;
        if (ParentIndex >= 0)
        {
            ParentLocation = (ComponentSpaceTransforms[ParentIndex] * WorldTransform).GetLocation();
        }
        else
        {
            ParentLocation = WorldTransform.GetLocation();
        }

        Bone(LogCategory, Skeleton->GetOwner(), BoneLocation, ParentLocation, Color, false, DepthPriority);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::LineTrace(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& Start, const FVector& End, const bool HasHits, TArray<FHitResult>& HitResults, const FCogDebugDrawLineTraceParams& Settings)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    { return; }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    { return; }

    Settings.Persistent = FCogDebug::GetDebugPersistent(Settings.Persistent);
    FCogDebugDrawHelper::DrawLineTrace(World, Start, End, HasHits, HitResults, Settings);

    ReplicateShape(WorldContextObject,
                   FCogDebugShape::MakeArrow(Start,
                                             End,
                                             FCogDebug::Settings.ArrowSize,
                                             HasHits
                                                 ? Settings.HitColor
                                                 : Settings.NoHitColor,
                                             FCogDebug::Settings.Thickness,
                                             Settings.Persistent,
                                             Settings.DepthPriority));

    ReplicateHitResults(WorldContextObject, HitResults, Settings);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Sweep(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FCollisionShape& Shape, const FVector& Start, const FVector& End, const FQuat& Rotation, const bool HasHits, TArray<FHitResult>& HitResults, const FCogDebugDrawSweepParams& Settings)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    { return; }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    { return; }

    FCogDebugDrawHelper::DrawSweep(World, Shape, Start, End, Rotation, HasHits, HitResults, Settings);

    const FColor Color = HasHits
                       ? Settings.HitColor
                       : Settings.NoHitColor;
    ReplicateShape(WorldContextObject,
                   FCogDebugShape::MakeCollisionShape(Shape,
                                                      Start,
                                                      Rotation,
                                                      Shape.GetExtent(),
                                                      Color,
                                                      FCogDebug::Settings.Thickness,
                                                      Settings.Persistent,
                                                      Settings.DepthPriority));
    ReplicateShape(WorldContextObject,
                   FCogDebugShape::MakeArrow(Start,
                                             End,
                                             FCogDebug::Settings.ArrowSize,
                                             Color,
                                             FCogDebug::Settings.Thickness,
                                             Settings.Persistent,
                                             Settings.DepthPriority));

    ReplicateHitResults(WorldContextObject, HitResults, Settings);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Overlap(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FCollisionShape& Shape, const FVector& Location, const FQuat& Rotation, const bool HasHits, TArray<FOverlapResult>& OverlapResults, const FCogDebugDrawOverlapParams& Settings)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory) == false)
    { return; }

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    { return; }

    FCogDebugDrawHelper::DrawOverlap(World, Shape, Location, Rotation, HasHits, OverlapResults, Settings);

    const FColor Color = OverlapResults.Num() > 0
                       ? Settings.HitColor
                       : Settings.NoHitColor;
    ReplicateShape(WorldContextObject,
                   FCogDebugShape::MakeCollisionShape(Shape,
                                                      Location,
                                                      Rotation,
                                                      Shape.GetExtent(),
                                                      Color,
                                                      FCogDebug::Settings.Thickness,
                                                      Settings.Persistent,
                                                      Settings.DepthPriority));

    // TODO: replicate overlap results
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::ReplicateShape(const UObject* WorldContextObject, const FCogDebugShape& Shape)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World == nullptr)
    {
        return;
    }

    const ENetMode NetMode = World->GetNetMode();
    if (NetMode != NM_DedicatedServer && NetMode != NM_ListenServer)
    {
        return;
    }

    TArray<ACogDebugReplicator*> Replicators;
    ACogDebugReplicator::GetRemoteReplicators(*World, Replicators);

    for (ACogDebugReplicator* Replicator : Replicators)
    {
        if (Replicator == nullptr)
        {
            continue;
        }

        if (FCogDebug::IsReplicatedDebugActiveForObject(WorldContextObject, Replicator->GetServerSelection(), Replicator->IsServerFilteringBySelection()) == false)
        {
            continue;
        }
            
        Replicator->ReplicatedShapes.Add(Shape);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::ReplicateHitResults(const UObject* WorldContextObject, const TArray<FHitResult>& HitResults, const FCogDebugDrawLineTraceParams& Settings)
{
    TSet<const UPrimitiveComponent*> AlreadyDrawnPrimitives;
    TSet<const AActor*>              AlreadyDrawnActors;

    for (const FHitResult& HitResult : HitResults)
    {
        const FColor& HitColor = Settings.HitColor;

        if (Settings.DrawHitLocation)
        {
            ReplicateShape(WorldContextObject,
                FCogDebugShape::MakePoint(HitResult.Location,
                    Settings.HitPointSize,
                    HitColor,
                    Settings.Persistent,
                    Settings.DepthPriority));
        }

        if (Settings.DrawHitImpactPoints)
        {
            ReplicateShape(WorldContextObject,
                FCogDebugShape::MakePoint(HitResult.ImpactPoint,
                    Settings.HitPointSize,
                    HitColor,
                    Settings.Persistent,
                    Settings.DepthPriority));
        }

        if (Settings.DrawHitNormals)
        {
            ReplicateShape(WorldContextObject,
                FCogDebugShape::MakeArrow(HitResult.Location,
                    HitResult.Location + HitResult.Normal * 20.0f,
                    FCogDebug::Settings.ArrowSize,
                    HitColor,
                    Settings.Thickness,
                    Settings.Persistent,
                    Settings.DepthPriority));
        }

        if (Settings.DrawHitImpactNormals)
        {
            ReplicateShape(WorldContextObject,
                FCogDebugShape::MakeArrow(HitResult.ImpactPoint,
                    HitResult.Location + HitResult.ImpactNormal * 20.0f,
                    FCogDebug::Settings.ArrowSize,
                    HitColor,
                    Settings.Thickness,
                    Settings.Persistent,
                    Settings.DepthPriority));
        }

        if (Settings.DrawHitPrimitives)
        {
            const UPrimitiveComponent* PrimitiveComponent = HitResult.GetComponent();
            if (PrimitiveComponent == nullptr)
            {
                continue;
            }

            if (AlreadyDrawnPrimitives.Contains(PrimitiveComponent))
            {
                continue;
            }

            const UBoxComponent* BoxComponent = Cast<UBoxComponent>(PrimitiveComponent);
            const FCollisionShape Shape = PrimitiveComponent->GetCollisionShape();

            AlreadyDrawnPrimitives.Add(PrimitiveComponent);
            const ECollisionChannel CollisionObjectType = PrimitiveComponent->GetCollisionObjectType();
            const FColor            PrimitiveColor = Settings.ChannelColors[CollisionObjectType];

            if (Shape.ShapeType == ECollisionShape::Box && BoxComponent == nullptr)
            {
                FVector Location;
                FVector Extent;
                PrimitiveComponent->Bounds.GetBox().GetCenterAndExtents(Location, Extent);

                // TODO: this adds padding to prevent Z fight. Maybe add this as a parameter.
                Extent += FVector::OneVector;

                ReplicateShape(WorldContextObject,
                    FCogDebugShape::MakeBox(Location,
                        FRotator::ZeroRotator,
                        Extent,
                        PrimitiveColor,
                        Settings.Thickness,
                        Settings.Persistent,
                        Settings.DepthPriority));
            }
            else
            {
                ReplicateShape(WorldContextObject,
                    FCogDebugShape::MakeCollisionShape(Shape,
                        PrimitiveComponent->GetComponentLocation(),
                        PrimitiveComponent->GetComponentQuat(),
                        Shape.GetExtent(),
                        PrimitiveColor,
                        Settings.Thickness,
                        Settings.Persistent,
                        Settings.DepthPriority));
            }
        }
    }
}

#endif //ENABLE_COG
