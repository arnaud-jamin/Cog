#include "CogDebugGizmo.h"

#include "CogDebug.h"
#include "CogDebugDrawHelper.h"
#include "CogImGuiHelper.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/PlayerController.h"
#include "imgui.h"
#include "Kismet/GameplayStatics.h"

//--------------------------------------------------------------------------------------------------------------------------
float ScreenDistanceToLine(const APlayerController& InPlayerController, const FVector2D& Point2D, const FVector& P0, const FVector& P1)
{
    FVector2D ScreenP0, ScreenP1;
    UGameplayStatics::ProjectWorldToScreen(&InPlayerController, P0, ScreenP0);
    UGameplayStatics::ProjectWorldToScreen(&InPlayerController, P1, ScreenP1);
    const FVector2D ClosestPoint = FMath::ClosestPointOnSegment2D(Point2D, ScreenP0, ScreenP1);
    const float DistanceToMouse = (ClosestPoint - Point2D).Length();
    return DistanceToMouse;
}

//--------------------------------------------------------------------------------------------------------------------------
float ScreenDistanceToSphere(const APlayerController& InPlayerController, const FVector2D& Point2D, const FVector& SphereLocation, float Radius)
{
    FVector2D ElementLocation2D;
    UGameplayStatics::ProjectWorldToScreen(&InPlayerController, SphereLocation, ElementLocation2D);

    //TODO : radius in 3D
    const float Distance = FVector2D::Distance(ElementLocation2D, Point2D) - Radius;
    return Distance;
}

//--------------------------------------------------------------------------------------------------------------------------
float ScreenDistanceToQuad(const APlayerController& InPlayerController, const FVector2D& Point2D, const FVector& QuadPosition, const FQuat& QuadRotation, const FVector2D& QuadExtents)
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

    const FVector B0 = FMath::GetBaryCentric2D(Point2D, P0, P1, P2);
    const FVector B1 = FMath::GetBaryCentric2D(Point2D, P0, P2, P3);

    const bool Inside0 = B0.X > 0.0f && B0.Y > 0.0f && B0.Z > 0.0f;
    const bool Inside1 = B1.X > 0.0f && B1.Y > 0.0f && B1.Z > 0.0f;

    const float D = (Inside0 || Inside1) ? 0.0f : FLT_MAX;
    return D;
}

//--------------------------------------------------------------------------------------------------------------------------
float ScreenDistanceToArc(const APlayerController& InPlayerController, const FVector2D& Point, const FMatrix& Matrix, const float Radius, const float AngleStart, const float AngleEnd, const int32 Segments)
{
    const float AngleStartRad = FMath::DegreesToRadians(AngleStart);
    const float AngleEndRad = FMath::DegreesToRadians(AngleEnd);
    const FVector Center = Matrix.GetOrigin();

    // Need at least 4 segments
    const int32 NumSegments = FMath::Max(Segments, 4);

    const FVector AxisY = Matrix.GetScaledAxis(EAxis::Y);
    const FVector AxisZ = Matrix.GetScaledAxis(EAxis::Z);

    float CurrentAngle = AngleStartRad;
    const float AngleStep = (AngleEndRad - AngleStartRad) / float(NumSegments);

    FVector P0 = Center + Radius * (AxisZ * FMath::Sin(CurrentAngle) + AxisY * FMath::Cos(CurrentAngle));

	FVector2D ScreenP0;
    UGameplayStatics::ProjectWorldToScreen(&InPlayerController, P0, ScreenP0);

    float MinDistanceSqr = FLT_MAX;

    int32 Count = NumSegments;
    while (Count--)
    {
        CurrentAngle += AngleStep;
        const FVector P1 = Center + Radius * (AxisZ * FMath::Sin(CurrentAngle) + AxisY * FMath::Cos(CurrentAngle));

        FVector2D ScreenP1;
        UGameplayStatics::ProjectWorldToScreen(&InPlayerController, P1, ScreenP1);

        const FVector2D ClosestPoint = FMath::ClosestPointOnSegment2D(Point, ScreenP0, ScreenP1);
        const float ScreenDistanceSqr = (ClosestPoint - Point).SquaredLength();

        if (ScreenDistanceSqr < MinDistanceSqr)
        {
            MinDistanceSqr = ScreenDistanceSqr;
        }

        P0 = P1;
        ScreenP0 = ScreenP1;
    }

    return FMath::Sqrt(MinDistanceSqr);
}

