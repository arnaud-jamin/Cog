#pragma once

#include "CoreMinimal.h"
#include "Engine/HitResult.h"
#include "CogEngineCollisionTester.generated.h"

class UCogEngineEditAnywhere_CollisionViewer;
class UCogEngineDataAsset;
class UPrimitiveComponent;
struct FCollisionShape;

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
enum class ECogEngine_CollisionQueryMode : uint8
{
    Single,
    Multi,
    Test,
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

    void Query();
    void DrawPrimitive(const UPrimitiveComponent* PrimitiveComponent);
    void DrawShape(const FCollisionShape& Shape, const FVector& InLocation, const FQuat& InRotation, const FVector& InScale,
                   const FColor& InColor, bool InDrawSolid) const;

    UPROPERTY(EditAnywhere)
    bool TickInEditor = false;

    UPROPERTY(EditAnywhere)
    ECogEngine_CollisionQueryType Type = ECogEngine_CollisionQueryType::LineTrace;

    UPROPERTY(EditAnywhere)
    ECogEngine_CollisionQueryMode Mode = ECogEngine_CollisionQueryMode::Multi;

    UPROPERTY(EditAnywhere)
    ECogEngine_CollisionQueryBy By = ECogEngine_CollisionQueryBy::Channel;

    UPROPERTY(EditAnywhere)
    ECogEngine_CollisionQueryShape Shape = ECogEngine_CollisionQueryShape::Sphere;

    UPROPERTY(EditAnywhere)
    bool TraceComplex = false;

    UPROPERTY()
    int32 ObjectTypesToQuery = 0;

    UPROPERTY(EditAnywhere)
    TEnumAsByte<ECollisionChannel> Channel = ECC_WorldStatic;

    UPROPERTY()
    int32 ProfileIndex = 0;

    UPROPERTY(EditAnywhere)
    FVector ShapeExtent = FVector::One();

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

    TSet<const AActor*> AlreadyDrawnActors;

    TSet<const UPrimitiveComponent*> AlreadyDrawnComponents;

    UPROPERTY(EditAnywhere)
    USceneComponent* StartComponent = nullptr;

    UPROPERTY(EditAnywhere)
    USceneComponent* EndComponent = nullptr;
};
