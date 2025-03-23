#include "CogDebugGizmo.h"

#include "CogDebug.h"
#include "CogDebugDrawHelper.h"
#include "CogImguiHelper.h"
#include "DrawDebugHelpers.h"
#include "imgui.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
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
    const float AngleStep = (AngleEndRad - AngleStartRad) / static_cast<float>(NumSegments);

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
void DrawGizmoText(const ImVec2& Position, ImU32 Color, const char* Text)
{
    ImGuiViewport* Viewport = ImGui::GetMainViewport();
    if (Viewport == nullptr)
    {
        return;
    }

    ImDrawList* DrawList = ImGui::GetBackgroundDrawList(Viewport);

    const float Alpha = ImGui::ColorConvertU32ToFloat4(Color).w;
    DrawList->AddText(Viewport->Pos + Position + ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, Alpha)), Text, nullptr);
    DrawList->AddText(Viewport->Pos + Position, Color, Text, nullptr);
}

//--------------------------------------------------------------------------------------------------------------------------
bool RenderComponent(const char* Label, double* Value, double Reset)
{
    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-1);
    bool Result = FCogImguiHelper::DragDouble(Label, Value, 0.1f, 0.0f, 0.0f, "%.1f");
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
        *Value = Reset;
        Result = true;
    }
    ImGui::PopItemWidth();
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
void RenderSnap(bool* SnapEnable, float* Snap)
{
    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-1);
    ImGui::Checkbox("Snap", SnapEnable);
    ImGui::PopItemWidth();

    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-1);
    ImGui::DragFloat("##SnapValue", Snap, 0.1f, 0.1f, 1000.0f, "%.1f");
    ImGui::PopItemWidth();
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebug_Gizmo::Draw(const char* Id, const APlayerController& InPlayerController, USceneComponent& SceneComponent, ECogDebug_GizmoFlags Flags)
{
    FTransform Transform = SceneComponent.GetComponentTransform();
    if (Draw(Id, InPlayerController, Transform, Flags))
    {
        SceneComponent.SetWorldTransform(Transform);
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebug_Gizmo::Draw(const char* Id, const APlayerController& InPlayerController, FTransform& InOutTransform, ECogDebug_GizmoFlags Flags)
{
    UWorld* World = InPlayerController.GetWorld();

    const ImGuiViewport* Viewport = ImGui::GetMainViewport();
    if (Viewport == nullptr)
    {
        return false;
    }

    const FVector GizmoCenter = InOutTransform.GetTranslation();

    FVector2D Center2D;
    if (UGameplayStatics::ProjectWorldToScreen(&InPlayerController, GizmoCenter, Center2D) == false)
    {
        if (DraggedElementType != ECogDebug_GizmoElementType::MAX)
        {
            InOutTransform = InitialTransform;
            return true;
        }

        return false;
    }

    bool Result = false;

    const ImGuiIO& IO = ImGui::GetIO();
    FCogDebugSettings& Settings = FCogDebug::Settings;

    FVector ProjectedGizmoCenter, CenterDirection, AxisProjection, AxisDirection;
    UGameplayStatics::DeprojectScreenToWorld(&InPlayerController, Center2D, ProjectedGizmoCenter, CenterDirection);
    UGameplayStatics::DeprojectScreenToWorld(&InPlayerController, Center2D + FVector2d(0, 1.0f), AxisProjection, AxisDirection);
    const FVector CameraLocation = InPlayerController.PlayerCameraManager->GetCameraLocation();
    const float CameraToProjectedGizmoCenter = FVector::Dist(CameraLocation, ProjectedGizmoCenter);
    const float ScaleToKeepGizmoScreenSizeConstant = FMath::IsNearlyZero(CameraToProjectedGizmoCenter) ? 1.0f : FVector::Dist(AxisProjection, ProjectedGizmoCenter) * FVector::Dist(CameraLocation, GizmoCenter) / CameraToProjectedGizmoCenter;
    const float GizmoScale = Settings.GizmoScale * ScaleToKeepGizmoScreenSizeConstant;

    const FQuat RotX = Settings.GizmoUseLocalSpace ? InOutTransform.GetRotation() : FQuat(FVector(0.0f, 0.0f, 1.0f), 0.0f);
    const FQuat RotY = RotX * FQuat(FVector(0.0f, 0.0f, -1.0f), UE_HALF_PI);
    const FQuat RotZ = RotX * FQuat(FVector(0.0f, 1.0f, 0.0f), UE_HALF_PI);

    const FVector UnitAxisX = Settings.GizmoUseLocalSpace ? RotX.GetAxisX() : FVector::XAxisVector;
    const FVector UnitAxisY = Settings.GizmoUseLocalSpace ? RotX.GetAxisY() : FVector::YAxisVector;
    const FVector UnitAxisZ = Settings.GizmoUseLocalSpace ? RotX.GetAxisZ() : FVector::ZAxisVector;

    const float ScaleBoxExtent = Settings.GizmoScaleBoxExtent * GizmoScale;
    const float PlaneExtent = Settings.GizmoTranslationPlaneExtent * GizmoScale;
    const float ThicknessZLow = Settings.GizmoThicknessZLow * ScaleToKeepGizmoScreenSizeConstant;
    const float ThicknessZHigh = Settings.GizmoThicknessZHigh * ScaleToKeepGizmoScreenSizeConstant;

    const ImVec2 ImMousePos = ImGui::GetMousePos() - Viewport->Pos;
    const FVector2D MousePos = FCogImguiHelper::ToFVector2D(ImMousePos);

    const FColor GizmoAxisColorsZLow[]  = {Settings.GizmoAxisColorsZLowX, Settings.GizmoAxisColorsZLowY, Settings.GizmoAxisColorsZLowZ, Settings.GizmoAxisColorsZLowW};
    const FColor GizmoAxisColorsZHigh[] = {Settings.GizmoAxisColorsZHighX, Settings.GizmoAxisColorsZHighY, Settings.GizmoAxisColorsZHighZ, Settings.GizmoAxisColorsZHighW};
    const FColor GizmoAxisColorsSelection[] = {Settings.GizmoAxisColorsSelectionX, Settings.GizmoAxisColorsSelectionY, Settings.GizmoAxisColorsSelectionZ, Settings.GizmoAxisColorsSelectionW};

    FCogDebug_GizmoElement GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::MAX)];
    for (FCogDebug_GizmoElement& GizmoElement : GizmoElements)
    {
        GizmoElement.Type = ECogDebug_GizmoType::MAX;
    }

    if (EnumHasAnyFlags(Flags, ECogDebug_GizmoFlags::NoTranslationAxis) == false)
    {
        GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::MoveX)] = {ECogDebug_GizmoType::MoveAxis, ECogDebug_GizmoAxis::X, FVector::XAxisVector, UnitAxisX, RotX, GizmoCenter + UnitAxisX * Settings.GizmoTranslationAxisLength * GizmoScale};
        GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::MoveY)] = {ECogDebug_GizmoType::MoveAxis, ECogDebug_GizmoAxis::Y, FVector::YAxisVector, UnitAxisY, RotY, GizmoCenter + UnitAxisY * Settings.GizmoTranslationAxisLength * GizmoScale};
        GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::MoveZ)] = {ECogDebug_GizmoType::MoveAxis, ECogDebug_GizmoAxis::Z, FVector::ZAxisVector, UnitAxisZ, RotZ, GizmoCenter + UnitAxisZ * Settings.GizmoTranslationAxisLength * GizmoScale};
    }

    if (EnumHasAnyFlags(Flags, ECogDebug_GizmoFlags::NoTranslationPlane) == false)
    {
        GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::MoveXY)] = {ECogDebug_GizmoType::MovePlane, ECogDebug_GizmoAxis::Z, FVector::ZAxisVector, UnitAxisZ, RotZ, GizmoCenter + ((UnitAxisX + UnitAxisY) * Settings.GizmoTranslationPlaneOffset * GizmoScale)};
        GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::MoveXZ)] = {ECogDebug_GizmoType::MovePlane, ECogDebug_GizmoAxis::Y, FVector::YAxisVector, UnitAxisY, RotY, GizmoCenter + ((UnitAxisX + UnitAxisZ) * Settings.GizmoTranslationPlaneOffset * GizmoScale)};
        GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::MoveYZ)] = {ECogDebug_GizmoType::MovePlane, ECogDebug_GizmoAxis::X, FVector::XAxisVector, UnitAxisX, RotX, GizmoCenter + ((UnitAxisY + UnitAxisZ) * Settings.GizmoTranslationPlaneOffset * GizmoScale)};
    }

    if (EnumHasAnyFlags(Flags, ECogDebug_GizmoFlags::NoRotation) == false)
    {
        GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::RotateX)] = {ECogDebug_GizmoType::Rotate, ECogDebug_GizmoAxis::X, FVector::XAxisVector, UnitAxisX, RotX, FVector::ZeroVector};
        GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::RotateY)] = {ECogDebug_GizmoType::Rotate, ECogDebug_GizmoAxis::Y, FVector::YAxisVector, UnitAxisY, RotY, FVector::ZeroVector};
        GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::RotateZ)] = {ECogDebug_GizmoType::Rotate, ECogDebug_GizmoAxis::Z, FVector::ZAxisVector, UnitAxisZ, RotZ, FVector::ZeroVector};
    }

    if (EnumHasAnyFlags(Flags, ECogDebug_GizmoFlags::NoScaleUniform) == false)
    {
        GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::ScaleXYZ)] = {ECogDebug_GizmoType::ScaleUniform, ECogDebug_GizmoAxis::MAX, FVector::OneVector, FVector::OneVector, RotX, GizmoCenter};
    }

    if (EnumHasAnyFlags(Flags, ECogDebug_GizmoFlags::NoScaleAxis) == false)
    {
        GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::ScaleX)] = {ECogDebug_GizmoType::ScaleAxis, ECogDebug_GizmoAxis::X, FVector::XAxisVector, UnitAxisX, RotX, GizmoCenter + UnitAxisX * Settings.GizmoScaleBoxOffset * GizmoScale};
        GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::ScaleY)] = {ECogDebug_GizmoType::ScaleAxis, ECogDebug_GizmoAxis::Y, FVector::YAxisVector, UnitAxisY, RotY, GizmoCenter + UnitAxisY * Settings.GizmoScaleBoxOffset * GizmoScale};
        GizmoElements[static_cast<uint8>(ECogDebug_GizmoElementType::ScaleZ)] = {ECogDebug_GizmoType::ScaleAxis, ECogDebug_GizmoAxis::Z, FVector::ZAxisVector, UnitAxisZ, RotZ, GizmoCenter + UnitAxisZ * Settings.GizmoScaleBoxOffset * GizmoScale};
    }

    ECogDebug_GizmoElementType HoveredElementType = ECogDebug_GizmoElementType::MAX;
    if (DraggedElementType != ECogDebug_GizmoElementType::MAX)
    {
        HoveredElementType = DraggedElementType;
    }
    else if (IO.WantCaptureMouse == false)
    {
        float MinDistanceToMouse = FLT_MAX;
        for (uint8 i = static_cast<uint8>(ECogDebug_GizmoElementType::MoveX); i < static_cast<uint8>(ECogDebug_GizmoElementType::MAX); ++i)
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
                    const FRotationTranslationMatrix Matrix(FRotator(Elm.Rotation), GizmoCenter);
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

                default: ;
            }

            if (DistanceToMouse < Settings.GizmoCursorSelectionThreshold && DistanceToMouse < MinDistanceToMouse)
            {
                HoveredElementType = static_cast<ECogDebug_GizmoElementType>(i);
                MinDistanceToMouse = DistanceToMouse;
            }
        }
    }

    for (uint8 i = static_cast<uint8>(ECogDebug_GizmoElementType::MoveX); i < static_cast<uint8>(ECogDebug_GizmoElementType::MAX); ++i)
    {
        const FCogDebug_GizmoElement& Elm = GizmoElements[i];
        const bool IsClosestToMouse = i == static_cast<uint8>(HoveredElementType);
        const uint8 AxisIndex = static_cast<uint8>(Elm.AxisType);
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
            const FCogDebug_GizmoElement& DraggedElement = GizmoElements[static_cast<uint8>(DraggedElementType)];

            switch (DraggedElement.Type)
            {
                case ECogDebug_GizmoType::MoveAxis:
                {
                    const FVector CursorOnLine = GetMouseCursorOnLine(InPlayerController, InitialTransform.GetTranslation(), DraggedElement.Direction, MousePos - CursorOffset);
                    const float Delta = FVector::DotProduct(DraggedElement.Direction, CursorOnLine - InitialTransform.GetTranslation());
                    const float SnappedDelta = FMath::GridSnap(Delta, Settings.GizmoTranslationSnapEnable ? Settings.GizmoTranslationSnapValue : 0.0f);
                    const FVector NewLocation = InitialTransform.GetTranslation() + DraggedElement.Direction * SnappedDelta;
                    InOutTransform.SetLocation(NewLocation);
                    Result = true;

                    const FString Text = FString::Printf(TEXT("%+0.1f"), SnappedDelta);
                    DrawGizmoText(FCogImguiHelper::ToImVec2(Center2D), FCogImguiHelper::ToImU32(Settings.GizmoTextColor), StringCast<ANSICHAR>(*Text).Get());

                    //DrawDebugPoint(World, InitialTransform.GetTranslation(), 5.0f, FColor::White);
                    //DrawDebugLine(World, InitialTransform.GetTranslation(), InitialTransform.GetTranslation() + DraggedElement.Direction * SnappedDelta, FColor::White);
                    //const FVector TextLocation = InitialTransform.GetTranslation() + DraggedElement.Direction * SnappedDelta * 0.5f;
                    //UGameplayStatics::DeprojectScreenToWorld(&InPlayerController, Center2D + FVector2d(0, 1.0f), AxisProjection, AxisDirection);
                    //FVector2D TextLocation2D;
                    //if (UGameplayStatics::ProjectWorldToScreen(&InPlayerController, TextLocation, TextLocation2D))
                    //{
                    //    FCogDebugDrawImGui::AddText(FCogImguiHelper::ToImVec2(TextLocation2D), FString::Printf(TEXT("%0.1f"), SnappedDelta), IM_COL32(255, 255, 255, 255), true);
                    //}
                    break;
                }

                case ECogDebug_GizmoType::MovePlane:
                {
                    const FVector CursorOnPlane = GetMouseCursorOnPlane(InPlayerController, InitialTransform.GetTranslation(), DraggedElement.Direction, MousePos - CursorOffset);
                    const FVector U = DraggedElement.Rotation.GetAxisZ();
                    const FVector V = DraggedElement.Rotation.GetAxisY();
                    const float DeltaU = FVector::DotProduct(U, CursorOnPlane - InitialTransform.GetTranslation());
                    const float DeltaV = FVector::DotProduct(V, CursorOnPlane - InitialTransform.GetTranslation());
                    const float SnappedDeltaU = FMath::GridSnap(DeltaU, Settings.GizmoTranslationSnapEnable ? Settings.GizmoTranslationSnapValue : 0.0f);
                    const float SnappedDeltaV = FMath::GridSnap(DeltaV, Settings.GizmoTranslationSnapEnable ? Settings.GizmoTranslationSnapValue : 0.0f);
                    const FVector WorldDeltaU = U * SnappedDeltaU;
                    const FVector WorldDeltaV = V * SnappedDeltaV;
                    const FVector NewLocation = InitialTransform.GetTranslation() + WorldDeltaU + WorldDeltaV;
                    InOutTransform.SetLocation(NewLocation);
                    Result = true;

                    const FString Text = FString::Printf(TEXT("%+0.1f  %+0.1f"), SnappedDeltaU, SnappedDeltaV);
                    DrawGizmoText(FCogImguiHelper::ToImVec2(Center2D), FCogImguiHelper::ToImU32(Settings.GizmoTextColor), StringCast<ANSICHAR>(*Text).Get());

                    //DrawDebugPoint(World, InitialTransform.GetTranslation(), 5.0f, FColor::White);
                    //DrawDebugLine(World, InitialTransform.GetTranslation(), InitialTransform.GetTranslation() + WorldDeltaU, FColor::White);
                    //DrawDebugLine(World, InitialTransform.GetTranslation() + WorldDeltaU, InitialTransform.GetTranslation() + WorldDeltaU + WorldDeltaV, FColor::White);

                    break;
                }

                case ECogDebug_GizmoType::Rotate:
                {
                    const FVector2D DragDelta = FCogImguiHelper::ToFVector2D(ImGui::GetMouseDragDelta(ImGuiMouseButton_Left));
                    const float DragAmount = DragDelta.X - DragDelta.Y;
                    const float NormalizedAngle = FRotator::NormalizeAxis(DragAmount * Settings.GizmoRotationSpeed);
                    const float SnappedAngle = FMath::GridSnap(NormalizedAngle, Settings.GizmoRotationSnapEnable ? Settings.GizmoRotationSnapValue : 0.0f);
                    const FQuat RotDelta(-DraggedElement.Axis, FMath::DegreesToRadians(SnappedAngle));
                    const FQuat NewRot = (Settings.GizmoUseLocalSpace) ? InitialTransform.GetRotation() * RotDelta : RotDelta * InitialTransform.GetRotation();
                    InOutTransform.SetRotation(NewRot);
                    Result = true;

                    const FString Text = FString::Printf(TEXT("%+0.1f"), SnappedAngle);
                    DrawGizmoText(FCogImguiHelper::ToImVec2(Center2D), FCogImguiHelper::ToImU32(Settings.GizmoTextColor), StringCast<ANSICHAR>(*Text).Get());

                    break;
                }

                case ECogDebug_GizmoType::ScaleAxis:
                {
                    const FVector Point = GetMouseCursorOnLine(InPlayerController, GizmoCenter, DraggedElement.Direction, MousePos - CursorOffset);
                    const float Sign = FMath::Sign(FVector::DotProduct(DraggedElement.Direction, Point - GizmoCenter));
                    const float Delta = (Point - GizmoCenter).Length() * Sign;
                    const float SnappedDelta = FMath::GridSnap(Delta, Settings.GizmoScaleSnapEnable ? Settings.GizmoScaleSnapValue : 0.0f);
                    const FVector NewScale = VectorMax(InitialTransform.GetScale3D() + (DraggedElement.Axis * SnappedDelta * Settings.GizmoScaleSpeed), Settings.GizmoScaleMin);
                    InOutTransform.SetScale3D(NewScale);
                    Result = true;

                    const FString Text = FString::Printf(TEXT("%+0.1f"), SnappedDelta);
                    DrawGizmoText(FCogImguiHelper::ToImVec2(Center2D), FCogImguiHelper::ToImU32(Settings.GizmoTextColor), StringCast<ANSICHAR>(*Text).Get());

                    break;
                }

                case ECogDebug_GizmoType::ScaleUniform:
                {
                    const FVector2D DragDelta = FCogImguiHelper::ToFVector2D(ImGui::GetMouseDragDelta(ImGuiMouseButton_Left));
                    const float Delta = DragDelta.X - DragDelta.Y;
                    const float SnappedDelta = FMath::GridSnap(Delta, Settings.GizmoScaleSnapEnable ? Settings.GizmoScaleSnapValue : 0.0f);
                    const FVector NewScale = VectorMax(InitialTransform.GetScale3D() + (DraggedElement.Axis * SnappedDelta * Settings.GizmoScaleSpeed), Settings.GizmoScaleMin);
                    InOutTransform.SetScale3D(NewScale);
                    Result = true;

                    const FString Text = FString::Printf(TEXT("%+0.1f"), SnappedDelta);
                    DrawGizmoText(FCogImguiHelper::ToImVec2(Center2D), FCogImguiHelper::ToImU32(Settings.GizmoTextColor), StringCast<ANSICHAR>(*Text).Get());

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
            if (Settings.GizmoSupportContextMenu)
            {
                ImGui::OpenPopup(Id);
            }
        }
    }

    if (Settings.GizmoSupportContextMenu)
    {
        if (ImGui::BeginPopup(Id))
        {
            FVector Translation = InOutTransform.GetTranslation();
            FRotator Rotation = InOutTransform.GetRotation().Rotator();
            FVector Scale = InOutTransform.GetScale3D();

            ImGui::Checkbox("Local Space", &Settings.GizmoUseLocalSpace);

            ImGui::Separator();

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(1.0f, 1.0f));

            if (ImGui::BeginTable("Pools", 6, ImGuiTableFlags_SizingFixedFit))
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 5);
                ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 4);
                ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 4);
                ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 4);
                ImGui::TableSetupColumn("SnapEnable", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 4);
                ImGui::TableSetupColumn("Snap", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 3);

                ImGui::PushID("Location");
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Location");
                
                bool Translate = false;
                Translate |= RenderComponent("##X", &Translation.X, 0.0);
                Translate |=  RenderComponent("##Y", &Translation.Y, 0.0);
                Translate |=  RenderComponent("##Z", &Translation.Z, 0.0);
                if (Translate)
                {
                    if (Settings.GizmoTranslationSnapEnable)
                    {
                        Translation.X = FMath::GridSnap(Translation.X, Settings.GizmoTranslationSnapValue);
                        Translation.Y = FMath::GridSnap(Translation.Y, Settings.GizmoTranslationSnapValue);
                        Translation.Z = FMath::GridSnap(Translation.Z, Settings.GizmoTranslationSnapValue);
                    }
                    
                    InOutTransform.SetTranslation(Translation);
                    Result = true;
                }
                
                RenderSnap(&Settings.GizmoTranslationSnapEnable, &Settings.GizmoTranslationSnapValue);
                ImGui::PopID();

                ImGui::PushID("Rotation");
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Rotation");

                bool Rotate = false;
                Rotate |= RenderComponent("##X", &Rotation.Yaw, 0.0);
                Rotate |= RenderComponent("##Y", &Rotation.Pitch, 0.0); 
                Rotate |= RenderComponent("##Z", &Rotation.Roll, 0.0); 

                if (Rotate)
                {
                    if (Settings.GizmoRotationSnapEnable)
                    {
                        Rotation.Yaw = FMath::GridSnap(Rotation.Yaw, Settings.GizmoRotationSnapValue);
                        Rotation.Pitch = FMath::GridSnap(Rotation.Pitch, Settings.GizmoRotationSnapValue);
                        Rotation.Roll = FMath::GridSnap(Rotation.Roll, Settings.GizmoRotationSnapValue);
                    }
                    
                    InOutTransform.SetRotation(Rotation.Quaternion());
                    Result = true;
                }
                
                RenderSnap(&Settings.GizmoRotationSnapEnable, &Settings.GizmoRotationSnapValue);
                ImGui::PopID();

                ImGui::PushID("Scale");
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Scale");

                bool Rescale = false;
                Rescale |= RenderComponent("##X", &Scale.X, 1.0);
                Rescale |= RenderComponent("##Y", &Scale.Y, 1.0);
                Rescale |= RenderComponent("##Z", &Scale.Z, 1.0);

                if (Rescale)
                {
                    if (Settings.GizmoScaleSnapEnable)
                    {
                        Scale.X = FMath::GridSnap(Scale.X, Settings.GizmoScaleSnapValue);
                        Scale.Y = FMath::GridSnap(Scale.Y, Settings.GizmoScaleSnapValue);
                        Scale.Z = FMath::GridSnap(Scale.Z, Settings.GizmoScaleSnapValue);
                    }
                    
                    InOutTransform.SetScale3D(Scale);
                    Result = true;
                }
                
                RenderSnap(&Settings.GizmoScaleSnapEnable, &Settings.GizmoScaleSnapValue);
                ImGui::PopID();

                ImGui::EndTable();
            }

            ImGui::PopStyleVar();

            ImGui::EndPopup();
        }
    }

    return Result;
}