//--------------------------------------------------------------------------------------------------------------------------
FVector GetMouseCursorOnLine(const APlayerController& InPlayerController, const FVector& LinePoint, const FVector& LineDirection, const FVector2D& MousePos)
{
    FVector MouseWorldOrigin, MouseWorldDirection;
    UGameplayStatics::DeprojectScreenToWorld(&InPlayerController, MousePos, MouseWorldOrigin, MouseWorldDirection);

    FVector PointOnAxis;
    FVector PointOnMouseRay;

    constexpr float LineLength = 10000;
    FMath::SegmentDistToSegmentSafe(LinePoint - LineDirection * LineLength, LinePoint + LineDirection * LineLength, MouseWorldOrigin - MouseWorldDirection * LineLength, MouseWorldOrigin + MouseWorldDirection * LineLength, PointOnAxis, PointOnMouseRay);

    return PointOnAxis;
}

//--------------------------------------------------------------------------------------------------------------------------
FVector GetMouseCursorOnPlane(const APlayerController& InPlayerController, const FVector& PlanePoint, const FVector& PlaneNormal, const FVector2D& MousePos)
{
    FVector MouseWorldOrigin, MouseWorldDirection;
    UGameplayStatics::DeprojectScreenToWorld(&InPlayerController, MousePos, MouseWorldOrigin, MouseWorldDirection);

    const FPlane Plane(PlanePoint, PlaneNormal);
    const FVector PlaneIntersection = FMath::RayPlaneIntersection(MouseWorldOrigin, MouseWorldDirection, Plane);
    return PlaneIntersection;
}

