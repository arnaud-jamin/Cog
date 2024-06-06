#include "CogEngineCollisionTester.h"

#include "CogDebugDrawHelper.h"
#include "CogDebug.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"

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
void ACogEngineCollisionTester::Query() const
{
    FVector QueryStart = StartComponent->GetComponentLocation();
    FVector QueryEnd = EndComponent->GetComponentLocation();
    FQuat QueryRotation = StartComponent->GetComponentQuat();
    bool HasHits = false;
    
    static const FName TraceTag(TEXT("FCogWindow_Collision"));
    const FCollisionQueryParams QueryParams(TraceTag, SCENE_QUERY_STAT_ONLY(CogHitDetection), TraceComplex);

    const FCollisionResponseTemplate* Profile = UCollisionProfile::Get()->GetProfileByIndex(ProfileIndex);
    const FName ProfileName = Profile != nullptr ? Profile->Name : FName();

    FCollisionShape QueryShape;
    if (Type == ECogEngine_CollisionQueryType::Overlap || Type == ECogEngine_CollisionQueryType::Sweep)
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
			TArray<FOverlapResult> Overlaps;
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

			FCogDebugDrawOverlapParams DrawParams;
			FCogDebug::GetDebugDrawOverlapSettings(DrawParams);
			FCogDebugDrawHelper::DrawOverlap(GetWorld(), QueryShape, QueryStart, QueryRotation, Overlaps, DrawParams);
	        break;
	    }

	    case ECogEngine_CollisionQueryType::LineTrace:
	    {
			TArray<FHitResult> Hits;
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

			FCogDebugDrawLineTraceParams DrawParams;
			FCogDebug::GetDebugDrawLineTraceSettings(DrawParams);
			FCogDebugDrawHelper::DrawLineTrace(GetWorld(), QueryStart, QueryEnd, HasHits, Hits, DrawParams);
			break;
	    }

	    case ECogEngine_CollisionQueryType::Sweep:
	    {
			TArray<FHitResult> Hits;
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

			FCogDebugDrawSweepParams DrawParams;
			FCogDebug::GetDebugDrawSweepSettings(DrawParams);
			FCogDebugDrawHelper::DrawSweep(GetWorld(), QueryShape, QueryStart, QueryEnd, QueryRotation, HasHits, Hits, DrawParams);
			break;
	    }
    }
}
