#include "CogEngineCollisionTester.h"

#include "CogDebugDrawHelper.h"
#include "CogDebug.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"

//--------------------------------------------------------------------------------------------------------------------------
ACogEngineCollisionTester::ACogEngineCollisionTester(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

	StartComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Start"));
    RootComponent = StartComponent;

	EndComponent = CreateDefaultSubobject<USceneComponent>(TEXT("End"));
    EndComponent->SetupAttachment(RootComponent);
    EndComponent->SetRelativeLocation(FVector(1000, 0, 0));
}

//--------------------------------------------------------------------------------------------------------------------------
bool ACogEngineCollisionTester::ShouldTickIfViewportsOnly() const
{
    if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor && TickInEditor)
    {
        return true;
    }
    
    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineCollisionTester::Tick(float DeltaSeconds)
{
    Query();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineCollisionTester::Query()
{
    AlreadyDrawnActors.Empty();
    AlreadyDrawnComponents.Empty();

    FVector QueryStart = StartComponent->GetComponentLocation();
    FVector QueryEnd = EndComponent->GetComponentLocation();
    FQuat QueryRotation = StartComponent->GetComponentQuat();
    TArray<FHitResult> Hits;
    TArray<FOverlapResult> Overlaps;
    bool HasHits = false;
    
    static const FName TraceTag(TEXT("FCogWindow_Collision"));
    const FCollisionQueryParams QueryParams(TraceTag, SCENE_QUERY_STAT_ONLY(CogHitDetection), TraceComplex);

    const FCollisionResponseTemplate* Profile = UCollisionProfile::Get()->GetProfileByIndex(ProfileIndex);
    const FName ProfileName = Profile != nullptr ? Profile->Name : FName();

    FCollisionShape QueryShape;

    const bool bIsUsingShape = Type == ECogEngine_CollisionQueryType::Overlap || Type == ECogEngine_CollisionQueryType::Sweep;
    if (bIsUsingShape)
    {
        switch (Shape)
        {
            case ECogEngine_CollisionQueryShape::Sphere:    QueryShape.SetSphere(ShapeExtent.X); break;
            case ECogEngine_CollisionQueryShape::Capsule:   QueryShape.SetCapsule(ShapeExtent.X, ShapeExtent.Z); break;
            case ECogEngine_CollisionQueryShape::Box:       QueryShape.SetBox(FVector3f(ShapeExtent)); break;
        }
    }

    switch (Type)
    {
    case ECogEngine_CollisionQueryType::Overlap:
    {
        switch (By)
        {
        case ECogEngine_CollisionQueryBy::Channel:
        {
            HasHits = GetWorld()->OverlapMultiByChannel(Overlaps, QueryStart, QueryRotation, Channel, QueryShape, QueryParams);
            break;
        }

        case ECogEngine_CollisionQueryBy::ObjectType:
        {
            FCollisionObjectQueryParams QueryObjectParams;
            QueryObjectParams.ObjectTypesToQuery = ObjectTypesToQuery;
            HasHits = GetWorld()->OverlapMultiByObjectType(Overlaps, QueryStart, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
            break;
        }

        case ECogEngine_CollisionQueryBy::Profile:
        {
            HasHits = GetWorld()->OverlapMultiByProfile(Overlaps, QueryStart, QueryRotation, ProfileName, QueryShape, QueryParams);
            break;
        }
        }

        break;
    }

    case ECogEngine_CollisionQueryType::LineTrace:
    {
        switch (By)
        {
        case ECogEngine_CollisionQueryBy::Channel:
        {
            switch (Mode)
            {
            case  ECogEngine_CollisionQueryMode::Single:
            {
                FHitResult Hit;
                HasHits = GetWorld()->LineTraceSingleByChannel(Hit, QueryStart, QueryEnd, Channel, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
                break;
            }
            case ECogEngine_CollisionQueryMode::Multi:
            {
                HasHits = GetWorld()->LineTraceMultiByChannel(Hits, QueryStart, QueryEnd, Channel, QueryParams);
                break;
            }
            case ECogEngine_CollisionQueryMode::Test:
            {
                HasHits = GetWorld()->LineTraceTestByChannel(QueryStart, QueryEnd, Channel, QueryParams);
                break;
            }
            }

            break;
        }

        case ECogEngine_CollisionQueryBy::ObjectType:
        {
            FCollisionObjectQueryParams QueryObjectParams;
            QueryObjectParams.ObjectTypesToQuery = ObjectTypesToQuery;

            switch (Mode)
            {
            case  ECogEngine_CollisionQueryMode::Single:
            {
                FHitResult Hit;
                HasHits = GetWorld()->LineTraceSingleByObjectType(Hit, QueryStart, QueryEnd, QueryObjectParams, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
                break;
            }
            case ECogEngine_CollisionQueryMode::Multi:
            {
                HasHits = GetWorld()->LineTraceMultiByObjectType(Hits, QueryStart, QueryEnd, QueryObjectParams, QueryParams);
                break;
            }
            case ECogEngine_CollisionQueryMode::Test:
            {
                HasHits = GetWorld()->LineTraceTestByObjectType(QueryStart, QueryEnd, QueryObjectParams, QueryParams);
                break;
            }
            }
            break;
        }

        case ECogEngine_CollisionQueryBy::Profile:
        {
            switch (Mode)
            {
            case  ECogEngine_CollisionQueryMode::Single:
            {
                FHitResult Hit;
                HasHits = GetWorld()->LineTraceSingleByProfile(Hit, QueryStart, QueryEnd, ProfileName, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
                break;
            }
            case ECogEngine_CollisionQueryMode::Multi:
            {
                HasHits = GetWorld()->LineTraceMultiByProfile(Hits, QueryStart, QueryEnd, ProfileName, QueryParams);
                break;
            }
            case ECogEngine_CollisionQueryMode::Test:
            {
                HasHits = GetWorld()->LineTraceTestByProfile(QueryStart, QueryEnd, ProfileName, QueryParams);
                break;
            }
            }
            break;
        }
        }
        break;
    }

    case ECogEngine_CollisionQueryType::Sweep:
    {
        switch (By)
        {
        case ECogEngine_CollisionQueryBy::Channel:
        {
            switch (Mode)
            {
            case  ECogEngine_CollisionQueryMode::Single:
            {
                FHitResult Hit;
                HasHits = GetWorld()->SweepSingleByChannel(Hit, QueryStart, QueryEnd, QueryRotation, Channel, QueryShape, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
                break;
            }
            case ECogEngine_CollisionQueryMode::Multi:
            {
                HasHits = GetWorld()->SweepMultiByChannel(Hits, QueryStart, QueryEnd, QueryRotation, Channel, QueryShape, QueryParams);
                break;
            }
            case ECogEngine_CollisionQueryMode::Test:
            {
                HasHits = GetWorld()->SweepTestByChannel(QueryStart, QueryEnd, QueryRotation, Channel, QueryShape, QueryParams);
                break;
            }
            }
            break;
        }

        case ECogEngine_CollisionQueryBy::ObjectType:
        {
            FCollisionObjectQueryParams QueryObjectParams;
            QueryObjectParams.ObjectTypesToQuery = ObjectTypesToQuery;

            switch (Mode)
            {
            case  ECogEngine_CollisionQueryMode::Single:
            {
                FHitResult Hit;
                HasHits = GetWorld()->SweepSingleByObjectType(Hit, QueryStart, QueryEnd, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
                break;
            }
            case ECogEngine_CollisionQueryMode::Multi:
            {
                HasHits = GetWorld()->SweepMultiByObjectType(Hits, QueryStart, QueryEnd, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
                break;
            }
            case ECogEngine_CollisionQueryMode::Test:
            {
                HasHits = GetWorld()->SweepTestByObjectType(QueryStart, QueryEnd, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
                break;
            }
            }
            break;
        }

        case ECogEngine_CollisionQueryBy::Profile:
        {
            switch (Mode)
            {
            case  ECogEngine_CollisionQueryMode::Single:
            {
                FHitResult Hit;
                HasHits = GetWorld()->SweepSingleByProfile(Hit, QueryStart, QueryEnd, QueryRotation, ProfileName, QueryShape, QueryParams);
                if (HasHits)
                {
                    Hits.Add(Hit);
                }
                break;
            }
            case ECogEngine_CollisionQueryMode::Multi:
            {
                HasHits = GetWorld()->SweepMultiByProfile(Hits, QueryStart, QueryEnd, QueryRotation, ProfileName, QueryShape, QueryParams);
                break;
            }
            case ECogEngine_CollisionQueryMode::Test:
            {
                HasHits = GetWorld()->SweepTestByProfile(QueryStart, QueryEnd, QueryRotation, ProfileName, QueryShape, QueryParams);
                break;
            }
            }
            break;
        }
        }
        break;
    }
    }

    const FColor Color = HasHits ? FLinearColor(HitColor).ToFColor(true) : FLinearColor(NoHitColor).ToFColor(true);
    const bool bUseTrace = Type == ECogEngine_CollisionQueryType::LineTrace || Type == ECogEngine_CollisionQueryType::Sweep;
    if (bUseTrace)
    {
        DrawDebugDirectionalArrow(
            GetWorld(),
            QueryStart,
            QueryEnd,
            FCogDebug::Settings.ArrowSize,
            Color,
            false,
            0.0f,
            FCogDebug::GetDebugDepthPriority(0),
            FCogDebug::GetDebugThickness(0.0f));
    }

    if (bIsUsingShape)
    {
        DrawShape(QueryShape, QueryStart, QueryRotation, FVector::OneVector, Color, false);
    }

    for (const FOverlapResult& Overlap : Overlaps)
    {
        if (DrawHitPrimitives)
        {
            DrawPrimitive(Overlap.GetComponent());
        }
    }

    for (const FHitResult& Hit : Hits)
    {
        if (DrawHitLocations)
        {
            DrawDebugPoint(
                GetWorld(),
                Hit.Location,
                HitPointSize,
                Color,
                false,
                0.0f,
                FCogDebug::GetDebugDepthPriority(0));
        }

        if (DrawHitImpactPoints)
        {
            DrawDebugPoint(
                GetWorld(),
                Hit.ImpactPoint,
                HitPointSize,
                Color,
                false,
                0.0f,
                FCogDebug::GetDebugDepthPriority(0));
        }

        if (bIsUsingShape && DrawHitShapes)
        {
            DrawShape(QueryShape, Hit.Location, QueryRotation, FVector::OneVector, Color, false);
        }

        if (DrawHitNormals)
        {
            DrawDebugDirectionalArrow(
                GetWorld(),
                Hit.Location,
                Hit.Location + Hit.Normal * 20.0f,
                FCogDebug::Settings.ArrowSize,
                FLinearColor(NormalColor).ToFColor(true),
                false,
                0.0f,
                FCogDebug::GetDebugDepthPriority(0),
                FCogDebug::GetDebugThickness(0.0f));
        }

        if (DrawHitImpactNormals)
        {
            DrawDebugDirectionalArrow(
                GetWorld(),
                Hit.ImpactPoint,
                Hit.ImpactPoint + Hit.ImpactNormal * 20.0f,
                FCogDebug::Settings.ArrowSize,
                FLinearColor(ImpactNormalColor).ToFColor(true),
                false,
                0.0f,
                FCogDebug::GetDebugDepthPriority(0),
                FCogDebug::GetDebugThickness(0.0f));
        }

        if (DrawHitPrimitives)
        {
            DrawPrimitive(Hit.GetComponent());
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineCollisionTester::DrawPrimitive(const UPrimitiveComponent* PrimitiveComponent)
{
    //-------------------------------------------------------
    // Don't draw same primitives multiple times (for bones)
    //-------------------------------------------------------
    if (AlreadyDrawnComponents.Contains(PrimitiveComponent))
    {
        return;
    }

    AlreadyDrawnComponents.Add(PrimitiveComponent);

    ECollisionChannel CollisionObjectType = PrimitiveComponent->GetCollisionObjectType();
    FColor Color = FColor::Green; // Channels[CollisionObjectType].Color;

    //-------------------------------------------------------
    // Draw Name
    //-------------------------------------------------------
    if (DrawHitActorsNames)
    {
        const AActor* Actor = PrimitiveComponent->GetOwner();
        if (Actor != nullptr)
        {
            if (AlreadyDrawnActors.Contains(Actor) == false)
            {
                FColor TextColor = Color.WithAlpha(255);
                DrawDebugString(GetWorld(), Actor->GetActorLocation(), GetNameSafe(Actor->GetClass()), nullptr, FColor::White, 0.0f, FCogDebug::Settings.TextShadow, FCogDebug::Settings.TextSize);
                AlreadyDrawnActors.Add(Actor);
            }
        }
    }

    const FVector Location = PrimitiveComponent->GetComponentLocation();
    const FQuat Rotation = PrimitiveComponent->GetComponentQuat();
    const FVector Scale = PrimitiveComponent->GetComponentScale();
    const FCollisionShape PrimitiveShape = PrimitiveComponent->GetCollisionShape();
    DrawShape(PrimitiveShape, Location, Rotation, Scale, Color, true);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogEngineCollisionTester::DrawShape(const FCollisionShape& InShape, const FVector& InLocation, const FQuat& InRotation, const FVector& InScale, const FColor& InColor, bool InDrawSolid) const
{
    switch (InShape.ShapeType)
    {
    case ECollisionShape::Box:
    {
        //--------------------------------------------------
        // see UBoxComponent::GetScaledBoxExtent()
        //--------------------------------------------------
        const FVector HalfExtent(InShape.Box.HalfExtentX * InScale.X, InShape.Box.HalfExtentY * InScale.Y, InShape.Box.HalfExtentZ * InScale.Z);

        if (InDrawSolid)
        {
            DrawDebugSolidBox(
                GetWorld(),
                InLocation,
                HalfExtent,
                InRotation,
                InColor,
                false,
                0.0f,
                FCogDebug::GetDebugDepthPriority(0));
        }

        DrawDebugBox(
            GetWorld(),
            InLocation,
            HalfExtent,
            InRotation,
            InColor,
            false,
            0.0f,
            FCogDebug::GetDebugDepthPriority(0),
            FCogDebug::GetDebugThickness(0.0f));

        break;
    }

    case ECollisionShape::Sphere:
    {
        //--------------------------------------------------
        // see USphereComponent::GetScaledSphereRadius()
        //--------------------------------------------------
        const float RadiusScale = FMath::Min(InScale.X, FMath::Min(InScale.Y, InScale.Z));
        const float Radius = InShape.Sphere.Radius * RadiusScale;

        FCogDebugDrawHelper::DrawSphere(
            GetWorld(),
            InLocation,
            Radius,
            FCogDebug::GetCircleSegments(),
            InColor,
            false,
            0.0f,
            FCogDebug::GetDebugDepthPriority(0),
            FCogDebug::GetDebugThickness(0.0f));
        break;
    }

    case ECollisionShape::Capsule:
    {
        //--------------------------------------------------
        // see UCapsuleComponent::GetScaledCapsuleRadius()
        //--------------------------------------------------
        const float Radius = InShape.Capsule.Radius * FMath::Min(InScale.X, InScale.Y);
        const float HalfHeight = InShape.Capsule.HalfHeight * UE_REAL_TO_FLOAT(InScale.Z);

        DrawDebugCapsule(
            GetWorld(),
            InLocation,
            HalfHeight,
            Radius,
            InRotation,
            InColor,
            false,
            0.0f,
            FCogDebug::GetDebugDepthPriority(0),
            FCogDebug::GetDebugThickness(0.0f));
        break;
    }
    }
}
