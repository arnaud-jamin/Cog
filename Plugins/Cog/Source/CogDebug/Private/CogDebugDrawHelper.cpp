#include "CogDebugDrawHelper.h"

#include "CogDebug.h"
#include "Components/LineBatchComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/OverlapResult.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/SkeletalBodySetup.h"
#include "PhysicsEngine/SphylElem.h"
#include "Runtime/Experimental/Chaos/Private/Chaos/PhysicsObjectInternal.h"

namespace 
{
    //----------------------------------------------------------------------------------------------------------------------
    ULineBatchComponent* GetDebugLineBatcher(const UWorld* InWorld, const bool bPersistentLines, const float LifeTime, const bool bDepthIsForeground)
    {
        return (InWorld ? (bDepthIsForeground ? InWorld->ForegroundLineBatcher : ((bPersistentLines || (LifeTime > 0.f)) ? InWorld->PersistentLineBatcher : InWorld->LineBatcher)) : nullptr);
    }

    //----------------------------------------------------------------------------------------------------------------------
    static float GetLineLifeTime(const ULineBatchComponent* LineBatcher, const float LifeTime, const bool bPersistent)
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
    float AngleStart,
    float AngleEnd,
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
    const float AngleStartRad = FMath::DegreesToRadians(AngleStart);
    const float AngleEndRad = FMath::DegreesToRadians(AngleEnd);
    const FVector Center = Matrix.GetOrigin();

    // Need at least 4 segments
    Segments = FMath::Max(Segments, 4);

    const FVector AxisY = Matrix.GetScaledAxis(EAxis::Y);
    const FVector AxisZ = Matrix.GetScaledAxis(EAxis::Z);

    TArray<FBatchedLine> Lines;
    Lines.Empty(Segments * 2 + 2);

    if (InnerRadius != OuterRadius)
    {
	    const FVector P0 = Center + InnerRadius * (AxisZ * FMath::Sin(AngleStartRad) + AxisY * FMath::Cos(AngleStartRad));
	    const FVector P1 = Center + OuterRadius * (AxisZ * FMath::Sin(AngleStartRad) + AxisY * FMath::Cos(AngleStartRad));
        Lines.Emplace(FBatchedLine(P0, P1, Color, LineLifeTime, Thickness, DepthPriority));

	    const FVector P2 = Center + InnerRadius * (AxisZ * FMath::Sin(AngleEndRad) + AxisY * FMath::Cos(AngleEndRad));
	    const FVector P3 = Center + OuterRadius * (AxisZ * FMath::Sin(AngleEndRad) + AxisY * FMath::Cos(AngleEndRad));
        Lines.Emplace(FBatchedLine(P2, P3, Color, LineLifeTime, Thickness, DepthPriority));
    }

    float CurrentAngle = AngleStartRad;
    const float AngleStep = (AngleEndRad - AngleStartRad) / static_cast<float>(Segments);
    FVector PrevVertex = Center + OuterRadius * (AxisZ * FMath::Sin(CurrentAngle) + AxisY * FMath::Cos(CurrentAngle));
    int32 Count = Segments;
    while (Count--)
    {
        CurrentAngle += AngleStep;
        const FVector NextVertex = Center + OuterRadius * (AxisZ * FMath::Sin(CurrentAngle) + AxisY * FMath::Cos(CurrentAngle));
        Lines.Emplace(FBatchedLine(PrevVertex, NextVertex, Color, LineLifeTime, Thickness, DepthPriority));
        PrevVertex = NextVertex;
    }

    if (InnerRadius != 0.0f)
    {
        CurrentAngle = AngleStartRad;
        PrevVertex = Center + InnerRadius * (AxisZ * FMath::Sin(CurrentAngle) + AxisY * FMath::Cos(CurrentAngle));

        while (Segments--)
        {
            CurrentAngle += AngleStep;
            const FVector NextVertex = Center + InnerRadius * (AxisZ * FMath::Sin(CurrentAngle) + AxisY * FMath::Cos(CurrentAngle));
            Lines.Emplace(FBatchedLine(PrevVertex, NextVertex, Color, LineLifeTime, Thickness, DepthPriority));
            PrevVertex = NextVertex;
        }
    }

