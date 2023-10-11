#include "CogDebugDraw.h"

#include "CogDebugDrawHelper.h"
#include "CogDebugDrawImGui.h"
#include "CogDebugHelper.h"
#include "CogDebugLog.h"
#include "CogDebugModule.h"
#include "CogDebugReplicator.h"
#include "CogDebugSettings.h"
#include "CogDebugShape.h"
#include "CogImguiHelper.h"
#include "Engine/SkeletalMesh.h"
#include "VisualLogger/VisualLogger.h"

#if ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::String2D(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FString& Text, const FVector2D& Location, const FColor& Color, bool Persistent)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        FCogDebugDrawImGui::AddText(
            FCogImguiHelper::ToImVec2(Location),
            Text,
            FCogImguiHelper::ToImU32(Color),
            true,
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::Fade2D);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Segment2D(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector2D& SegmentStart, const FVector2D& SegmentEnd, const FColor& Color, bool Persistent)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        FCogDebugDrawImGui::AddLine(
            FCogImguiHelper::ToImVec2(SegmentStart),
            FCogImguiHelper::ToImVec2(SegmentEnd),
            FCogImguiHelper::ToImU32(Color),
            FCogDebugSettings::GetDebugThickness(0),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::Fade2D);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Circle2D(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector2D& Location, float Radius, const FColor& Color, bool Persistent)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        FCogDebugDrawImGui::AddCircle(
            FCogImguiHelper::ToImVec2(Location),
            Radius,
            FCogImguiHelper::ToImU32(Color),
            FCogDebugSettings::GetDebugSegments(),
            FCogDebugSettings::GetDebugThickness(0),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::Fade2D);
    }
}

//--------------------------------------------------------------------------------------------------------------------------

