#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogDebugGizmo.h"
#include "CogWindow.h"
#include "Engine/HitResult.h"
#include "CogEngineCollisionTester.h"
#include "CogEngineWindow_CollisionTester.generated.h"

class UCogEngineConfig_CollisionViewer;
class UPrimitiveComponent;
struct FCollisionShape;

enum class ECogDebug_GizmoTransformSpace : uint8;

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_CollisionTester : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    struct FChannel
    {
        bool IsValid = false;
        FColor Color;
    };

    TObjectPtr<UCogEngineConfig_CollisionTester> Config = nullptr;

    FChannel Channels[ECC_MAX] = {};

    FCogDebug_Gizmo StartGizmo;
    
    FCogDebug_Gizmo EndGizmo;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_CollisionTester : public UCogCommonConfig
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
    ECogEngine_CollisionQueryTraceMode Mode;

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

    virtual void Reset() override
    {
        Super::Reset();

        Type = ECogEngine_CollisionQueryType::LineTrace;
        By = ECogEngine_CollisionQueryBy::Channel;
        Mode = ECogEngine_CollisionQueryTraceMode::Multi;
        Channel = ECC_WorldStatic;
        TraceComplex = false;
        Shape = ECogEngine_CollisionQueryShape::Sphere;
        ShapeExtent = FVector(50.0f, 50.0f, 50.0f);

        ObjectTypesToQuery = 0;
        ProfileIndex = 0;
    }
};