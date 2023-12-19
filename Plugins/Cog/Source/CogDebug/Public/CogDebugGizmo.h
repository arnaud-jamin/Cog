#pragma once

#include "CoreMinimal.h"
#include "CogDebugGizmo.generated.h"

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
    void Draw(const APlayerController& InPlayerController, FTransform& InOutTransform);

    ECogDebug_GizmoElementType DraggedElementType = ECogDebug_GizmoElementType::MAX;
    FVector2D CursorOffset = FVector2D::ZeroVector;
    FVector InitialScale = FVector::OneVector;
};