    LineBatcher->DrawLines(Lines);
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
    DrawArc(InWorld, Matrix, InnerRadius, OuterRadius, -Angle / 2.0f, Angle / 2.0f, Segments, Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
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
	const FVector2D Forward = (End - Start).GetSafeNormal();
	const FVector2D Right = FVector2D(-Forward.Y, Forward.X);

    ::DrawDebugLine(InWorld, FVector(Start - Right * Radius, Z), FVector(End - Right * Radius, Z), Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    ::DrawDebugLine(InWorld, FVector(Start + Right * Radius, Z), FVector(End + Right * Radius, Z), Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
    ::DrawDebugCircle(InWorld, FRotationTranslationMatrix(FRotator(90, 0, 0), FVector(Start, Z)), Radius, Segments, Color, bPersistentLines, LifeTime, DepthPriority, Thickness, false);
    ::DrawDebugCircle(InWorld, FRotationTranslationMatrix(FRotator(90, 0, 0), FVector(End, Z)), Radius, Segments, Color, bPersistentLines, LifeTime, DepthPriority, Thickness, false);
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
	const FVector Direction(1, 0, 0);
	const FVector LeftVector(0, 1, 0);
	const FVector UpVector(0, 0, 1);

    FVector Verts[8];

    const float HozHalfAngleInRadians = FMath::DegreesToRadians(Angle * 0.5f);

    float HozLength;
    float VertLength;

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

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawQuad(const UWorld* World, const FVector& Position, const FQuat& Rotation, const FVector2D& Extents, const FColor& Color, bool bPersistent, float LifeTime, uint8 DepthPriority, const float Thickness)
{
    if (GEngine->GetNetMode(World) == NM_DedicatedServer)
    {
        return;
    }

    const FVector U = Rotation.GetAxisZ() * Extents.X;
    const FVector V = Rotation.GetAxisY() * Extents.Y;

    const FVector V0 = Position + U + V;
    const FVector V1 = Position + U - V;
    const FVector V2 = Position - U - V;
    const FVector V3 = Position - U + V;

    DrawDebugLine(World, V0, V1, Color, bPersistent, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, V1, V2, Color, bPersistent, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, V2, V3, Color, bPersistent, LifeTime, DepthPriority, Thickness);
    DrawDebugLine(World, V3, V0, Color, bPersistent, LifeTime, DepthPriority, Thickness);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawSolidQuad(const UWorld* World, const FVector& Position, const FQuat& Rotation, const FVector2D& Extents, const FColor& Color, bool bPersistent, float LifeTime, uint8 DepthPriority)
{
    if (GEngine->GetNetMode(World) == NM_DedicatedServer)
    {
        return;
    }

    const FVector U = Rotation.GetAxisZ() * Extents.X;
    const FVector V = Rotation.GetAxisY() * Extents.Y;

    TArray<FVector> Verts;
    Verts.AddUninitialized(4);
    
    Verts[0] = Position + U + V;
    Verts[1] = Position - U + V;
    Verts[2] = Position + U - V;
    Verts[3] = Position - U - V;

    TArray<int32> Indices;
    Indices.AddUninitialized(6);
    
    Indices[0] = 0; 
    Indices[1] = 2; 
    Indices[2] = 1;

    Indices[3] = 1; 
    Indices[4] = 2; 
    Indices[5] = 3;

    DrawDebugMesh(World, Verts, Indices, Color, bPersistent, LifeTime, DepthPriority);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawHitResult(
    const UWorld* World, 
    const FHitResult& HitResult,
    const FCogDebugDrawLineTraceParams& Settings)
{

}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawHitResults(
    const UWorld* World,
    const TArray<FHitResult>& HitResults,
    const FCogDebugDrawLineTraceParams& Settings)
{
    for (const FHitResult& HitResult : HitResults)
    {
        const FColor& HitColor = Settings.HitColor;

        if (Settings.DrawHitLocation)
        {
            DrawDebugPoint(
                World,
                HitResult.Location,
                Settings.HitPointSize,
                HitColor,
                Settings.Persistent,
                Settings.LifeTime,
                Settings.DepthPriority);
        }

        if (Settings.DrawHitImpactPoints)
        {
            DrawDebugPoint(
                World,
                HitResult.ImpactPoint,
                Settings.HitPointSize,
                HitColor,
                Settings.Persistent,
                Settings.LifeTime,
                Settings.DepthPriority);
        }

        if (Settings.DrawHitNormals)
        {
            DrawDebugDirectionalArrow(
                World,
                HitResult.Location,
                HitResult.Location + HitResult.Normal * 20.0f,
                FCogDebug::Settings.ArrowSize,
                Settings.NormalColor,
                Settings.Persistent,
                Settings.LifeTime,
                Settings.DepthPriority,
                Settings.Thickness);
        }

        if (Settings.DrawHitImpactNormals)
        {
            DrawDebugDirectionalArrow(
                World,
                HitResult.ImpactPoint,
                HitResult.ImpactPoint + HitResult.ImpactNormal * 20.0f,
                FCogDebug::Settings.ArrowSize,
                Settings.ImpactNormalColor,
                Settings.Persistent,
                Settings.LifeTime,
                Settings.DepthPriority,
                Settings.Thickness);
        }

        if (Settings.DrawHitPrimitives)
        {
            const UPrimitiveComponent* PrimitiveComponent = HitResult.GetComponent();
            if (PrimitiveComponent == nullptr)
            {
                continue;
            }

            const ECollisionChannel CollisionObjectType = PrimitiveComponent->GetCollisionObjectType();
            const FColor PrimitiveColor = Settings.ChannelColors[CollisionObjectType];
            DrawPrimitiveComponent(
                *PrimitiveComponent, 
                HitResult.PhysicsObject->GetBodyIndex(), 
                PrimitiveColor, 
                Settings.Persistent, 
                Settings.LifeTime, 
                Settings.DepthPriority, 
                Settings.Thickness, 
                Settings.DrawHitPrimitiveActorsName,
                Settings.DrawHitPrimitiveActorsNameShadow,
                Settings.HitPrimitiveActorsNameSize);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawLineTrace(
    const UWorld* World,
    const FVector& Start,
    const FVector& End,
    const bool HasHits,
    const TArray<FHitResult>& HitResults,
    const FCogDebugDrawLineTraceParams& Settings
)
{
    DrawDebugDirectionalArrow(
        World,
        Start,
        End,
        FCogDebug::Settings.ArrowSize,
        HasHits ? Settings.HitColor : Settings.NoHitColor,
        Settings.Persistent,
        Settings.LifeTime,
        Settings.DepthPriority,
        Settings.Thickness);

    DrawHitResults(World, HitResults, Settings);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawSweep(
    const UWorld* World,
    const FCollisionShape& Shape,
    const FVector& Start,
    const FVector& End,
    const FQuat& Rotation,
    const bool HasHits,
    TArray<FHitResult>& HitResults,
    const FCogDebugDrawSweepParams& Settings
)
{
    const FColor Color = HasHits ? Settings.HitColor : Settings.NoHitColor;

    DrawDebugDirectionalArrow(
        World,
        Start,
        End,
        FCogDebug::Settings.ArrowSize,
        Color,
        Settings.Persistent,
        Settings.LifeTime,
        Settings.DepthPriority,
        Settings.Thickness);

	DrawShape(World, Shape, Start, Rotation, FVector::OneVector, Color, Settings.Persistent, Settings.LifeTime, Settings.DepthPriority, Settings.Thickness);

	DrawHitResults(World, HitResults, Settings);

    for (const FHitResult& Hit : HitResults)
    {
        if (Settings.DrawHitShapes)
        {
            DrawShape(World, Shape, Hit.Location, Rotation, FVector::OneVector, Color, Settings.Persistent, Settings.LifeTime, Settings.DepthPriority, Settings.Thickness);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawOverlap(
    const UWorld* World,
    const FCollisionShape& Shape,
    const FVector& Location,
    const FQuat& Rotation,
    const bool HasHits,
    const TArray<FOverlapResult>& OverlapResults,
    const FCogDebugDrawOverlapParams& Settings
)
{
    const FColor Color = HasHits ? Settings.HitColor : Settings.NoHitColor;
    DrawShape(World, Shape, Location, Rotation, FVector::OneVector, Color, Settings.Persistent, Settings.LifeTime, Settings.DepthPriority, Settings.Thickness);

    if (Settings.DrawHitPrimitives)
    {
	    for (const FOverlapResult& OverlapResult : OverlapResults)
	    {
	        if (const UPrimitiveComponent* PrimitiveComponent = OverlapResult.GetComponent())
	        {
	            const ECollisionChannel CollisionObjectType = PrimitiveComponent->GetCollisionObjectType();
	            const FColor PrimitiveColor = Settings.ChannelColors[CollisionObjectType];
	            DrawPrimitiveComponent(
                    *PrimitiveComponent, 
                    OverlapResult.PhysicsObject->GetBodyIndex(), 
                    PrimitiveColor, 
                    Settings.Persistent, 
                    Settings.LifeTime, 
                    Settings.DepthPriority, 
                    Settings.Thickness,
                    Settings.DrawHitPrimitiveActorsName,
                    Settings.DrawHitPrimitiveActorsNameShadow,
                    Settings.HitPrimitiveActorsNameSize);
	        }
	    }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawPrimitiveComponent(
    const UPrimitiveComponent& PrimitiveComponent,
    const int32 BodyIndex,
    const FColor& Color,
    const bool Persistent,
    const float LifeTime,
    const uint8 DepthPriority,
    const float Thickness,
    const bool DrawName,
    const bool DrawNameShadow,
    const float DrawNameSize)
{
    const UWorld* World = PrimitiveComponent.GetWorld();
    if (World == nullptr)
    {
        return;
    }

    if (const USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(&PrimitiveComponent))
    {
        const UPhysicsAsset* PhysicsAsset = SkeletalMeshComponent->GetPhysicsAsset();
        if (PhysicsAsset->SkeletalBodySetups.IsValidIndex(BodyIndex) == false)
        {
	        return;
        }

        const USkeletalBodySetup* BodySetup = PhysicsAsset->SkeletalBodySetups[BodyIndex];
        const int32 BoneIndex = SkeletalMeshComponent->GetBoneIndex(BodySetup->BoneName);
        const FTransform BoneTransform = SkeletalMeshComponent->GetBoneTransform(BoneIndex);

        if (DrawName)
        {
            DrawDebugString(World, BoneTransform.GetLocation(), BodySetup->BoneName.ToString(), nullptr, Color.WithAlpha(255), 0.0f, DrawNameShadow, DrawNameSize);
        }

        for (const FKSphereElem& Sphere : BodySetup->AggGeom.SphereElems)
        {
            const FTransform transform = Sphere.GetTransform() * BoneTransform;
            DrawSphere(
                World,
                transform.GetLocation(),
                Sphere.Radius,
                FCogDebug::GetDebugSegments(),
                Color,
                Persistent,
                LifeTime,
                DepthPriority,
                Thickness);
        }

        for (const FKBoxElem& Box : BodySetup->AggGeom.BoxElems)
        {
            const FTransform transform = Box.GetTransform() * BoneTransform;

            DrawDebugBox(
                World,
                transform.GetLocation(),
                FVector(Box.X, Box.Y, Box.Z) * 0.5f,
                transform.GetRotation(),
                Color,
                Persistent,
                LifeTime,
                DepthPriority,
                Thickness);
        }

        for (const FKSphylElem& Sphy : BodySetup->AggGeom.SphylElems)
        {
            const FTransform transform = Sphy.GetTransform() * BoneTransform;

            DrawDebugCapsule(
                World,
                transform.GetLocation(),
                Sphy.Length * 0.5f,
                Sphy.Radius,
                transform.GetRotation(),
                Color,
                Persistent,
                LifeTime,
                DepthPriority,
                Thickness);
        }
    }
    else
    {
        const AActor* Actor = PrimitiveComponent.GetOwner();

        const UBoxComponent* BoxComponent = Cast<UBoxComponent>(&PrimitiveComponent);
        const FCollisionShape Shape = PrimitiveComponent.GetCollisionShape();

        if (Shape.ShapeType == ECollisionShape::Box && BoxComponent == nullptr)
        {
            FVector Location;
            FVector Extent;
            PrimitiveComponent.Bounds.GetBox().GetCenterAndExtents(Location, Extent);

            if (DrawName)
            {
                DrawDebugString(World, Location, GetNameSafe(Actor), nullptr, Color.WithAlpha(255), 0.0f, DrawNameShadow, DrawNameSize);
            }

            // TODO: this adds padding to prevent Z fight. Maybe add this as a parameter.
            Extent += FVector::OneVector;

            DrawDebugSolidBox(
                World,
                Location,
                Extent,
                FQuat::Identity,
                Color,
                Persistent,
                LifeTime,
                DepthPriority);

            DrawDebugBox(
                World,
                Location,
                Extent,
                FQuat::Identity,
                Color,
                Persistent,
                LifeTime,
                DepthPriority,
                Thickness);


        }
        else
        {
            const FVector Location = PrimitiveComponent.GetComponentLocation();
            const FQuat Rotation = PrimitiveComponent.GetComponentQuat();
            const FVector Scale = PrimitiveComponent.GetComponentScale();

            if (DrawName)
            {
                DrawDebugString(World, Location, GetNameSafe(Actor), nullptr, Color.WithAlpha(255), 0.0f, DrawNameShadow, DrawNameSize);
            }

            DrawShape(World, Shape, Location, Rotation, Scale, Color, Persistent, LifeTime, DepthPriority, Thickness);
        }
    }


}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawHelper::DrawShape(
    const UWorld* World, 
    const FCollisionShape& InShape, 
    const FVector& Location, 
    const FQuat& Rotation, 
    const FVector& Scale, 
    const FColor& Color, 
    const bool Persistent, 
    const float LifeTime, 
    const uint8 DepthPriority, 
    const float Thickness)
{
    switch (InShape.ShapeType)
    {
	    case ECollisionShape::Box:
	    {
	        //--------------------------------------------------
	        // see UBoxComponent::GetScaledBoxExtent()
	        //--------------------------------------------------
	        const FVector HalfExtent(InShape.Box.HalfExtentX * Scale.X, InShape.Box.HalfExtentY * Scale.Y, InShape.Box.HalfExtentZ * Scale.Z);

	        DrawDebugBox(
	            World,
	            Location,
	            HalfExtent,
	            Rotation,
	            Color,
	            Persistent,
	            LifeTime,
	            DepthPriority,
	            Thickness);

	        break;
	    }

	    case ECollisionShape::Sphere:
	    {
	        //--------------------------------------------------
	        // see USphereComponent::GetScaledSphereRadius()
	        //--------------------------------------------------
	        const float RadiusScale = FMath::Min(Scale.X, FMath::Min(Scale.Y, Scale.Z));
	        const float Radius = InShape.Sphere.Radius * RadiusScale;

	        DrawSphere(
	            World,
	            Location,
	            Radius,
	            FCogDebug::GetCircleSegments(),
	            Color,
	            Persistent,
	            LifeTime,
	            DepthPriority,
	            Thickness);
	        break;
	    }

	    case ECollisionShape::Capsule:
	    {
	        //--------------------------------------------------
	        // see UCapsuleComponent::GetScaledCapsuleRadius()
	        //--------------------------------------------------
	        const float Radius = InShape.Capsule.Radius * FMath::Min(Scale.X, Scale.Y);
	        const float HalfHeight = InShape.Capsule.HalfHeight * UE_REAL_TO_FLOAT(Scale.Z);

	        DrawDebugCapsule(
	            World,
	            Location,
	            HalfHeight,
	            Radius,
	            Rotation,
	            Color,
	            Persistent,
	            LifeTime,
	            DepthPriority,
	            Thickness);
	        break;
	    }

    	default: 
            break;
    }
}
