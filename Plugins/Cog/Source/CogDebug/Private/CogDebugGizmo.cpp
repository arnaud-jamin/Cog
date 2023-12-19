#include "CogDebugGizmo.h"

#include "CogDebugDrawHelper.h"
#include "CogDebugSettings.h"
#include "CogImGuiHelper.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/PlayerController.h"
#include "imgui.h"
#include "Kismet/GameplayStatics.h"

//--------------------------------------------------------------------------------------------------------------------------
float PointDistanceToQuad(const APlayerController& InPlayerController, const FVector2D& Point, const FVector& QuadPosition, const FQuat& QuadRotation, const FVector2D& QuadExtents)
{
    const FVector U = QuadRotation.GetAxisZ() * QuadExtents.X;
    const FVector V = QuadRotation.GetAxisY() * QuadExtents.Y;

    const FVector V0 = QuadPosition + U + V;
    const FVector V1 = QuadPosition + U - V;
    const FVector V2 = QuadPosition - U - V;
    const FVector V3 = QuadPosition - U + V;

    FVector2D P0, P1, P2, P3;
    UGameplayStatics::ProjectWorldToScreen(&InPlayerController, V0, P0);
    UGameplayStatics::ProjectWorldToScreen(&InPlayerController, V1, P1);
    UGameplayStatics::ProjectWorldToScreen(&InPlayerController, V2, P2);
    UGameplayStatics::ProjectWorldToScreen(&InPlayerController, V3, P3);

    const FVector B0 = FMath::GetBaryCentric2D(Point, P0, P1, P2);
    const FVector B1 = FMath::GetBaryCentric2D(Point, P0, P2, P3);

    const bool Inside0 = B0.X > 0.0f && B0.Y > 0.0f && B0.Z > 0.0f;
    const bool Inside1 = B1.X > 0.0f && B1.Y > 0.0f && B1.Z > 0.0f;

    const float D = (Inside0 || Inside1) ? 0.0f : FLT_MAX;
    return D;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebug_Gizmo::Draw(const APlayerController& InPlayerController, FTransform& InOutTransform)
{
    UWorld* World = InPlayerController.GetWorld();

    const ImGuiViewport* Viewport = ImGui::GetMainViewport();
    if (Viewport == nullptr)
    {
        return;
    }

    const FVector GizmoCenter = InOutTransform.GetTranslation();

    FVector2D Center2D;
    if (UGameplayStatics::ProjectWorldToScreen(&InPlayerController, GizmoCenter, Center2D) == false)
    {
        return;
    }

    const FCogDebugData& Debug = FCogDebugSettings::Data;

    FVector CenterProjection, CenterDirection, AxisProjection, AxisDirection;
    UGameplayStatics::DeprojectScreenToWorld(&InPlayerController, Center2D, CenterProjection, CenterDirection);
    UGameplayStatics::DeprojectScreenToWorld(&InPlayerController, Center2D + FVector2d(0, 1.0f), AxisProjection, AxisDirection);
    const FVector CameraLocation = InPlayerController.PlayerCameraManager->GetCameraLocation();
    const float Sc = FVector::Dist(AxisProjection, CenterProjection) * FVector::Dist(CameraLocation, GizmoCenter) / FVector::Dist(CameraLocation, CenterProjection);
    const float Scale = Debug.GizmoScale * Sc;

    const FQuat RotX = InOutTransform.GetRotation();
    const FQuat RotY = RotX * FQuat(FVector(0.0f, 0.0f, 1.0f), UE_HALF_PI);
    const FQuat RotZ = RotX * FQuat(FVector(0.0f, 1.0f, 0.0f), UE_HALF_PI);

    const FVector UnitAxisX = RotX.GetAxisX();
    const FVector UnitAxisY = RotX.GetAxisY();
    const FVector UnitAxisZ = RotX.GetAxisZ();

    const FVector AxisX = GizmoCenter + UnitAxisX * Debug.GizmoAxisLength * Scale;
    const FVector AxisY = GizmoCenter + UnitAxisY * Debug.GizmoAxisLength * Scale;
    const FVector AxisZ = GizmoCenter + UnitAxisZ * Debug.GizmoAxisLength * Scale;

    const FVector PlaneXY = GizmoCenter + ((UnitAxisX + UnitAxisY) * Debug.GizmoPlaneOffset * Scale);
    const FVector PlaneXZ = GizmoCenter + ((UnitAxisX + UnitAxisZ) * Debug.GizmoPlaneOffset * Scale);
    const FVector PlaneYZ = GizmoCenter + ((UnitAxisY + UnitAxisZ) * Debug.GizmoPlaneOffset * Scale);

    const float AdjustedScaleBoxExtent = Debug.GizmoScaleBoxExtent * Scale;
    const float AdjustedPlaneExtent = Debug.GizmoPlaneExtent * Scale;
    const float AdjustedThicknessZLow = Debug.GizmoThicknessZLow * Scale;
    const float AdjustedThicknessZHigh = Debug.GizmoThicknessZHigh * Scale;

    const ImVec2 ImMousePos = ImGui::GetMousePos() - Viewport->Pos;
    const FVector2D MousePos = FCogImguiHelper::ToFVector2D(ImMousePos);

    const FColor GizmoAxisColorsZLow[]      = { Debug.GizmoAxisColorsZLowX,         Debug.GizmoAxisColorsZLowY,         Debug.GizmoAxisColorsZLowZ,         Debug.GizmoAxisColorsZLowW };
    const FColor GizmoAxisColorsZHigh[]     = { Debug.GizmoAxisColorsZHighX,        Debug.GizmoAxisColorsZHighY,        Debug.GizmoAxisColorsZHighZ,        Debug.GizmoAxisColorsZHighW };
    const FColor GizmoAxisColorsSelection[] = { Debug.GizmoAxisColorsSelectionX,    Debug.GizmoAxisColorsSelectionY,    Debug.GizmoAxisColorsSelectionZ,    Debug.GizmoAxisColorsSelectionW };

    FCogDebug_GizmoElement GizmoElements[ECogDebug_GizmoElementType::MAX];
    GizmoElements[(uint8)ECogDebug_GizmoElementType::MoveX]    = { ECogDebug_GizmoType::MoveAxis,       ECogDebug_GizmoAxis::X,    FVector::XAxisVector,     UnitAxisX,            FQuat::Identity,    AxisX };
    GizmoElements[(uint8)ECogDebug_GizmoElementType::MoveY]    = { ECogDebug_GizmoType::MoveAxis,       ECogDebug_GizmoAxis::Y,    FVector::YAxisVector,     UnitAxisY,            FQuat::Identity,    AxisY };
    GizmoElements[(uint8)ECogDebug_GizmoElementType::MoveZ]    = { ECogDebug_GizmoType::MoveAxis,       ECogDebug_GizmoAxis::Z,    FVector::ZAxisVector,     UnitAxisZ,            FQuat::Identity,    AxisZ };
    GizmoElements[(uint8)ECogDebug_GizmoElementType::MoveXY]   = { ECogDebug_GizmoType::MovePlane,      ECogDebug_GizmoAxis::Z,    FVector::ZAxisVector,     UnitAxisZ,            RotZ,               PlaneXY };
    GizmoElements[(uint8)ECogDebug_GizmoElementType::MoveXZ]   = { ECogDebug_GizmoType::MovePlane,      ECogDebug_GizmoAxis::Y,    FVector::YAxisVector,     UnitAxisY,            RotY,               PlaneXZ };
    GizmoElements[(uint8)ECogDebug_GizmoElementType::MoveYZ]   = { ECogDebug_GizmoType::MovePlane,      ECogDebug_GizmoAxis::X,    FVector::XAxisVector,     UnitAxisX,            RotX,               PlaneYZ };
    GizmoElements[(uint8)ECogDebug_GizmoElementType::RotateX]  = { ECogDebug_GizmoType::Rotate,         ECogDebug_GizmoAxis::X,    FVector::XAxisVector,     UnitAxisX,            RotX,               AxisX };
    GizmoElements[(uint8)ECogDebug_GizmoElementType::RotateY]  = { ECogDebug_GizmoType::Rotate,         ECogDebug_GizmoAxis::Y,    FVector::YAxisVector,     UnitAxisY,            RotY,               AxisY };
    GizmoElements[(uint8)ECogDebug_GizmoElementType::RotateZ]  = { ECogDebug_GizmoType::Rotate,         ECogDebug_GizmoAxis::Z,    FVector::ZAxisVector,     UnitAxisZ,            RotZ,               AxisZ };
    GizmoElements[(uint8)ECogDebug_GizmoElementType::ScaleXYZ] = { ECogDebug_GizmoType::ScaleUniform,   ECogDebug_GizmoAxis::MAX,  FVector::OneVector,       FVector::OneVector,   RotX,               GizmoCenter };
    GizmoElements[(uint8)ECogDebug_GizmoElementType::ScaleX]   = { ECogDebug_GizmoType::ScaleAxis,      ECogDebug_GizmoAxis::X,    FVector::XAxisVector,     UnitAxisX,            RotX,               AxisX };
    GizmoElements[(uint8)ECogDebug_GizmoElementType::ScaleY]   = { ECogDebug_GizmoType::ScaleAxis,      ECogDebug_GizmoAxis::Y,    FVector::YAxisVector,     UnitAxisY,            RotY,               AxisY };
    GizmoElements[(uint8)ECogDebug_GizmoElementType::ScaleZ]   = { ECogDebug_GizmoType::ScaleAxis,      ECogDebug_GizmoAxis::Z,    FVector::ZAxisVector,     UnitAxisZ,            RotZ,               AxisZ };

    ECogDebug_GizmoElementType HoveredElementType = ECogDebug_GizmoElementType::MAX;
    if (DraggedElementType != ECogDebug_GizmoElementType::MAX)
    {
        HoveredElementType = DraggedElementType;
    }
    else
    {
        float MinDistanceToMouse = FLT_MAX;
        for (uint8 i = (uint8)ECogDebug_GizmoElementType::MoveX; i < (uint8)ECogDebug_GizmoElementType::MAX; ++i)
        {
            FCogDebug_GizmoElement& Elm = GizmoElements[i];
            float DistanceToMouse = FLT_MAX;

            switch (Elm.Type)
            {
                case ECogDebug_GizmoType::MoveAxis:
                {
                    FVector2D ElementLocation2D;
                    UGameplayStatics::ProjectWorldToScreen(&InPlayerController, Elm.Location, ElementLocation2D);
                    const FVector2D ClosestPoint = FMath::ClosestPointOnSegment2D(MousePos, Center2D, ElementLocation2D);
                    DistanceToMouse = (ClosestPoint - MousePos).Length();
                    break;
                }

                case ECogDebug_GizmoType::MovePlane:
                {
                    DistanceToMouse = PointDistanceToQuad(InPlayerController, MousePos, Elm.Location, Elm.Rotation, FVector2D(AdjustedPlaneExtent, AdjustedPlaneExtent));
                    break;
                }

                case ECogDebug_GizmoType::ScaleUniform:
                case ECogDebug_GizmoType::ScaleAxis:
                {
                    FVector2D ElementLocation2D;
                    UGameplayStatics::ProjectWorldToScreen(&InPlayerController, Elm.Location, ElementLocation2D);
                    DistanceToMouse = FVector2D::Distance(ElementLocation2D, MousePos) - Debug.GizmoScaleBoxExtent;
                    break;
                }
            }

            if (DistanceToMouse < Debug.GizmoMouseMaxDistance && DistanceToMouse < MinDistanceToMouse)
            {
                HoveredElementType = (ECogDebug_GizmoElementType)i;
                MinDistanceToMouse = DistanceToMouse;
            }
        }
    }

    for (uint8 i = (uint8)ECogDebug_GizmoElementType::MoveX; i < (uint8)ECogDebug_GizmoElementType::MAX; ++i)
    {
        const FCogDebug_GizmoElement& Elm = GizmoElements[i];
        const bool IsClosestToMouse = i == (uint8)HoveredElementType;
        const uint8 AxisIndex = (uint8)Elm.AxisType;
        const FColor ZLowColor = IsClosestToMouse ? GizmoAxisColorsSelection[AxisIndex] : GizmoAxisColorsZLow[AxisIndex];
        const FColor ZHighColor = IsClosestToMouse ? GizmoAxisColorsSelection[AxisIndex] : GizmoAxisColorsZHigh[AxisIndex];

        switch (Elm.Type)
        {
            case ECogDebug_GizmoType::MoveAxis:
            {
                DrawDebugLine(World, GizmoCenter, Elm.Location, ZLowColor, false, 0.0f, Debug.GizmoZLow, AdjustedThicknessZLow);
                DrawDebugLine(World, GizmoCenter, Elm.Location, ZHighColor, false, 0.0f, Debug.GizmoZHigh, Debug.GizmoThicknessZHigh);
                break;
            }

            case ECogDebug_GizmoType::MovePlane:
            {
                FCogDebugDrawHelper::DrawQuad(World, Elm.Location, Elm.Rotation, FVector2D(AdjustedPlaneExtent, AdjustedPlaneExtent), ZLowColor, false, 0.0f, Debug.GizmoZLow, AdjustedThicknessZLow);
                FCogDebugDrawHelper::DrawQuad(World, Elm.Location, Elm.Rotation, FVector2D(AdjustedPlaneExtent, AdjustedPlaneExtent), ZHighColor, false, 0.0f, Debug.GizmoZHigh, AdjustedThicknessZHigh);
                FCogDebugDrawHelper::DrawSolidQuad(World, Elm.Location, Elm.Rotation, FVector2D(AdjustedPlaneExtent, AdjustedPlaneExtent), ZLowColor, false, 0.0f, Debug.GizmoZLow);
                break;
            }

            case ECogDebug_GizmoType::Rotate:
            {
                FRotationTranslationMatrix Matrix(FRotator(Elm.Rotation), Elm.Location);
                DrawDebugCircle(World, Matrix, Debug.GizmoRotationRadius, Debug.GizmoRotationSegments, ZLowColor, false, 0.0f, Debug.GizmoZLow, AdjustedThicknessZLow, false);
                DrawDebugCircle(World, Matrix, Debug.GizmoRotationRadius, Debug.GizmoRotationSegments, ZHighColor, false, 0.0f, Debug.GizmoZHigh, AdjustedThicknessZHigh, false);
                break;
            }

            case ECogDebug_GizmoType::ScaleUniform:
            case ECogDebug_GizmoType::ScaleAxis:
            {
                DrawDebugBox(World, Elm.Location, FVector::OneVector * AdjustedScaleBoxExtent, Elm.Rotation, ZLowColor, false, 0.0f, Debug.GizmoZLow, AdjustedThicknessZLow);
                DrawDebugBox(World, Elm.Location, FVector::OneVector * AdjustedScaleBoxExtent, Elm.Rotation, ZHighColor, false, 0.0f, Debug.GizmoZHigh, AdjustedThicknessZHigh);
                DrawDebugSolidBox(World, Elm.Location, FVector::OneVector * AdjustedScaleBoxExtent, Elm.Rotation, ZLowColor, false, 0.0f, Debug.GizmoZLow);
                break;
            }
        }
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        DraggedElementType = ECogDebug_GizmoElementType::MAX;
    }
    else if (DraggedElementType != ECogDebug_GizmoElementType::MAX)
    {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 4.0f))
        {
            FVector MouseWorldOrigin, MouseWorldDirection;
            UGameplayStatics::DeprojectScreenToWorld(&InPlayerController, MousePos - CursorOffset, MouseWorldOrigin, MouseWorldDirection);

            FCogDebug_GizmoElement& DraggedElement = GizmoElements[(uint8)DraggedElementType];

            switch (DraggedElement.Type)
            {
                case ECogDebug_GizmoType::MoveAxis: 
                {
                    FVector Pa, Pb;
                    FMath::SegmentDistToSegmentSafe(
                        GizmoCenter - DraggedElement.Direction * 10000, 
                        GizmoCenter + DraggedElement.Direction * 10000, 
                        MouseWorldOrigin - MouseWorldDirection * 10000,
                        MouseWorldOrigin + MouseWorldDirection * 10000, 
                        Pa, Pb);
                    InOutTransform.SetLocation(Pa);
                    break;
                }

                case ECogDebug_GizmoType::MovePlane:
                {
                    const FPlane Plane(GizmoCenter, DraggedElement.Direction);
                    const FVector PlaneIntersection = FMath::RayPlaneIntersection(MouseWorldOrigin, MouseWorldDirection, Plane);
                    InOutTransform.SetLocation(PlaneIntersection);
                    break;
                }

                case ECogDebug_GizmoType::ScaleAxis:
                {
                    FVector Pa, Pb;
                    FMath::SegmentDistToSegmentSafe(
                        GizmoCenter - DraggedElement.Direction * 10000,
                        GizmoCenter + DraggedElement.Direction * 10000,
                        MouseWorldOrigin - MouseWorldDirection * 10000,
                        MouseWorldOrigin + MouseWorldDirection * 10000,
                        Pa, Pb);

                    const float Sign = FMath::Sign(FVector::DotProduct(DraggedElement.Direction, Pa - GizmoCenter));
                    const float Delta = (Pa - GizmoCenter).Length() * Sign;
                    FVector NewScale = InitialScale + (DraggedElement.Axis * Delta * Debug.GizmoScaleSpeed);
                    NewScale.X = FMath::Max(NewScale.X, Debug.GizmoScaleMin);
                    NewScale.Y = FMath::Max(NewScale.Y, Debug.GizmoScaleMin);
                    NewScale.Z = FMath::Max(NewScale.Z, Debug.GizmoScaleMin);
                    InOutTransform.SetScale3D(NewScale);
                    break;
                }

                case ECogDebug_GizmoType::ScaleUniform:
                {
                    const FVector2D DragDelta = FCogImguiHelper::ToFVector2D(ImGui::GetMouseDragDelta(ImGuiMouseButton_Left));
                    FVector NewScale = InitialScale + (DraggedElement.Axis * (DragDelta.X - DragDelta.Y) * Debug.GizmoScaleSpeed);
                    NewScale.X = FMath::Max(NewScale.X, Debug.GizmoScaleMin);
                    NewScale.Y = FMath::Max(NewScale.Y, Debug.GizmoScaleMin);
                    NewScale.Z = FMath::Max(NewScale.Z, Debug.GizmoScaleMin);
                    InOutTransform.SetScale3D(NewScale);
                    break;
                }

                default: 
                    break;
            }
            
        }
    }
    else if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        DraggedElementType = HoveredElementType;

        if (HoveredElementType != ECogDebug_GizmoElementType::MAX)
        {
            CursorOffset = MousePos - Center2D;
            InitialScale = InOutTransform.GetScale3D();
        }
    }
}