//--------------------------------------------------------------------------------------------------------------------------
FVector VectorMax(const FVector& Vector, const float MaxValue)
{
    FVector NewValue;
    NewValue.X = FMath::Max(Vector.X, MaxValue);
    NewValue.Y = FMath::Max(Vector.Y, MaxValue);
    NewValue.Z = FMath::Max(Vector.Z, MaxValue);
    return NewValue;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebug_Gizmo::Draw(const char* Id, const APlayerController& InPlayerController, FTransform& InOutTransform, ECogDebug_GizmoFlags Flags)
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
        if (DraggedElementType != ECogDebug_GizmoElementType::MAX)
        {
            InOutTransform = InitialTransform;
        }

        return;
    }

    const ImGuiIO& IO = ImGui::GetIO();
    FCogDebugSettings& Settings = FCogDebug::Settings;

    FVector ProjectedGizmoCenter, CenterDirection, AxisProjection, AxisDirection;
    UGameplayStatics::DeprojectScreenToWorld(&InPlayerController, Center2D, ProjectedGizmoCenter, CenterDirection);
    UGameplayStatics::DeprojectScreenToWorld(&InPlayerController, Center2D + FVector2d(0, 1.0f), AxisProjection, AxisDirection);
    const FVector CameraLocation = InPlayerController.PlayerCameraManager->GetCameraLocation();
    const float CameraToProjectedGizmoCenter = FVector::Dist(CameraLocation, ProjectedGizmoCenter);
    const float ScaleToKeepGizmoScreenSizeConstant = FMath::IsNearlyZero(CameraToProjectedGizmoCenter) ? 1.0f : FVector::Dist(AxisProjection, ProjectedGizmoCenter) * FVector::Dist(CameraLocation, GizmoCenter) / CameraToProjectedGizmoCenter;
    const float GizmoScale = Settings.GizmoScale * ScaleToKeepGizmoScreenSizeConstant;

    const FQuat RotX = UseLocalSpace ? InOutTransform.GetRotation() : FQuat(FVector(0.0f, 0.0f, 1.0f), 0.0f);
    const FQuat RotY = RotX * FQuat(FVector(0.0f, 0.0f,-1.0f), UE_HALF_PI);
    const FQuat RotZ = RotX * FQuat(FVector(0.0f, 1.0f, 0.0f), UE_HALF_PI);

    const FVector UnitAxisX = UseLocalSpace ? RotX.GetAxisX() : FVector::XAxisVector;
    const FVector UnitAxisY = UseLocalSpace ? RotX.GetAxisY() : FVector::YAxisVector;
    const FVector UnitAxisZ = UseLocalSpace ? RotX.GetAxisZ() : FVector::ZAxisVector;

    const FVector AxisX = GizmoCenter + UnitAxisX * Settings.GizmoAxisLength * GizmoScale;
    const FVector AxisY = GizmoCenter + UnitAxisY * Settings.GizmoAxisLength * GizmoScale;
    const FVector AxisZ = GizmoCenter + UnitAxisZ * Settings.GizmoAxisLength * GizmoScale;

    const FVector PlaneXY = GizmoCenter + ((UnitAxisX + UnitAxisY) * Settings.GizmoPlaneOffset * GizmoScale);
    const FVector PlaneXZ = GizmoCenter + ((UnitAxisX + UnitAxisZ) * Settings.GizmoPlaneOffset * GizmoScale);
    const FVector PlaneYZ = GizmoCenter + ((UnitAxisY + UnitAxisZ) * Settings.GizmoPlaneOffset * GizmoScale);

    const float ScaleBoxExtent = Settings.GizmoScaleBoxExtent * GizmoScale;
    const float PlaneExtent = Settings.GizmoPlaneExtent * GizmoScale;
    const float ThicknessZLow = Settings.GizmoThicknessZLow * ScaleToKeepGizmoScreenSizeConstant;
    const float ThicknessZHigh = Settings.GizmoThicknessZHigh * ScaleToKeepGizmoScreenSizeConstant;

    const ImVec2 ImMousePos = ImGui::GetMousePos() - Viewport->Pos;
    const FVector2D MousePos = FCogImguiHelper::ToFVector2D(ImMousePos);

    const FColor GizmoAxisColorsZLow[]      = { Settings.GizmoAxisColorsZLowX,         Settings.GizmoAxisColorsZLowY,         Settings.GizmoAxisColorsZLowZ,         Settings.GizmoAxisColorsZLowW };
    const FColor GizmoAxisColorsZHigh[]     = { Settings.GizmoAxisColorsZHighX,        Settings.GizmoAxisColorsZHighY,        Settings.GizmoAxisColorsZHighZ,        Settings.GizmoAxisColorsZHighW };
    const FColor GizmoAxisColorsSelection[] = { Settings.GizmoAxisColorsSelectionX,    Settings.GizmoAxisColorsSelectionY,    Settings.GizmoAxisColorsSelectionZ,    Settings.GizmoAxisColorsSelectionW };

    FCogDebug_GizmoElement GizmoElements[ECogDebug_GizmoElementType::MAX];
    for (FCogDebug_GizmoElement& GizmoElement : GizmoElements)
    {
        GizmoElement.Type = ECogDebug_GizmoType::MAX;
    }

    if (EnumHasAnyFlags(Flags, ECogDebug_GizmoFlags::NoTranslationAxis) == false)
    {
        GizmoElements[(uint8)ECogDebug_GizmoElementType::MoveX] = { ECogDebug_GizmoType::MoveAxis, ECogDebug_GizmoAxis::X, FVector::XAxisVector, UnitAxisX, FQuat::Identity, AxisX };
        GizmoElements[(uint8)ECogDebug_GizmoElementType::MoveY] = { ECogDebug_GizmoType::MoveAxis, ECogDebug_GizmoAxis::Y, FVector::YAxisVector, UnitAxisY, FQuat::Identity, AxisY };
        GizmoElements[(uint8)ECogDebug_GizmoElementType::MoveZ] = { ECogDebug_GizmoType::MoveAxis, ECogDebug_GizmoAxis::Z, FVector::ZAxisVector, UnitAxisZ, FQuat::Identity, AxisZ };
    }

    if (EnumHasAnyFlags(Flags, ECogDebug_GizmoFlags::NoTranslationPlane) == false)
    {
        GizmoElements[(uint8)ECogDebug_GizmoElementType::MoveXY] = { ECogDebug_GizmoType::MovePlane, ECogDebug_GizmoAxis::Z, FVector::ZAxisVector, UnitAxisZ, RotZ, PlaneXY };
        GizmoElements[(uint8)ECogDebug_GizmoElementType::MoveXZ] = { ECogDebug_GizmoType::MovePlane, ECogDebug_GizmoAxis::Y, FVector::YAxisVector, UnitAxisY, RotY, PlaneXZ };
        GizmoElements[(uint8)ECogDebug_GizmoElementType::MoveYZ] = { ECogDebug_GizmoType::MovePlane, ECogDebug_GizmoAxis::X, FVector::XAxisVector, UnitAxisX, RotX, PlaneYZ };
    }

    if (EnumHasAnyFlags(Flags, ECogDebug_GizmoFlags::NoRotation) == false)
    {
        GizmoElements[(uint8)ECogDebug_GizmoElementType::RotateX] = { ECogDebug_GizmoType::Rotate, ECogDebug_GizmoAxis::X, FVector::XAxisVector, UnitAxisX, RotX, AxisX };
        GizmoElements[(uint8)ECogDebug_GizmoElementType::RotateY] = { ECogDebug_GizmoType::Rotate, ECogDebug_GizmoAxis::Y, FVector::YAxisVector, UnitAxisY, RotY, AxisY };
        GizmoElements[(uint8)ECogDebug_GizmoElementType::RotateZ] = { ECogDebug_GizmoType::Rotate, ECogDebug_GizmoAxis::Z, FVector::ZAxisVector, UnitAxisZ, RotZ, AxisZ };
    }

    if (EnumHasAnyFlags(Flags, ECogDebug_GizmoFlags::NoScaleUniform) == false)
    {
        GizmoElements[(uint8)ECogDebug_GizmoElementType::ScaleXYZ] = { ECogDebug_GizmoType::ScaleUniform, ECogDebug_GizmoAxis::MAX, FVector::OneVector, FVector::OneVector, RotX, GizmoCenter };
    }

    if (EnumHasAnyFlags(Flags, ECogDebug_GizmoFlags::NoScaleAxis) == false)
    {
        GizmoElements[(uint8)ECogDebug_GizmoElementType::ScaleX] = { ECogDebug_GizmoType::ScaleAxis, ECogDebug_GizmoAxis::X, FVector::XAxisVector, UnitAxisX, RotX, AxisX };
        GizmoElements[(uint8)ECogDebug_GizmoElementType::ScaleY] = { ECogDebug_GizmoType::ScaleAxis, ECogDebug_GizmoAxis::Y, FVector::YAxisVector, UnitAxisY, RotY, AxisY };
        GizmoElements[(uint8)ECogDebug_GizmoElementType::ScaleZ] = { ECogDebug_GizmoType::ScaleAxis, ECogDebug_GizmoAxis::Z, FVector::ZAxisVector, UnitAxisZ, RotZ,                AxisZ };
    }

    ECogDebug_GizmoElementType HoveredElementType = ECogDebug_GizmoElementType::MAX;
    if (DraggedElementType != ECogDebug_GizmoElementType::MAX)
    {
        HoveredElementType = DraggedElementType;
    }
    else if (IO.WantCaptureMouse == false)
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
                    DistanceToMouse = ScreenDistanceToLine(InPlayerController, MousePos, GizmoCenter, Elm.Location);
                    break;
                }

                case ECogDebug_GizmoType::MovePlane:
                {
                    DistanceToMouse = ScreenDistanceToQuad(InPlayerController, MousePos, Elm.Location, Elm.Rotation, FVector2D(PlaneExtent, PlaneExtent));
                    break;
                }

                case ECogDebug_GizmoType::Rotate:
                {
                    FRotationTranslationMatrix Matrix(FRotator(Elm.Rotation), GizmoCenter);
                    const float RotationGizmoRadius = Settings.GizmoRotationRadius * GizmoScale;
                    DistanceToMouse = ScreenDistanceToArc(InPlayerController, MousePos, Matrix, RotationGizmoRadius, 0.0f, 90.0f, Settings.GizmoRotationSegments);
                    break;
                }

                case ECogDebug_GizmoType::ScaleUniform:
                case ECogDebug_GizmoType::ScaleAxis:
                {
                    DistanceToMouse = ScreenDistanceToSphere(InPlayerController, MousePos, Elm.Location, Settings.GizmoScaleBoxExtent);
                    break;
                }

            	default:;
            }

            if (DistanceToMouse < Settings.GizmoCursorSelectionThreshold && DistanceToMouse < MinDistanceToMouse)
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
                DrawDebugLine(World, GizmoCenter, Elm.Location, ZLowColor, false, 0.0f, Settings.GizmoZLow, ThicknessZLow);
                DrawDebugLine(World, GizmoCenter, Elm.Location, ZHighColor, false, 0.0f, Settings.GizmoZHigh, Settings.GizmoThicknessZHigh);
                break;
            }

            case ECogDebug_GizmoType::MovePlane:
            {
                FCogDebugDrawHelper::DrawQuad(World, Elm.Location, Elm.Rotation, FVector2D(PlaneExtent, PlaneExtent), ZLowColor, false, 0.0f, Settings.GizmoZLow, ThicknessZLow);
                FCogDebugDrawHelper::DrawQuad(World, Elm.Location, Elm.Rotation, FVector2D(PlaneExtent, PlaneExtent), ZHighColor, false, 0.0f, Settings.GizmoZHigh, ThicknessZHigh);
                FCogDebugDrawHelper::DrawSolidQuad(World, Elm.Location, Elm.Rotation, FVector2D(PlaneExtent, PlaneExtent), ZLowColor, false, 0.0f, Settings.GizmoZLow);
                break;
            }

            case ECogDebug_GizmoType::Rotate:
            {
                FRotationTranslationMatrix Matrix(FRotator(Elm.Rotation), GizmoCenter);
                const float RotationGizmoRadius = Settings.GizmoRotationRadius * GizmoScale;
                FCogDebugDrawHelper::DrawArc(World, Matrix, RotationGizmoRadius, RotationGizmoRadius, 0.0f, 90.0f, Settings.GizmoRotationSegments, ZLowColor, false, 0.0f, Settings.GizmoZLow, ThicknessZLow);
                FCogDebugDrawHelper::DrawArc(World, Matrix, RotationGizmoRadius, RotationGizmoRadius, 0.0f, 90.0f, Settings.GizmoRotationSegments, ZHighColor, false, 0.0f, Settings.GizmoZHigh, ThicknessZHigh);
                break;
            }

            case ECogDebug_GizmoType::ScaleUniform:
            case ECogDebug_GizmoType::ScaleAxis:
            {
                DrawDebugBox(World, Elm.Location, FVector::OneVector * ScaleBoxExtent, Elm.Rotation, ZLowColor, false, 0.0f, Settings.GizmoZLow, ThicknessZLow);
                DrawDebugBox(World, Elm.Location, FVector::OneVector * ScaleBoxExtent, Elm.Rotation, ZHighColor, false, 0.0f, Settings.GizmoZHigh, ThicknessZHigh);
                DrawDebugSolidBox(World, Elm.Location, FVector::OneVector * ScaleBoxExtent, Elm.Rotation, ZLowColor, false, 0.0f, Settings.GizmoZLow);
                break;
            }

			default:
                break;
        }
    }

    if (Settings.GizmoGroundRaycastLength > 0.0f)
    {
        FVector Bottom = GizmoCenter - FVector(0.0f, 0.0f, Settings.GizmoGroundRaycastLength);
        FHitResult GroundHit;
        if (World->LineTraceSingleByChannel(GroundHit, GizmoCenter, Bottom, Settings.GizmoGroundRaycastChannel))
        {
            Bottom = GroundHit.ImpactPoint;
            const FRotationTranslationMatrix Matrix(FRotator(90.0f, 0, 0), GroundHit.ImpactPoint);
            FCogDebugDrawHelper::DrawArc(World, Matrix, Settings.GizmoGroundRaycastCircleRadius, Settings.GizmoGroundRaycastCircleRadius, 0.0f, 360.0f, 24, Settings.GizmoGroundRaycastCircleColor, false, 0.0f, Settings.GizmoZLow, ThicknessZLow);
        }
		DrawDebugLine(World, GizmoCenter, Bottom, Settings.GizmoGroundRaycastColor, false, 0.0f, Settings.GizmoZLow, ThicknessZLow);
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        DraggedElementType = ECogDebug_GizmoElementType::MAX;
    }
    else if (DraggedElementType != ECogDebug_GizmoElementType::MAX)
    {
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            InOutTransform = InitialTransform;
            DraggedElementType = ECogDebug_GizmoElementType::MAX;
        }
        else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, Settings.GizmoCursorDraggingThreshold))
        {
            const FCogDebug_GizmoElement& DraggedElement = GizmoElements[(uint8)DraggedElementType];

            switch (DraggedElement.Type)
            {
                case ECogDebug_GizmoType::MoveAxis: 
                {
                    const FVector Point = GetMouseCursorOnLine(InPlayerController, InitialTransform.GetTranslation(), DraggedElement.Direction, MousePos - CursorOffset);
                    InOutTransform.SetLocation(Point);
                    break;
                }

                case ECogDebug_GizmoType::MovePlane:
                {
                    const FVector Point = GetMouseCursorOnPlane(InPlayerController, InitialTransform.GetTranslation(), DraggedElement.Direction, MousePos - CursorOffset);
                    InOutTransform.SetLocation(Point);
                    break;
                }

                case ECogDebug_GizmoType::Rotate:
                {
                    const FVector2D DragDelta = FCogImguiHelper::ToFVector2D(ImGui::GetMouseDragDelta(ImGuiMouseButton_Left));
                    const float DragAmount = DragDelta.X - DragDelta.Y;
                    const FQuat RotDelta(DraggedElement.Axis, DragAmount * Settings.GizmoScaleSpeed);

                    FQuat NewRot;
                	if (UseLocalSpace)
                    {
                        NewRot = InitialTransform.GetRotation() * RotDelta;

                    }
                    else
                    {
                        NewRot = RotDelta * InitialTransform.GetRotation();
                    }

                    InOutTransform.SetRotation(NewRot);
                    break;
                }

                case ECogDebug_GizmoType::ScaleAxis:
                {
                    const FVector Point = GetMouseCursorOnLine(InPlayerController, GizmoCenter, DraggedElement.Direction, MousePos - CursorOffset);
                    const float Sign = FMath::Sign(FVector::DotProduct(DraggedElement.Direction, Point - GizmoCenter));
                    const float Delta = (Point - GizmoCenter).Length() * Sign;
                    const FVector NewScale = VectorMax(InitialTransform.GetScale3D() + (DraggedElement.Axis * Delta * Settings.GizmoScaleSpeed), Settings.GizmoScaleMin);
                    InOutTransform.SetScale3D(NewScale);
                    break;
                }

                case ECogDebug_GizmoType::ScaleUniform:
                {
                    const FVector2D DragDelta = FCogImguiHelper::ToFVector2D(ImGui::GetMouseDragDelta(ImGuiMouseButton_Left));
                    const float DragAmount = DragDelta.X - DragDelta.Y;
                    const FVector NewScale = VectorMax(InitialTransform.GetScale3D() + (DraggedElement.Axis * DragAmount * Settings.GizmoScaleSpeed), Settings.GizmoScaleMin);
                    InOutTransform.SetScale3D(NewScale);
                    break;
                }

                default: 
                    break;
            }
            
        }
    }
    else if (HoveredElementType != ECogDebug_GizmoElementType::MAX)
    {
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
			DraggedElementType = HoveredElementType;
            CursorOffset = MousePos - Center2D;
            InitialTransform = InOutTransform;
        }
        else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup(Id);
        }
    }

    if (ImGui::BeginPopup(Id))
    {
        ImGui::Checkbox("Local Space", &UseLocalSpace);

        ImGui::Separator();

        FVector Translation = InOutTransform.GetTranslation();
        if (FCogImguiHelper::DragFVector("Translation", Translation, 1.0f, 0.0f, 0.0f, "%.1f"))
        {
            InOutTransform.SetTranslation(Translation);
        }

        //if (ImGui::MenuItem("Reset Translation"))
        //{
        //    InOutTransform.SetTranslation(FVector::ZeroVector);
        //}

        FRotator Rotation = FRotator(InOutTransform.GetRotation());
        if (FCogImguiHelper::DragFRotator("Rotation", Rotation, 1.0f, 0.0f, 0.0f, "%.1f"))
        {
            InOutTransform.SetRotation(Rotation.Quaternion());
        }

        //if (ImGui::MenuItem("Reset Rotation"))
        //{
        //    InOutTransform.SetRotation(FQuat::Identity);
        //}

        FVector Scale = InOutTransform.GetScale3D();
        if (FCogImguiHelper::DragFVector("Scale", Scale, 1.0f, 0.0f, FLT_MAX, "%.1f"))
        {
            InOutTransform.SetScale3D(Scale);
        }

        //if (ImGui::MenuItem("Reset Scale"))
        //{
        //    InOutTransform.SetScale3D(FVector::OneVector);
        //}

        ImGui::Separator();

        ImGui::DragFloat("Gizmo Scale", &Settings.GizmoScale, 0.1f, 0.1f, 10.0f, "%.1f");

        ImGui::EndPopup();
    }

}
