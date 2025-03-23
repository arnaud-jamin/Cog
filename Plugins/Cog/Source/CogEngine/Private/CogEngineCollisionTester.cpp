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
    const UWorld* World = GetWorld();
    
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
            case ECogEngine_CollisionQueryShape::Sphere: QueryShape.SetSphere(ShapeExtent.X);
                break;
            case ECogEngine_CollisionQueryShape::Capsule: QueryShape.SetCapsule(ShapeExtent.X, ShapeExtent.Z);
                break;
            case ECogEngine_CollisionQueryShape::Box: QueryShape.SetBox(FVector3f(ShapeExtent));
                break;
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
                    switch (OverlapMode)
                    {
                        case ECogEngine_CollisionQueryOverlapMode::AnyTest:
                            HasHits = World->OverlapAnyTestByChannel(QueryStart, QueryRotation, TraceChannel, QueryShape, QueryParams);
                            break;

                        case ECogEngine_CollisionQueryOverlapMode::BlockingTest:
                            HasHits = World->OverlapBlockingTestByChannel(QueryStart, QueryRotation, TraceChannel, QueryShape, QueryParams);
                            break;

                        case ECogEngine_CollisionQueryOverlapMode::Multi:
                            HasHits = World->OverlapMultiByChannel(Overlaps, QueryStart, QueryRotation, TraceChannel, QueryShape, QueryParams);
                            break;
                    }
                    break;
                }

                case ECogEngine_CollisionQueryBy::ObjectType:
                {
                    FCollisionObjectQueryParams QueryObjectParams;
                    QueryObjectParams.ObjectTypesToQuery = ObjectTypesToQuery;

                    switch (OverlapMode)
                    {
                        case ECogEngine_CollisionQueryOverlapMode::AnyTest:
                            HasHits = World->OverlapAnyTestByObjectType(QueryStart, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
                            break;

                        case ECogEngine_CollisionQueryOverlapMode::BlockingTest:
                            break;

                        case ECogEngine_CollisionQueryOverlapMode::Multi:
                            HasHits = World->OverlapMultiByObjectType(Overlaps, QueryStart, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
                            break;
                    }
                    break;
                }

                case ECogEngine_CollisionQueryBy::Profile:
                {
                    switch (OverlapMode)
                    {
                        case ECogEngine_CollisionQueryOverlapMode::AnyTest:
                            HasHits = World->OverlapAnyTestByProfile(QueryStart, QueryRotation, ProfileName, QueryShape, QueryParams);
                            break;

                        case ECogEngine_CollisionQueryOverlapMode::BlockingTest:
                            HasHits = World->OverlapBlockingTestByProfile(QueryStart, QueryRotation, ProfileName, QueryShape, QueryParams);
                            break;

                        case ECogEngine_CollisionQueryOverlapMode::Multi:
                            HasHits = World->OverlapMultiByProfile(Overlaps, QueryStart, QueryRotation, ProfileName, QueryShape, QueryParams);
                            break;
                    }
                    break;
                }
            }

            FCogDebugDrawOverlapParams DrawParams;
            FCogDebug::GetDebugDrawOverlapSettings(DrawParams);
            FCogDebugDrawHelper::DrawOverlap(World, QueryShape, QueryStart, QueryRotation, HasHits, Overlaps, DrawParams);
            break;
        }

        case ECogEngine_CollisionQueryType::LineTrace:
        {
            TArray<FHitResult> Hits;
            switch (By)
            {
                case ECogEngine_CollisionQueryBy::Channel:
                {
                    switch (TraceMode)
                    {
                        case ECogEngine_CollisionQueryTraceMode::Single:
                        {
                            FHitResult Hit;
                            HasHits = World->LineTraceSingleByChannel(Hit, QueryStart, QueryEnd, TraceChannel, QueryParams);
                            if (HasHits)
                            {
                                Hits.Add(Hit);
                            }
                            break;
                        }
                        case ECogEngine_CollisionQueryTraceMode::Multi:
                        {
                            HasHits = World->LineTraceMultiByChannel(Hits, QueryStart, QueryEnd, TraceChannel, QueryParams);
                            break;
                        }
                        case ECogEngine_CollisionQueryTraceMode::Test:
                        {
                            HasHits = World->LineTraceTestByChannel(QueryStart, QueryEnd, TraceChannel, QueryParams);
                            break;
                        }
                    }
                    break;
                }

                case ECogEngine_CollisionQueryBy::ObjectType:
                {
                    FCollisionObjectQueryParams QueryObjectParams;
                    QueryObjectParams.ObjectTypesToQuery = ObjectTypesToQuery;

                    switch (TraceMode)
                    {
                        case ECogEngine_CollisionQueryTraceMode::Single:
                        {
                            FHitResult Hit;
                            HasHits = World->LineTraceSingleByObjectType(Hit, QueryStart, QueryEnd, QueryObjectParams, QueryParams);
                            if (HasHits)
                            {
                                Hits.Add(Hit);
                            }
                            break;
                        }
                        case ECogEngine_CollisionQueryTraceMode::Multi:
                        {
                            HasHits = World->LineTraceMultiByObjectType(Hits, QueryStart, QueryEnd, QueryObjectParams, QueryParams);
                            break;
                        }
                        case ECogEngine_CollisionQueryTraceMode::Test:
                        {
                            HasHits = World->LineTraceTestByObjectType(QueryStart, QueryEnd, QueryObjectParams, QueryParams);
                            break;
                        }
                    }
                    break;
                }

                case ECogEngine_CollisionQueryBy::Profile:
                {
                    switch (TraceMode)
                    {
                        case ECogEngine_CollisionQueryTraceMode::Single:
                        {
                            FHitResult Hit;
                            HasHits = World->LineTraceSingleByProfile(Hit, QueryStart, QueryEnd, ProfileName, QueryParams);
                            if (HasHits)
                            {
                                Hits.Add(Hit);
                            }
                            break;
                        }
                        case ECogEngine_CollisionQueryTraceMode::Multi:
                        {
                            HasHits = World->LineTraceMultiByProfile(Hits, QueryStart, QueryEnd, ProfileName, QueryParams);
                            break;
                        }
                        case ECogEngine_CollisionQueryTraceMode::Test:
                        {
                            HasHits = World->LineTraceTestByProfile(QueryStart, QueryEnd, ProfileName, QueryParams);
                            break;
                        }
                    }
                    break;
                }
            }

            FCogDebugDrawLineTraceParams DrawParams;
            FCogDebug::GetDebugDrawLineTraceSettings(DrawParams);
            FCogDebugDrawHelper::DrawLineTrace(World, QueryStart, QueryEnd, HasHits, Hits, DrawParams);
            break;
        }

        case ECogEngine_CollisionQueryType::Sweep:
        {
            TArray<FHitResult> Hits;
            switch (By)
            {
                case ECogEngine_CollisionQueryBy::Channel:
                {
                    switch (TraceMode)
                    {
                        case ECogEngine_CollisionQueryTraceMode::Single:
                        {
                            FHitResult Hit;
                            HasHits = World->SweepSingleByChannel(Hit, QueryStart, QueryEnd, QueryRotation, TraceChannel, QueryShape, QueryParams);
                            if (HasHits)
                            {
                                Hits.Add(Hit);
                            }
                            break;
                        }
                        case ECogEngine_CollisionQueryTraceMode::Multi:
                        {
                            HasHits = World->SweepMultiByChannel(Hits, QueryStart, QueryEnd, QueryRotation, TraceChannel, QueryShape, QueryParams);
                            break;
                        }
                        case ECogEngine_CollisionQueryTraceMode::Test:
                        {
                            HasHits = World->SweepTestByChannel(QueryStart, QueryEnd, QueryRotation, TraceChannel, QueryShape, QueryParams);
                            break;
                        }
                    }
                    break;
                }

                case ECogEngine_CollisionQueryBy::ObjectType:
                {
                    FCollisionObjectQueryParams QueryObjectParams;
                    QueryObjectParams.ObjectTypesToQuery = ObjectTypesToQuery;

                    switch (TraceMode)
                    {
                        case ECogEngine_CollisionQueryTraceMode::Single:
                        {
                            FHitResult Hit;
                            HasHits = World->SweepSingleByObjectType(Hit, QueryStart, QueryEnd, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
                            if (HasHits)
                            {
                                Hits.Add(Hit);
                            }
                            break;
                        }
                        case ECogEngine_CollisionQueryTraceMode::Multi:
                        {
                            HasHits = World->SweepMultiByObjectType(Hits, QueryStart, QueryEnd, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
                            break;
                        }
                        case ECogEngine_CollisionQueryTraceMode::Test:
                        {
                            HasHits = World->SweepTestByObjectType(QueryStart, QueryEnd, QueryRotation, QueryObjectParams, QueryShape, QueryParams);
                            break;
                        }
                    }
                    break;
                }

                case ECogEngine_CollisionQueryBy::Profile:
                {
                    switch (TraceMode)
                    {
                        case ECogEngine_CollisionQueryTraceMode::Single:
                        {
                            FHitResult Hit;
                            HasHits = World->SweepSingleByProfile(Hit, QueryStart, QueryEnd, QueryRotation, ProfileName, QueryShape, QueryParams);
                            if (HasHits)
                            {
                                Hits.Add(Hit);
                            }
                            break;
                        }
                        case ECogEngine_CollisionQueryTraceMode::Multi:
                        {
                            HasHits = World->SweepMultiByProfile(Hits, QueryStart, QueryEnd, QueryRotation, ProfileName, QueryShape, QueryParams);
                            break;
                        }
                        case ECogEngine_CollisionQueryTraceMode::Test:
                        {
                            HasHits = World->SweepTestByProfile(QueryStart, QueryEnd, QueryRotation, ProfileName, QueryShape, QueryParams);
                            break;
                        }
                    }
                    break;
                }
            }

            FCogDebugDrawSweepParams DrawParams;
            FCogDebug::GetDebugDrawSweepSettings(DrawParams);
            FCogDebugDrawHelper::DrawSweep(World, QueryShape, QueryStart, QueryEnd, QueryRotation, HasHits, Hits, DrawParams);
            break;
        }
    }
}