void FCogDebugDraw::Rect2D(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector2D& Min, const FVector2D& Max, const FColor& Color, bool Persistent)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const ImVec2 ImMin = FCogImguiHelper::ToImVec2(Min);
        const ImVec2 ImMax = FCogImguiHelper::ToImVec2(Max);

        FCogDebugDrawImGui::AddRect(
            ImMin,
            ImMax,
            FCogImguiHelper::ToImU32(Color),
            0.0f,
            FCogDebugSettings::GetDebugThickness(0),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::Fade2D);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::String(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FString& Text, const FVector& Location, const FColor& Color, const bool Persistent)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FColor NewColor = FCogDebugSettings::ModulateDebugColor(WorldContextObject->GetWorld(), Color, Persistent);
        UE_VLOG_LOCATION(WorldContextObject, LogCategory, Verbose, Location, 10.0f, NewColor, TEXT("%s"), *Text);

        ::DrawDebugString(
            WorldContextObject->GetWorld(),
            Location,
            *Text,
            nullptr,
            NewColor,
            FCogDebugSettings::GetDebugTextDuration(Persistent),
            FCogDebugSettings::TextShadow,
            FCogDebugSettings::TextSize);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Point(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& Location, const float Size, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FColor NewColor = FCogDebugSettings::ModulateDebugColor(WorldContextObject->GetWorld(), Color, Persistent);
        ::DrawDebugPoint(
            WorldContextObject->GetWorld(),
            Location,
            Size,
            NewColor,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority));

        ReplicateShape(WorldContextObject, FCogDebugShape::MakePoint(Location, Size, NewColor, Persistent, FCogDebugSettings::DepthPriority));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Segment(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& SegmentStart, const FVector& SegmentEnd, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FColor NewColor = FCogDebugSettings::ModulateDebugColor(WorldContextObject->GetWorld(), Color, Persistent);
        UE_VLOG_SEGMENT(WorldContextObject, LogCategory, Verbose, SegmentStart, SegmentEnd, NewColor, TEXT_EMPTY);
        ::DrawDebugLine(
            WorldContextObject->GetWorld(),
            SegmentStart,
            SegmentEnd,
            NewColor,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugThickness(0));

        ReplicateShape(WorldContextObject, FCogDebugShape::MakeSegment(SegmentStart, SegmentEnd, NewColor, 0.0f, Persistent, DepthPriority));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Bone(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& BoneLocation, const FVector& ParentLocation, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FColor NewColor = FCogDebugSettings::ModulateDebugColor(WorldContextObject->GetWorld(), Color, Persistent);
        UE_VLOG_SEGMENT(WorldContextObject, LogCategory, Verbose, BoneLocation, ParentLocation, NewColor, TEXT_EMPTY);

        ::DrawDebugLine(
            WorldContextObject->GetWorld(),
            BoneLocation,
            ParentLocation,
            NewColor,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugThickness(0));

        ::DrawDebugPoint(
            WorldContextObject->GetWorld(),
            BoneLocation,
            FCogDebugSettings::GetDebugThickness(4.0f),
            NewColor,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority));

        ReplicateShape(WorldContextObject, FCogDebugShape::MakeBone(BoneLocation, ParentLocation, NewColor, 0.0f, Persistent, DepthPriority));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Arrow(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& SegmentStart, const FVector& SegmentEnd, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FColor NewColor = FCogDebugSettings::ModulateDebugColor(WorldContextObject->GetWorld(), Color, Persistent);
        UE_VLOG_ARROW(WorldContextObject, LogCategory, Verbose, SegmentStart, SegmentEnd, NewColor, TEXT_EMPTY);
        ::DrawDebugDirectionalArrow(
            WorldContextObject->GetWorld(),
            SegmentStart,
            SegmentEnd,
            FCogDebugSettings::ArrowSize,
            NewColor,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugThickness(0));

        ReplicateShape(WorldContextObject, FCogDebugShape::MakeArrow(SegmentStart, SegmentEnd, FCogDebugSettings::ArrowSize, NewColor, 0.0f, Persistent, DepthPriority));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Axis(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& AxisLoc, const FRotator& AxisRot, float Scale, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        FRotationMatrix R(AxisRot);
        UE_VLOG_ARROW(WorldContextObject, LogCategory, Verbose, AxisLoc, AxisLoc + R.GetScaledAxis(EAxis::X) * Scale, FColor::Red, TEXT_EMPTY);
        UE_VLOG_ARROW(WorldContextObject, LogCategory, Verbose, AxisLoc, AxisLoc + R.GetScaledAxis(EAxis::Y) * Scale, FColor::Green, TEXT_EMPTY);
        UE_VLOG_ARROW(WorldContextObject, LogCategory, Verbose, AxisLoc, AxisLoc + R.GetScaledAxis(EAxis::Z) * Scale, FColor::Blue, TEXT_EMPTY);
        ::DrawDebugCoordinateSystem(
            WorldContextObject->GetWorld(),
            AxisLoc,
            AxisRot,
            Scale * FCogDebugSettings::AxesScale,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugThickness(0));

        ReplicateShape(WorldContextObject, FCogDebugShape::MakeAxes(AxisLoc, AxisRot, FCogDebugSettings::ArrowSize, FColor::Red, 0.0f, Persistent, DepthPriority));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Circle(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FMatrix& Matrix, float Radius, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FColor NewColor = FCogDebugSettings::ModulateDebugColor(WorldContextObject->GetWorld(), Color, Persistent);
        const FVector Center = Matrix.GetOrigin();
        const FVector UpVector = Matrix.GetUnitAxis(EAxis::X);
        UE_VLOG_CIRCLE(WorldContextObject, LogCategory, Verbose, Center, UpVector, Radius, NewColor, TEXT_EMPTY);
        ::DrawDebugCircle(
            WorldContextObject->GetWorld(),
            Matrix,
            Radius,
            FCogDebugSettings::GetCircleSegments(),
            NewColor,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugThickness(0),
            false);

        ReplicateShape(WorldContextObject, FCogDebugShape::MakeCircle(Center, Matrix.Rotator(), Radius, NewColor, 0.0f, Persistent, DepthPriority));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::CircleArc(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FMatrix& Matrix, float InnerRadius, float OuterRadius, float Angle, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FColor NewColor = FCogDebugSettings::ModulateDebugColor(WorldContextObject->GetWorld(), Color, Persistent);

        //TODO : Add VLOG

        FCogDebugDrawHelper::DrawArc(
            WorldContextObject->GetWorld(),
            Matrix,
            InnerRadius,
            OuterRadius,
            Angle,
            FCogDebugSettings::GetCircleSegments(),
            NewColor,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugThickness(0));

        ReplicateShape(WorldContextObject, FCogDebugShape::MakeCircleArc(Matrix.GetOrigin(), Matrix.Rotator(), InnerRadius, OuterRadius, Angle, NewColor, 0.0f, Persistent, DepthPriority));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::FlatCapsule(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector2D& Start, const FVector2D& End, const float Radius, const float Z, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FColor NewColor = FCogDebugSettings::ModulateDebugColor(WorldContextObject->GetWorld(), Color, Persistent);
        // TODO : Add VLOG

        FCogDebugDrawHelper::DrawFlatCapsule(
            WorldContextObject->GetWorld(),
            Start,
            End,
            Radius,
            Z,
            FCogDebugSettings::GetCircleSegments(),
            NewColor,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugThickness(0));

        ReplicateShape(WorldContextObject, FCogDebugShape::MakeFlatCapsule(Start, End, Radius, Z, NewColor, 0.0f, Persistent, DepthPriority));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Sphere(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& Location, float Radius, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FColor NewColor = FCogDebugSettings::ModulateDebugColor(WorldContextObject->GetWorld(), Color, Persistent);
        UE_VLOG_CAPSULE(WorldContextObject, LogCategory, Verbose, Location, 0.0f, Radius, FQuat::Identity, NewColor, TEXT_EMPTY);
        FCogDebugDrawHelper::DrawSphere(
            WorldContextObject->GetWorld(),
            Location,
            Radius,
            FCogDebugSettings::GetDebugSegments(),
            NewColor,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugThickness(0));

        ReplicateShape(WorldContextObject, FCogDebugShape::MakeCapsule(Location, FQuat::Identity, Radius, 0.0f, NewColor, 0.0f, Persistent, DepthPriority));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Box(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& Center, const FVector& Extent, const FQuat& Rotation, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FColor NewColor = FCogDebugSettings::ModulateDebugColor(WorldContextObject->GetWorld(), Color, Persistent);
        UE_VLOG_OBOX(WorldContextObject, LogCategory, Verbose, FBox(-Extent, Extent), FQuatRotationTranslationMatrix::Make(Rotation, Center), NewColor, TEXT_EMPTY);

        ::DrawDebugBox(
            WorldContextObject->GetWorld(),
            Center,
            Extent,
            Rotation,
            NewColor,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugThickness(0));

        ReplicateShape(WorldContextObject, FCogDebugShape::MakeBox(Center, FRotator(Rotation), Extent, NewColor, 0.0f, Persistent, DepthPriority));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::SolidBox(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& Center, const FVector& Extent, const FQuat& Rotation, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FColor NewColor = FCogDebugSettings::ModulateDebugColor(WorldContextObject->GetWorld(), Color, Persistent);
        UE_VLOG_OBOX(WorldContextObject, LogCategory, Verbose, FBox(-Extent, Extent), FQuatRotationTranslationMatrix::Make(Rotation, Center), NewColor, TEXT_EMPTY);

        // If we make the Box Thick enough, it will be displayed as a filled box.
        // We don't use "DrawDebugSolidBox" because it produced weird result, with color being darker than what is intended
        const float NeededThickness = FMath::Min3(Extent.X, Extent.Y, Extent.Z) * 10.f;

        ::DrawDebugBox(
            WorldContextObject->GetWorld(),
            Center,
            Extent,
            Rotation,
            NewColor,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            NeededThickness);

        ReplicateShape(WorldContextObject, FCogDebugShape::MakeSolidBox(Center, FRotator(Rotation), Extent, NewColor, Persistent, DepthPriority));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Frustrum(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FMatrix& Matrix, const float Angle, const float AspectRatio, const float NearPlane, const float FarPlane, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FColor NewColor = FCogDebugSettings::ModulateDebugColor(WorldContextObject->GetWorld(), Color, Persistent);

        FCogDebugDrawHelper::DrawFrustum(
            WorldContextObject->GetWorld(),
            Matrix,
            Angle, 
            AspectRatio, 
            NearPlane, 
            FarPlane,
            NewColor,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugThickness(0));

        // TODO: Replicate Shape
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Capsule(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const FVector& Center, const float HalfHeight, const float Radius, const FQuat& Rotation, const FColor& Color, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FColor NewColor = FCogDebugSettings::ModulateDebugColor(WorldContextObject->GetWorld(), Color, Persistent);
        UE_VLOG_CAPSULE(WorldContextObject, LogCategory, Verbose, Center, HalfHeight, Radius, FQuat::Identity, NewColor, TEXT_EMPTY);

        DrawDebugCapsule(
            WorldContextObject->GetWorld(),
            Center,
            HalfHeight,
            Radius,
            Rotation,
            NewColor,
            FCogDebugSettings::GetDebugPersistent(Persistent),
            FCogDebugSettings::GetDebugDuration(Persistent),
            FCogDebugSettings::GetDebugDepthPriority(DepthPriority),
            FCogDebugSettings::GetDebugThickness(0));

        ReplicateShape(WorldContextObject, FCogDebugShape::MakeCapsule(Center, Rotation, Radius, HalfHeight, NewColor, 0.0f, Persistent, DepthPriority));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Points(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const TArray<FVector>& Points, float Radius, const FColor& StartColor, const FColor& EndColor, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        int32 index = 0;
        for (const FVector& Point : Points)
        {
            const FLinearColor Color = FLinearColor::LerpUsingHSV(FLinearColor(StartColor), FLinearColor(EndColor), Points.Num() <= 1 ? 0.0f : index / (float)(Points.Num() - 1));
            Sphere(LogCategory, WorldContextObject, Point, Radius, Color.ToFColor(true), Persistent, DepthPriority);
            index++;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Path(const FLogCategoryBase& LogCategory, const UObject* WorldContextObject, const TArray<FVector>& Points, float PointSize, const FColor& StartColor, const FColor& EndColor, const bool Persistent, const uint8 DepthPriority)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        if (Points.Num() == 0)
        {
            return;
        }

        FVector LastPoint = Points[0];

        int32 Index = 0;
        for (const FVector& Position : Points)
        {
            const FLinearColor LinearColor = FLinearColor::LerpUsingHSV(FLinearColor(StartColor), FLinearColor(EndColor), Points.Num() <= 1 ? 0.0f : Index / (float)(Points.Num() - 1));
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
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::Skeleton(const FLogCategoryBase& LogCategory, const USkeletalMeshComponent* Skeleton, const FColor& Color, bool DrawSecondaryBones, uint8 DepthPriority)
{
    if (Skeleton == nullptr)
    {
        return;
    }

    if (FCogDebugLog::IsLogCategoryActive(LogCategory))
    {
        const FReferenceSkeleton& ReferenceSkeleton = Skeleton->GetSkeletalMeshAsset()->GetRefSkeleton();
        const FTransform WorldTransform = Skeleton->GetComponentTransform();
        const TArray<FTransform>& ComponentSpaceTransforms = Skeleton->GetComponentSpaceTransforms();

        for (int32 BoneIndex = 0; BoneIndex < ComponentSpaceTransforms.Num(); ++BoneIndex)
        {
            if (DrawSecondaryBones == false)
            {
                FName BoneName = ReferenceSkeleton.GetBoneName(BoneIndex);
                if (FCogDebugSettings::IsSecondarySkeletonBone(BoneName))
                {
                    continue;
                }
            }

            const FTransform Transform = ComponentSpaceTransforms[BoneIndex] * WorldTransform;
            const FVector BoneLocation = Transform.GetLocation();
            const FRotator BoneRotation = FRotator(Transform.GetRotation());
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
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDraw::ReplicateShape(const UObject* WorldContextObject, const FCogDebugShape& Shape)
{
    UWorld* World = WorldContextObject != nullptr ? WorldContextObject->GetWorld() : nullptr;
    if (World == nullptr)
    {
        return;
    }

    const ENetMode NetMode = World->GetNetMode();
    if (NetMode == NM_DedicatedServer || NetMode == NM_ListenServer)
    {
        TArray<ACogDebugReplicator*> Replicators;
        ACogDebugReplicator::GetRemoteReplicators(*World, Replicators);

        for (ACogDebugReplicator* Replicator : Replicators)
        {
            if (Replicator != nullptr)
            {
                Replicator->ReplicatedShapes.Add(Shape);
            }
        }
    }
}

#endif //ENABLE_COG
