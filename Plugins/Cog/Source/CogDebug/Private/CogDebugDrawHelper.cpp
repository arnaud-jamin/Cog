#include "CogDebugDrawHelper.h"

#include "Components/LineBatchComponent.h"
#include "Kismet/KismetSystemLibrary.h"

namespace 
{
    //----------------------------------------------------------------------------------------------------------------------
    ULineBatchComponent* GetDebugLineBatcher(const UWorld* InWorld, bool bPersistentLines, float LifeTime, bool bDepthIsForeground)
    {
        return (InWorld ? (bDepthIsForeground ? InWorld->ForegroundLineBatcher : ((bPersistentLines || (LifeTime > 0.f)) ? InWorld->PersistentLineBatcher : InWorld->LineBatcher)) : nullptr);
    }

    //----------------------------------------------------------------------------------------------------------------------
    static float GetLineLifeTime(ULineBatchComponent* LineBatcher, float LifeTime, bool bPersistent)
    {
        return bPersistent ? -1.0f : ((LifeTime > 0.f) ? LifeTime : LineBatcher->DefaultLifeTime);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawArc(
    const UWorld* InWorld,
    const FMatrix& Matrix,
    float InnerRadius,
    float OuterRadius,
    float Angle,
    int32 Segments,
    const FColor& Color,
    bool bPersistentLines,
    float LifeTime,
    uint8 DepthPriority,
    float Thickness)
{
    if (GEngine->GetNetMode(InWorld) == NM_DedicatedServer)
    {
        return;
    }

    ULineBatchComponent* const LineBatcher = GetDebugLineBatcher(InWorld, bPersistentLines, LifeTime, (DepthPriority == SDPG_Foreground));
    if (LineBatcher == nullptr)
    {
        return;
    }

    const float LineLifeTime = GetLineLifeTime(LineBatcher, LifeTime, bPersistentLines);

    const float AngleRad = FMath::DegreesToRadians(Angle);
    const FVector Center = Matrix.GetOrigin();
    const FVector Direction = Matrix.GetUnitAxis(EAxis::Z);

    // Need at least 4 segments
    Segments = FMath::Max(Segments, 4);

    FVector AxisY, AxisZ;
    FVector DirectionNorm = Direction.GetSafeNormal();
    DirectionNorm.FindBestAxisVectors(AxisZ, AxisY);

    TArray<FBatchedLine> Lines;
    Lines.Empty(Segments * 2 + 2);

    if (InnerRadius != OuterRadius)
    {
        FVector P0 = Center + InnerRadius * (AxisY * -FMath::Sin(-AngleRad) + DirectionNorm * FMath::Cos(-AngleRad));
        FVector P1 = Center + OuterRadius * (AxisY * -FMath::Sin(-AngleRad) + DirectionNorm * FMath::Cos(-AngleRad));
        Lines.Emplace(FBatchedLine(P0, P1, Color, LineLifeTime, Thickness, DepthPriority));

        FVector P2 = Center + InnerRadius * (AxisY * -FMath::Sin(AngleRad) + DirectionNorm * FMath::Cos(AngleRad));
        FVector P3 = Center + OuterRadius * (AxisY * -FMath::Sin(AngleRad) + DirectionNorm * FMath::Cos(AngleRad));
        Lines.Emplace(FBatchedLine(P2, P3, Color, LineLifeTime, Thickness, DepthPriority));
    }

    float CurrentAngle = -AngleRad;
    const float AngleStep = AngleRad / float(Segments) * 2.f;
    FVector PrevVertex = Center + OuterRadius * (AxisY * -FMath::Sin(CurrentAngle) + DirectionNorm * FMath::Cos(CurrentAngle));
    int32 Count = Segments;
    while (Count--)
    {
        CurrentAngle += AngleStep;
        FVector NextVertex = Center + OuterRadius * (AxisY * -FMath::Sin(CurrentAngle) + DirectionNorm * FMath::Cos(CurrentAngle));
        Lines.Emplace(FBatchedLine(PrevVertex, NextVertex, Color, LineLifeTime, Thickness, DepthPriority));
        PrevVertex = NextVertex;
    }

    if (InnerRadius != 0.0f)
    {
        CurrentAngle = -AngleRad;
        PrevVertex = Center + InnerRadius * (AxisY * -FMath::Sin(CurrentAngle) + DirectionNorm * FMath::Cos(CurrentAngle));

        Count = Segments;
        while (Segments--)
        {
            CurrentAngle += AngleStep;
            FVector NextVertex = Center + InnerRadius * (AxisY * -FMath::Sin(CurrentAngle) + DirectionNorm * FMath::Cos(CurrentAngle));
            Lines.Emplace(FBatchedLine(PrevVertex, NextVertex, Color, LineLifeTime, Thickness, DepthPriority));
            PrevVertex = NextVertex;
        }
    }

    LineBatcher->DrawLines(Lines);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawSphere(
    const UWorld* InWorld,
    const FVector& Center,
    const float Radius,
    const int32 Segments,
    const FColor& Color,
    const bool bPersistentLines,
    const float LifeTime,
    const uint8 DepthPriority,
    const float Thickness)
{
    if (GEngine->GetNetMode(InWorld) != NM_DedicatedServer)
    {
        DrawCircle(InWorld, Center, FVector::XAxisVector, FVector::YAxisVector, Color, Radius, Segments, bPersistentLines, LifeTime, DepthPriority, Thickness);
        DrawCircle(InWorld, Center, FVector::XAxisVector, FVector::ZAxisVector, Color, Radius, Segments, bPersistentLines, LifeTime, DepthPriority, Thickness);
        DrawCircle(InWorld, Center, FVector::YAxisVector, FVector::ZAxisVector, Color, Radius, Segments, bPersistentLines, LifeTime, DepthPriority, Thickness);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawFlatCapsule(
    const UWorld* InWorld,
    const FVector2D& Start,
    const FVector2D& End,
    const float Radius,
    const float Z,
    const float Segments,
    const FColor& Color,
    const bool bPersistentLines,
    const float LifeTime,
    const uint8 DepthPriority,
    const float Thickness)
{
    FVector2D Forward = (End - Start).GetSafeNormal();
    FVector2D Right = FVector2D(-Forward.Y, Forward.X);

    ::DrawDebugLine(InWorld, FVector(Start - Right * Radius, Z), FVector(End - Right * Radius, Z), Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    ::DrawDebugLine(InWorld, FVector(Start + Right * Radius, Z), FVector(End + Right * Radius, Z), Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    ::DrawDebugCircle(InWorld, FRotationTranslationMatrix(FRotator(90, 0, 0), FVector(Start, Z)), Radius, Segments, Color, bPersistentLines, LifeTime, DepthPriority, Thickness, false);
    ::DrawDebugCircle(InWorld, FRotationTranslationMatrix(FRotator(90, 0, 0), FVector(End, Z)), Radius, Segments, Color, bPersistentLines, LifeTime, DepthPriority, Thickness, false);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawRaycastSingle(
    const UWorld* World,
    const FVector& Start,
    const FVector& End,
    const EDrawDebugTrace::Type DrawType,
    const bool bHit,
    const FHitResult& Hit,
    const float HitSize,
    const FLinearColor DrawColor,
    const FLinearColor DrawHitColor,
    const float DrawDuration,
    const uint8 DepthPriority /*= 0*/
)
{
    if (DrawType != EDrawDebugTrace::None)
    {
        bool DrawPersistent = DrawType == EDrawDebugTrace::Persistent;
        float DrawTime = (DrawType == EDrawDebugTrace::ForDuration) ? DrawDuration : 0.f;

        if (bHit && Hit.bBlockingHit)
        {
            ::DrawDebugLine(World, Start, Hit.ImpactPoint, DrawColor.ToFColor(true), DrawPersistent, DrawTime);
            ::DrawDebugLine(World, Hit.ImpactPoint, End, DrawHitColor.ToFColor(true), DrawPersistent, DrawTime);

            DrawHitResult(World, Hit, 0, DrawType, false, HitSize, DrawHitColor, DrawTime, DepthPriority);
        }
        else
        {
            ::DrawDebugLine(World, Start, End, DrawColor.ToFColor(true), DrawPersistent, DrawTime);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawSphereOverlapMulti(
    const UWorld* World,
    const FVector& Position,
    const float Radius,
    const EDrawDebugTrace::Type DrawType,
    const bool bOverlap,
    const TArray<AActor*>& OutActors,
    const FLinearColor DrawColor,
    const FLinearColor DrawHitColor,
    const float DrawDuration /*= 0*/
)
{
    if (DrawType == EDrawDebugTrace::None)
        return;

    const bool DrawPersistent = DrawType == EDrawDebugTrace::Persistent;
    const float DrawTime = (DrawType == EDrawDebugTrace::ForDuration) ? DrawDuration : 0.f;

    DrawSphereOverlapSingle(World, Position, Radius, DrawColor.ToFColor(true), DrawPersistent, DrawTime);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawSphereOverlapSingle(
    const UWorld* World,
    const FVector& Position,
    const float Radius,
    const FColor& DrawColor,
    const bool DrawPersistent,
    const float DrawTime /*= -1.f*/,
    const uint8 DepthPriority /*= 0*/
)
{
    ::DrawDebugSphere(World, Position, Radius, 16, DrawColor, DrawPersistent, DrawTime, DepthPriority);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawCapsuleCastMulti(const UWorld* World, const FVector& Start, const FVector& End, const FQuat& Rotation, const float HalfHeight, const float Radius, const EDrawDebugTrace::Type DrawType, const bool bHit, const TArray<FHitResult>& OutHits, const FLinearColor DrawColor, const FLinearColor DrawHitColor, const float DrawDuration /*= 0*/)
{
    if (DrawType == EDrawDebugTrace::None)
        return;

    const bool DrawPersistent = DrawType == EDrawDebugTrace::Persistent;
    const float DrawTime = (DrawType == EDrawDebugTrace::ForDuration) ? DrawDuration : 0.f;

    if (bHit && OutHits.Last().bBlockingHit)
    {
        FVector const BlockingHitPoint = OutHits.Last().Location;

        DrawCapsuleCastSingle(World, Start, BlockingHitPoint, Rotation, HalfHeight, Radius, DrawHitColor.ToFColor(true), DrawPersistent, DrawTime);
        DrawCapsuleCastSingle(World, Start, End, Rotation, HalfHeight, Radius, DrawColor.ToFColor(true), DrawPersistent, DrawTime);
    }
    else
    {
        DrawCapsuleCastSingle(World, Start, End, Rotation, HalfHeight, Radius, DrawColor.ToFColor(true), DrawPersistent, DrawTime);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawCapsuleCastSingle(const UWorld* World, const FVector& Start, const FVector& End, const FQuat& Rotation, const float HalfHeight, const float Radius, const FColor& DrawColor, const bool DrawPersistent, const float DrawDuration /*= -1.f*/, const uint8 DepthPriority /*= 0*/)
{
    ::DrawDebugCapsule(World, Start, HalfHeight, Radius, Rotation, DrawColor, DrawPersistent, DrawDuration, DepthPriority);
    ::DrawDebugLine(World, Start, End, DrawColor, DrawPersistent, DrawDuration, DepthPriority, 0.5f);
    ::DrawDebugCapsule(World, End, HalfHeight, Radius, Rotation, DrawColor, DrawPersistent, DrawDuration, DepthPriority);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawSphereCastMulti(
    const UWorld* World,
    const FVector& Start,
    const FVector& End,
    const float Radius,
    const EDrawDebugTrace::Type DrawType,
    const bool bHit,
    const TArray<FHitResult>& OutHits,
    const FLinearColor DrawColor,
    const FLinearColor DrawHitColor,
    const float DrawDuration /*= 0*/
)
{
    if (DrawType == EDrawDebugTrace::None)
        return;

    const bool DrawPersistent = DrawType == EDrawDebugTrace::Persistent;
    const float DrawTime = (DrawType == EDrawDebugTrace::ForDuration) ? DrawDuration : 0.f;

    if (bHit && OutHits.Last().bBlockingHit)
    {
        FVector const BlockingHitPoint = OutHits.Last().Location;
        DrawSphereCastSingle(World, Start, BlockingHitPoint, Radius, DrawColor.ToFColor(true), DrawPersistent, DrawTime);
        DrawSphereCastSingle(World, BlockingHitPoint, End, Radius, DrawHitColor.ToFColor(true), DrawPersistent, DrawTime);
    }
    else
    {
        DrawSphereCastSingle(World, Start, End, Radius, DrawColor.ToFColor(true), DrawPersistent, DrawTime);
    }
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawSphereCastSingle(
    const UWorld* World,
    const FVector& Start,
    const FVector& End,
    const float Radius,
    const FColor& DrawColor,
    const bool DrawPersistent,
    const float DrawDuration /*= -1.f*/,
    const uint8 DepthPriority /*= 0*/
)
{
    FVector const TraceVec = End - Start;
    float const Dist = TraceVec.Size();

    FVector const Center = Start + TraceVec * 0.5f;
    float const HalfHeight = (Dist * 0.5f) + Radius;

    FQuat const CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
    ::DrawDebugCapsule(World, Center, HalfHeight, Radius, CapsuleRot, DrawColor, DrawPersistent, DrawDuration, DepthPriority);
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawHitResults(
    const UWorld* World,
    const TArray<FHitResult>& OutHits,
    const EDrawDebugTrace::Type DrawType,
    const bool ShowHitIndex,
    const float HitSize,
    const FLinearColor HitColor,
    const float DrawDuration,
    const uint8 DepthPriority /*= 0*/
)
{
    if (DrawType == EDrawDebugTrace::None)
        return;

    for (int32 i = 0; i < OutHits.Num(); ++i)
    {
        DrawHitResult(World, OutHits[i], i, DrawType, ShowHitIndex, HitSize, HitColor, DrawDuration, DepthPriority);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawHitResultsDiscarded(
    const UWorld* World,
    const TArray<FHitResult>& AllHits,
    const TArray<FHitResult>& KeptHits,
    const EDrawDebugTrace::Type DrawType,
    const float HitSize,
    const FLinearColor DrawColor,
    const float DrawDuration,
    const uint8 DepthPriority /*= 0*/
)
{
    if (DrawType == EDrawDebugTrace::None)
        return;

    for (int32 i = 0; i < AllHits.Num(); ++i)
    {
        const FHitResult& PhysicHit = AllHits[i];
        if (!KeptHits.ContainsByPredicate([&](const FHitResult& Hit)
            {
                return Hit.GetActor() == PhysicHit.GetActor() && Hit.Component == PhysicHit.Component && Hit.Distance == PhysicHit.Distance;
            }))
        {
            DrawHitResult(World, PhysicHit, i, DrawType, false, HitSize, DrawColor, DrawDuration, DepthPriority);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawHitResult(
    const UWorld* World,
    const FHitResult& Hit,
    const int HitIndex,
    const EDrawDebugTrace::Type DrawType,
    const bool ShowHitIndex,
    const float HitSize,
    const FLinearColor HitColor,
    const float DrawDuration,
    const uint8 DepthPriority /*= 0*/
)
{
    if (DrawType == EDrawDebugTrace::None)
        return;

    const bool DrawPersistent = DrawType == EDrawDebugTrace::Persistent;
    const float DrawTime = (DrawType == EDrawDebugTrace::ForDuration) ? DrawDuration : 0.f;

    ::DrawDebugSphere(World, Hit.ImpactPoint, HitSize, 12, HitColor.ToFColor(true), DrawPersistent, DrawTime, DepthPriority);
    if (ShowHitIndex)
    {
        ::DrawDebugString(World, Hit.ImpactPoint, FString::Printf(TEXT("%d"), HitIndex), nullptr, HitColor.ToFColor(true), DrawTime, true, 1.0f);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawFrustum(
    const UWorld* World,
    const FMatrix& Matrix,
    const float Angle,
    const float AspectRatio,
    const float NearPlane,
    const float FarPlane,
    const FColor& Color,
    const bool bPersistentLines, 
    const float LifeTime, 
    const uint8 DepthPriority, 
    const float Thickness)
{
    FVector Direction(1, 0, 0);
    FVector LeftVector(0, 1, 0);
    FVector UpVector(0, 0, 1);

    FVector Verts[8];

    const float HozHalfAngleInRadians = FMath::DegreesToRadians(Angle * 0.5f);

    float HozLength = 0.0f;
    float VertLength = 0.0f;

    if (Angle > 0.0f)
    {
        HozLength = NearPlane * FMath::Tan(HozHalfAngleInRadians);
        VertLength = HozLength / AspectRatio;
    }
    else
    {
        const float OrthoWidth = (Angle == 0.0f) ? 1000.0f : -Angle;
        HozLength = OrthoWidth * 0.5f;
        VertLength = HozLength / AspectRatio;
    }

    // near plane verts
    Verts[0] = (Direction * NearPlane) + (UpVector * VertLength) + (LeftVector * HozLength);
    Verts[1] = (Direction * NearPlane) + (UpVector * VertLength) - (LeftVector * HozLength);
    Verts[2] = (Direction * NearPlane) - (UpVector * VertLength) - (LeftVector * HozLength);
    Verts[3] = (Direction * NearPlane) - (UpVector * VertLength) + (LeftVector * HozLength);

    if (Angle > 0.0f)
    {
        HozLength = FarPlane * FMath::Tan(HozHalfAngleInRadians);
        VertLength = HozLength / AspectRatio;
    }

    // far plane verts
    Verts[4] = (Direction * FarPlane) + (UpVector * VertLength) + (LeftVector * HozLength);
    Verts[5] = (Direction * FarPlane) + (UpVector * VertLength) - (LeftVector * HozLength);
    Verts[6] = (Direction * FarPlane) - (UpVector * VertLength) - (LeftVector * HozLength);
    Verts[7] = (Direction * FarPlane) - (UpVector * VertLength) + (LeftVector * HozLength);

    for (int32 X = 0; X < 8; ++X)
    {
        Verts[X] = Matrix.TransformPosition(Verts[X]);
    }

    DrawDebugLine(World, Verts[0], Verts[1], Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, Verts[1], Verts[2], Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, Verts[2], Verts[3], Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, Verts[3], Verts[0], Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, Verts[4], Verts[5], Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, Verts[5], Verts[6], Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, Verts[6], Verts[7], Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, Verts[7], Verts[4], Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, Verts[0], Verts[4], Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, Verts[1], Verts[5], Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, Verts[2], Verts[6], Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, Verts[3], Verts[7], Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
}
