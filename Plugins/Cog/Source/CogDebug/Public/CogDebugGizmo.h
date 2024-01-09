#pragma once

#include "CoreMinimal.h"
#include "CogDebugGizmo.generated.h"

class APlayerController;
class USceneComponent;

//--------------------------------------------------------------------------------------------------------------------------
UENUM(Flags)
enum class ECogDebug_GizmoFlags : uint8
{
    None                    = 0,
    NoTranslationAxis       = 1 << 0,
    NoTranslationPlane      = 1 << 1,
    NoRotation              = 1 << 2,
    NoScaleAxis             = 1 << 3,
    NoScaleUniform          = 1 << 4,

    NoTranslation           = NoTranslationAxis | NoTranslationPlane,
    NoScale                 = NoScaleAxis | NoScaleUniform,
};
ENUM_CLASS_FLAGS(ECogDebug_GizmoFlags);

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogDebug_GizmoType : uint8
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
enum class ECogDebug_GizmoAxis : uint8
{
    X,
    Y,
    Z,
    MAX,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogDebug_GizmoElementType : uint8
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
struct FCogDebug_GizmoElement
{
    ECogDebug_GizmoType Type;
    ECogDebug_GizmoAxis AxisType;
    FVector Axis;
    FVector Direction;
    FQuat Rotation;
    FVector Location;
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebug_Gizmo
{
    bool Draw(const char* Id, const APlayerController& InPlayerController, USceneComponent& SceneComponent, ECogDebug_GizmoFlags Flags = ECogDebug_GizmoFlags::None);

    bool Draw(const char* Id, const APlayerController& InPlayerController, FTransform& InOutTransform, ECogDebug_GizmoFlags Flags = ECogDebug_GizmoFlags::None);

    ECogDebug_GizmoElementType DraggedElementType = ECogDebug_GizmoElementType::MAX;

    FVector2D CursorOffset = FVector2D::ZeroVector;

    FTransform InitialTransform = FTransform::Identity;
};
