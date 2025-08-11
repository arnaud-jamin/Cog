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

    UPROPERTY(EditAnywhere, Category = "Cog")
    bool TickInEditor = false;

    UPROPERTY(EditAnywhere, Category = "Cog")
    ECogEngine_CollisionQueryType Type = ECogEngine_CollisionQueryType::LineTrace;

    UPROPERTY(EditAnywhere, Category = "Cog")
    ECogEngine_CollisionQueryTraceMode TraceMode = ECogEngine_CollisionQueryTraceMode::Multi;

    UPROPERTY(EditAnywhere, Category = "Cog")
    ECogEngine_CollisionQueryOverlapMode OverlapMode = ECogEngine_CollisionQueryOverlapMode::Multi;

    UPROPERTY(EditAnywhere, Category = "Cog")
    ECogEngine_CollisionQueryBy By = ECogEngine_CollisionQueryBy::Channel;

    UPROPERTY(EditAnywhere, Category = "Cog")
    ECogEngine_CollisionQueryShape Shape = ECogEngine_CollisionQueryShape::Sphere;

    UPROPERTY(EditAnywhere, Category = "Cog")
    bool TraceComplex = false;

    UPROPERTY()
    int32 ObjectTypesToQuery = 0;

    UPROPERTY(EditAnywhere, Category = "Cog")
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

    UPROPERTY()
    int32 ProfileIndex = 0;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FVector ShapeExtent = FVector(100, 100, 100);

    UPROPERTY(EditAnywhere, Category = "Cog")
    bool DrawHitLocations = true;

    UPROPERTY(EditAnywhere, Category = "Cog")
    bool DrawHitImpactPoints = true;

    UPROPERTY(EditAnywhere, Category = "Cog")
    bool DrawHitShapes = true;

    UPROPERTY(EditAnywhere, Category = "Cog")
    bool DrawHitNormals = true;

    UPROPERTY(EditAnywhere, Category = "Cog")
    bool DrawHitImpactNormals = true;

    UPROPERTY(EditAnywhere, Category = "Cog")
    bool DrawHitPrimitives = true;

    UPROPERTY(EditAnywhere, Category = "Cog")
    bool DrawHitActorsNames = false;

    UPROPERTY(EditAnywhere, Category = "Cog")
    float HitPointSize = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FColor NoHitColor = FColor::Red;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FColor HitColor = FColor::Green;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FColor NormalColor = FColor::Yellow;

    UPROPERTY(EditAnywhere, Category = "Cog")
    FColor ImpactNormalColor = FColor::Cyan;

    UPROPERTY(EditAnywhere, Category = "Cog")
    TObjectPtr<USceneComponent> StartComponent;

    UPROPERTY(EditAnywhere, Category = "Cog")
    TObjectPtr<USceneComponent> EndComponent;
};