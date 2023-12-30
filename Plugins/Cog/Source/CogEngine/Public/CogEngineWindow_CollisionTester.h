#pragma once

#include "CoreMinimal.h"
#include "CogDebugGizmo.h"
#include "CogWindow.h"
#include "CogWindowConfig.h"
#include "Engine/HitResult.h"
#include "CogEngineWindow_CollisionTester.generated.h"

class UCogEngineConfig_CollisionViewer;
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


enum class ECogDebug_GizmoTransformSpace : uint8;

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_CollisionTester : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void ResetConfig() override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void SetAsset(const UCogEngineDataAsset* Value);

    void Query();

    void DrawPrimitive(const UPrimitiveComponent* PrimitiveComponent);

    void DrawShape(const FCollisionShape& Shape, const FVector& Location, const FQuat& Rotation, const FVector& Scale, const FColor& Color, bool DrawSolid) const;


    struct FChannel
    {
        bool IsValid = false;
        FColor Color;
    };

    FChannel Channels[ECC_MAX];

    TObjectPtr<const UCogEngineDataAsset> Asset = nullptr;

    TObjectPtr<UCogEngineConfig_CollisionTester> Config = nullptr;

    TSet<const AActor*> AlreadyDrawnActors;

    TSet<const UPrimitiveComponent*> AlreadyDrawnComponents;

    bool IsDragging = false;

    FCogDebug_Gizmo GizmoStart;
    FCogDebug_Gizmo GizmoEnd;

    FTransform StartTransform;

};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_CollisionTester : public UCogWindowConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    FVector LocationStart;

    UPROPERTY(Config)
    FVector LocationEnd;

    UPROPERTY(Config)
    FRotator Rotation;

    UPROPERTY(Config)
    FVector Scale;

    UPROPERTY(Config)
    ECogEngine_CollisionQueryType Type;

    UPROPERTY(Config)
    ECogEngine_CollisionQueryMode Mode;

    UPROPERTY(Config)
    ECogEngine_CollisionQueryBy By;

    UPROPERTY(Config)
    ECogEngine_CollisionQueryShape Shape;

    UPROPERTY(Config)
    bool TraceComplex;

    UPROPERTY(Config)
    int32 ObjectTypesToQuery;

    UPROPERTY(Config)
    TEnumAsByte<ECollisionChannel> Channel;

    UPROPERTY(Config)
    int32 ProfileIndex;

    UPROPERTY(Config)
    FVector ShapeExtent;

    UPROPERTY(Config)
    bool DrawHitLocations;

    UPROPERTY(Config)
    bool DrawHitImpactPoints;

    UPROPERTY(Config)
    bool DrawHitShapes;

    UPROPERTY(Config)
    bool DrawHitNormals;

    UPROPERTY(Config)
    bool DrawHitImpactNormals;

    UPROPERTY(Config)
    bool DrawHitPrimitives;

    UPROPERTY(Config)
    bool DrawHitActorsNames;

    UPROPERTY(Config)
    float HitPointSize;

    UPROPERTY(Config)
    FVector4f NoHitColor;

    UPROPERTY(Config)
    FVector4f HitColor;

    UPROPERTY(Config)
    FVector4f NormalColor;

    UPROPERTY(Config)
    FVector4f ImpactNormalColor;

    UCogEngineConfig_CollisionTester()
    {
        Reset();
    }

    virtual void Reset() override
    {
        Super::Reset();

        Type = ECogEngine_CollisionQueryType::LineTrace;
        By = ECogEngine_CollisionQueryBy::Channel;
        Mode = ECogEngine_CollisionQueryMode::Multi;
        Channel = ECC_WorldStatic;
        TraceComplex = false;
        Shape = ECogEngine_CollisionQueryShape::Sphere;
        ShapeExtent = FVector(50.0f, 50.0f, 50.0f);

        ObjectTypesToQuery = 0;
        ProfileIndex = 0;
        DrawHitLocations = true;
        DrawHitImpactPoints = true;
        DrawHitShapes = true;
        DrawHitNormals = true;
        DrawHitImpactNormals = true;
        DrawHitPrimitives = true;
        DrawHitActorsNames = false;
        HitPointSize = 10.0f;
        NoHitColor = FVector4f(1.0f, 0.0f, 0.0f, 1.0f);
        HitColor = FVector4f(0.0f, 1.0f, 0.0f, 1.0f);
        NormalColor = FVector4f(0.0f, 1.0f, 0.0f, 1.0f);
        ImpactNormalColor = FVector4f(0.0f, 1.0f, 0.0f, 1.0f);
    }
};