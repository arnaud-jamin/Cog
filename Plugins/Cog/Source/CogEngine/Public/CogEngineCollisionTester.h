#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "GameFramework/Actor.h"
#include "CogEngineCollisionTester.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogEngine_CollisionQueryType : uint8
{
    Overlap,
    LineTrace,
    Sweep,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogEngine_CollisionQueryTraceMode : uint8
{
    Single,
    Multi,
    Test,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogEngine_CollisionQueryOverlapMode : uint8
{
    AnyTest,
    BlockingTest,
    Multi,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogEngine_CollisionQueryBy : uint8
{
    Channel,
    ObjectType,
    Profile,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogEngine_CollisionQueryShape : uint8
{
    Sphere,
    Box,
    Capsule,
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS()
class COGENGINE_API ACogEngineCollisionTester : public AActor
{
    GENERATED_BODY()

public:
    ACogEngineCollisionTester(const FObjectInitializer& ObjectInitializer);

    virtual void Tick(float DeltaSeconds) override;

    virtual bool ShouldTickIfViewportsOnly() const override;

    void Query() const;

    UPROPERTY(EditAnywhere)
    bool TickInEditor = false;

    UPROPERTY(EditAnywhere)
    ECogEngine_CollisionQueryType Type = ECogEngine_CollisionQueryType::LineTrace;

    UPROPERTY(EditAnywhere)
    ECogEngine_CollisionQueryTraceMode TraceMode = ECogEngine_CollisionQueryTraceMode::Multi;

    UPROPERTY(EditAnywhere)
    ECogEngine_CollisionQueryOverlapMode OverlapMode = ECogEngine_CollisionQueryOverlapMode::Multi;

    UPROPERTY(EditAnywhere)
    ECogEngine_CollisionQueryBy By = ECogEngine_CollisionQueryBy::Channel;

    UPROPERTY(EditAnywhere)
    ECogEngine_CollisionQueryShape Shape = ECogEngine_CollisionQueryShape::Sphere;

    UPROPERTY(EditAnywhere)
    bool TraceComplex = false;

    UPROPERTY()
    int32 ObjectTypesToQuery = 0;

    UPROPERTY(EditAnywhere)
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

    UPROPERTY()
    int32 ProfileIndex = 0;

    UPROPERTY(EditAnywhere)
    FVector ShapeExtent = FVector(100, 100, 100);

    UPROPERTY(EditAnywhere)
    bool DrawHitLocations = true;

    UPROPERTY(EditAnywhere)
    bool DrawHitImpactPoints = true;

    UPROPERTY(EditAnywhere)
    bool DrawHitShapes = true;

    UPROPERTY(EditAnywhere)
    bool DrawHitNormals = true;

    UPROPERTY(EditAnywhere)
    bool DrawHitImpactNormals = true;

    UPROPERTY(EditAnywhere)
    bool DrawHitPrimitives = true;

    UPROPERTY(EditAnywhere)
    bool DrawHitActorsNames = false;

    UPROPERTY(EditAnywhere)
    float HitPointSize = 5.0f;

    UPROPERTY(EditAnywhere)
    FColor NoHitColor = FColor::Red;

    UPROPERTY(EditAnywhere)
    FColor HitColor = FColor::Green;

    UPROPERTY(EditAnywhere)
    FColor NormalColor = FColor::Yellow;

    UPROPERTY(EditAnywhere)
    FColor ImpactNormalColor = FColor::Cyan;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USceneComponent> StartComponent;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USceneComponent> EndComponent;
};
