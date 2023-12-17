#pragma once

#include "CoreMinimal.h"
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
enum class ECogEngine_CollisionQueryPlacement : uint8
{
    Selection,
    View,
    Cursor,
    Transform,
};

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
enum class ECogEngine_CollisionQueryCount : uint8
{
    Single,
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
UENUM()
enum class ECogEngine_GizmoType : uint8
{
    MoveAxis,
    MovePlane,
    Rotate,
    ScaleAxis,
    ScaleUniform,
    MAX,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogEngine_GizmoAxis : uint8
{
    X,
    Y,
    Z,
    MAX,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogEngine_GizmoElementType : uint8
{
    MoveX,
    MoveY,
    MoveZ,
    MoveXY,
    MoveXZ,
    MoveYZ,
    RotateX,
    RotateY,
    RotateZ,
    ScaleXYZ,
    ScaleX,
    ScaleY,
    ScaleZ,
    MAX,
};

//--------------------------------------------------------------------------------------------------------------------------
struct FCogEngine_GizmoElement
{
    ECogEngine_GizmoType Type;
    ECogEngine_GizmoAxis Axis;
    FQuat Rotation;
    FVector Location;
};

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

    void DrawTransformGizmos(const UWorld* InWorld, FTransform& InTransform);

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

    FTransform StartTransform;

};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_CollisionTester : public UCogWindowConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    ECogEngine_CollisionQueryPlacement Placement;

    UPROPERTY(Config)
    FVector3f LocationStart;

    UPROPERTY(Config)
    FVector3f LocationEnd;

    UPROPERTY(Config)
    FRotator3f Rotation;

    UPROPERTY(Config)
    ECogEngine_CollisionQueryType Type;

    UPROPERTY(Config)
    ECogEngine_CollisionQueryBy By;

    UPROPERTY(Config)
    ECogEngine_CollisionQueryShape Shape;

    UPROPERTY(Config)
    bool MultiHits;

    UPROPERTY(Config)
    bool TraceComplex;

    UPROPERTY(Config)
    int32 ObjectTypesToQuery;

    UPROPERTY(Config)
    FName Profile;

    UPROPERTY(Config)
    int32 Channel;

    UPROPERTY(Config)
    int32 ProfileIndex;

    UPROPERTY(Config)
    float SphereRadius;

    UPROPERTY(Config)
    FVector3f BoxExtent;

    UPROPERTY(Config)
    float CapsuleRadius;

    UPROPERTY(Config)
    float CapsuleHalfHeight;

    UPROPERTY(Config)
    int QueryTypeOld;

    UPROPERTY(Config)
    float QueryLength;

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

        Placement = ECogEngine_CollisionQueryPlacement::Selection;
        Type = ECogEngine_CollisionQueryType::LineTrace;
        By = ECogEngine_CollisionQueryBy::Channel;
        Shape = ECogEngine_CollisionQueryShape::Sphere;
        MultiHits = false;
        TraceComplex = false;
        SphereRadius = 50.0f;
        BoxExtent = FVector3f(50.0f, 50.0f, 50.0f);
        CapsuleRadius = 50.0f;
        CapsuleHalfHeight = 50.0f;

        ObjectTypesToQuery = 0;
        ProfileIndex = 0;
        QueryTypeOld = 0;
        QueryLength = 5000.0f;
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