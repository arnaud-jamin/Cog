#include "CogSampleTargetAcquisition.h"

#include "Camera/CameraComponent.h"
#include "CogSampleCharacter.h"
#include "CogSampleDefines.h"
#include "CogSampleFunctionLibrary_Gameplay.h"
#include "CogSampleLogCategories.h"
#include "CogSampleTargetableInterface.h"
#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/PlayerController.h"

#if ENABLE_COG
#include "CogDebugDraw.h"
#include "CogDebugLog.h"
#endif //ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
// FCogSampleTargetTargetAcquisitionParams
//--------------------------------------------------------------------------------------------------------------------------
struct FCogSampleTargetTargetAcquisitionParams
{
    const AActor* Source;
    FVector SourceLocation;
    const APlayerController* Controller;
    TArray<AActor*> TargetsToIgnore;
    bool bWorldDistanceIgnoreZ = false;
    float MaxWorldDistance = 0.0f;
    FVector2D CrosshairPosition = FVector2D::ZeroVector;
    FVector YawDirection = FVector::ZeroVector;
    FVector CameraRight = FVector::ZeroVector;
    FMatrix ViewProjectionMatrix;
    FIntRect ViewRect;
    FCollisionObjectQueryParams BlockersParams;
    bool bUseSearchDirection = false;
    FVector2D SearchDirectionScreenOrigin = FVector2D::ZeroVector;
    FVector2D SearchDirectionNormalized = FVector2D::ZeroVector;
    bool IsDebugPersistent = false;
    bool IsCrosshairInsideAnyCandidate = false;
};

//--------------------------------------------------------------------------------------------------------------------------
// UCogSampleTargetAcquisition
//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleTargetAcquisition::FindBestTargets(
    const APlayerController* Controller,
    const int32 TargetCount,
    const TArray<AActor*>& TargetsToIgnore,
    const AActor* CurrentLockedTarget,
    const bool bForceSynchronousDetection,
    const FVector2D ScreenSearchDirection,
    bool bIsDebugPersistent,
    TArray<FCogSampleTargetAcquisitionResult>& Results) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UCogSampleTargetAcquisition::FindBestTargets);

    IF_COG(FCogDebugDraw::String2D(LogCogTargetAcquisition, Controller, GetName(), FVector2D(20, 20), FColor::White, bIsDebugPersistent));

    TArray<AActor*> TempTargetsToIgnore(TargetsToIgnore);

    //---------------------------------------
    // Find multiple target
    //---------------------------------------
    for (int32 i = 0; i < TargetCount; i++)
    {
        FCogSampleTargetAcquisitionResult Result;
        FindBestTarget(
            Controller,
            TempTargetsToIgnore,
            CurrentLockedTarget,
            bForceSynchronousDetection,
            ScreenSearchDirection,
            bIsDebugPersistent,
            Result);

        //---------------------------------------
        // Stop when out of valid targets 
        //---------------------------------------
        if (Result.Target == nullptr)
        {
            break;
        }

        Results.Add(Result);
        TempTargetsToIgnore.Add(Result.Target);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleTargetAcquisition::FindBestTarget(
    const APlayerController* Controller,
    const TArray<AActor*>& TargetsToIgnore,
    const AActor* CurrentLockedTarget,
    const bool bForceSynchronousDetection,
    const FVector2D TargetSwitchSearchDirection,
    const bool bIsDebugPersistent,
    FCogSampleTargetAcquisitionResult& Result) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UCogSampleTargetAcquisition::FindBestTarget);

    ACogSampleCharacter* Character = Cast<ACogSampleCharacter>(Controller->GetPawn());
    if (Character == nullptr)
    {
        return;
    }

    if (FMath::IsNearlyZero(DetectionLength) && FMath::IsNearlyZero(DetectionRadius))
    {
        return;
    }

    FMatrix ViewProjectionMatrix;
    FIntRect ViewRect;
    if (GetViewInfo(Controller, ViewProjectionMatrix, ViewRect) == false)
    {
        return;
    }

    FVector2D SearchDirectionScreenOrigin(ViewRect.Width() / 2.0f, ViewRect.Height() / 2.0f);
    if (CurrentLockedTarget != nullptr)
    {
        FVector CurrentLockedTargetLocation = UCogSampleFunctionLibrary_Gameplay::GetActorTargetLocation(CurrentLockedTarget);
        FSceneView::ProjectWorldToScreen(CurrentLockedTargetLocation, ViewRect, ViewProjectionMatrix, SearchDirectionScreenOrigin);
    }

    //----------------------------------------------------------------------------------------------------------------------
    // Draw the ScreenSearchDirection if valid
    //----------------------------------------------------------------------------------------------------------------------
#if ENABLE_COG
    const FVector2D SearchDirectionNormalized = (TargetSwitchSearchDirection.IsNearlyZero() == false) ? TargetSwitchSearchDirection.GetSafeNormal() : FVector2D::ZeroVector;
    if (SearchDirectionNormalized.IsNearlyZero() == false)
    {
        FCogDebugDraw::Segment2D(LogCogTargetAcquisition, Controller, FVector2D::ZeroVector, FVector2D(SearchDirectionNormalized.X, -SearchDirectionNormalized.Y), FColor(255, 255, 0, 255), bIsDebugPersistent);
    }
#endif //ENABLE_COG

    static const FName TraceTag(TEXT("FindLockTarget_GatherTargets"));
    FCollisionQueryParams QueryParams(TraceTag, SCENE_QUERY_STAT_ONLY(CogSampleTargetAcquisition), false);
    QueryParams.bReturnPhysicalMaterial = true;
    QueryParams.bReturnFaceIndex = false;
    QueryParams.AddIgnoredActor(Character);

    const FCollisionObjectQueryParams ObjectParams = UCogSampleFunctionLibrary_Gameplay::ConfigureCollisionObjectParams(ObjectTypes);
    const FVector CastLocation = GetReferentialLocation(Character, DetectionLocation);
    const FRotator CastRotation = GetReferentialRotation(Character, DetectionRotation);
    const float CapsuleHalfHeight = DetectionLength * 0.5f;
    const float CapsuleRadius = DetectionRadius;
    const FVector CapsuleCenter = CastLocation + CastRotation.Vector() * CapsuleHalfHeight;
    const FQuat CapsuleRotation = (CastRotation + FRotator(90, 0, 0)).Quaternion();
    const FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);

    IF_COG(FCogDebugDraw::Capsule(LogCogTargetAcquisition, Controller, CapsuleCenter, CapsuleHalfHeight, CapsuleRadius, CapsuleRotation, FColor::Yellow, bIsDebugPersistent, 0));

    //-------------------------------------------------
    // Gather targets asynchronously    
    //-------------------------------------------------
    TArray<AActor*> QueriedActors;
    //if (UseAsyncDetection && bForceSynchronousDetection == false)
    //{
    //    TRACE_CPUPROFILER_EVENT_SCOPE(UCogSampleTargetAcquisition_Generic::EvaluateCandidateQueryAsync);

    //    FOverlapDatum OutData;
    //    if (Character->GetWorld()->QueryOverlapData(QueryHandle, OutData))
    //    {
    //        for (const FOverlapResult& Overlap : OutData.OutOverlaps)
    //        {
    //            if (AActor* Target = Overlap.GetActor())
    //            {
    //                QueriedActors.Add(Target);
    //            }
    //        }
    //    }

    //    QueryHandle = Character->GetWorld()->AsyncOverlapByObjectType(
    //        CapsuleCenter,
    //        CapsuleRotation,
    //        ObjectParams,
    //        CapsuleShape,
    //        QueryParams);
    //}
    //-------------------------------------------------
    // Gather targets synchronously    
    //-------------------------------------------------
    //else
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(UCogSampleTargetAcquisition::EvaluateCandidateQuerySync);

        TArray<FOverlapResult> OutOverlaps;
        const bool Hit = Character->GetWorld()->OverlapMultiByObjectType(
            OutOverlaps,
            CapsuleCenter,
            CapsuleRotation,
            ObjectParams,
            CapsuleShape,
            QueryParams);

        for (const FOverlapResult& Overlap : OutOverlaps)
        {
            if (AActor* Target = Overlap.GetActor())
            {
                QueriedActors.Add(Target);
            }
        }
    }

    //-------------------------------------------------
    // Validate actors 
    //-------------------------------------------------
    TArray<AActor*> ValidCandidates;
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(UCogSampleTargetAcquisition::EvaluateCandidateCheckValids);

        for (AActor* Actor : QueriedActors)
        {
            if (CheckIfTargetValid(Controller, Character, Actor) == false)
            {
                IF_COG(FCogDebugDraw::String(LogCogTargetAcquisition, Controller, "Filter", UCogSampleFunctionLibrary_Gameplay::GetActorTargetLocation(Actor), FColor::Red, bIsDebugPersistent));
                continue;
            }

            ValidCandidates.Add(Actor);
        }
    }

    FindBestTargetInCandidates(Controller, TargetsToIgnore, ValidCandidates, TargetSwitchSearchDirection, SearchDirectionScreenOrigin, bIsDebugPersistent, Result);
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleTargetAcquisition::GetViewInfo(
    const APlayerController* Controller,
    FMatrix& viewProjectionMatrix,
    FIntRect& viewRect) const
{
    const ULocalPlayer* localPlayer = Controller->GetLocalPlayer();
    if (localPlayer == nullptr || localPlayer->ViewportClient == nullptr)
    {
        return false;
    }

    FSceneViewProjectionData projectionData;
    if (localPlayer->GetProjectionData(localPlayer->ViewportClient->Viewport, projectionData) == false)
    {
        return false;
    }

    viewProjectionMatrix = projectionData.ComputeViewProjectionMatrix();
    viewRect = projectionData.GetConstrainedViewRect();

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleTargetAcquisition::FindBestTargetInCandidates(
    const APlayerController* Controller,
    const TArray<AActor*>& TargetsToIgnore,
    const TArray<AActor*>& Candidates,
    const FVector2D ScreenSearchDirection,
    const FVector2D SearchDirectionScreenOrigin,
    const bool bIsDebugPersistent,
    FCogSampleTargetAcquisitionResult& Result) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UCogSampleTargetAcquisition::FindBestTargetInCandidates);

    ACogSampleCharacter* Character = Cast<ACogSampleCharacter>(Controller->GetPawn());
    if (Character == nullptr)
    {
        return;
    }

    //----------------------------------------------------------------------------------------------------------------------
    // Compute view matrix to project each candidate capsules in screen space
    //----------------------------------------------------------------------------------------------------------------------
    FMatrix ViewProjectionMatrix;
    FIntRect ViewRect;
    if (GetViewInfo(Controller, ViewProjectionMatrix, ViewRect) == false)
    {
        return;
    }


    
    FVector2D ScreenSize(ViewRect.Width(), ViewRect.Height()); 
    FVector2D ScreenCrosshairPosition = ScreenSize * 0.5f;;

    //----------------------------------------------------------------------------------------------------------------------
    // Draw screen limits
    //----------------------------------------------------------------------------------------------------------------------

#if ENABLE_COG

    FCogDebugDraw::Circle2D(LogCogTargetAcquisition, Controller, ScreenCrosshairPosition, 5.0f, FColor(255, 255, 255, 255), bIsDebugPersistent);

    if (bUseScreenLimit)
    {
        if (ScreenLimitType == ECogSampleTargetAcquisitionScreenLimitType::Rectangle)
        {
            FCogDebugDraw::Rect2D(
                LogCogTargetAcquisition,
                Controller,
                UCogSampleFunctionLibrary_Gameplay::ViewportToScreen(FVector2D(-ScreenMaxX, -ScreenMaxY), ScreenSize),
                UCogSampleFunctionLibrary_Gameplay::ViewportToScreen(FVector2D(ScreenMaxX, ScreenMaxY), ScreenSize),
                FColor(255, 255, 255, 255),
                bIsDebugPersistent);
        }
        else
        {
            FCogDebugDraw::Circle2D(
                LogCogTargetAcquisition,
                Controller,
                ScreenCrosshairPosition,
                UCogSampleFunctionLibrary_Gameplay::ViewportToScreen(ScreenMaxX, ScreenSize),
                FColor(255, 255, 255, 255),
                bIsDebugPersistent);
        }
    }
#endif //ENABLE_COG

    const FRotator YawRotation = GetReferentialRotation(Character, YawReferential);
    const FVector YawDirection = YawRotation.Vector();

    FCogSampleTargetTargetAcquisitionParams Params;
    Params.Source = Character;
    Params.SourceLocation = UCogSampleFunctionLibrary_Gameplay::GetActorTargetLocation(Character);
    Params.Controller = Controller;
    Params.TargetsToIgnore = TargetsToIgnore;
    Params.bWorldDistanceIgnoreZ = WorldDistanceIgnoreZ;
    Params.MaxWorldDistance = WorldDistanceMax;
    Params.CrosshairPosition = ScreenCrosshairPosition;
    Params.YawDirection = YawDirection;
    Params.CameraRight = Character->GetFollowCamera()->GetComponentQuat().GetRightVector();
    Params.ViewProjectionMatrix = ViewProjectionMatrix;
    Params.ViewRect = ViewRect;
    Params.BlockersParams = UCogSampleFunctionLibrary_Gameplay::ConfigureCollisionObjectParams(BlockerTypes);
    Params.bUseSearchDirection = ScreenSearchDirection.IsNearlyZero() == false;
    Params.SearchDirectionScreenOrigin = SearchDirectionScreenOrigin;
    Params.SearchDirectionNormalized = Params.bUseSearchDirection ? ScreenSearchDirection.GetSafeNormal() : FVector2D::ZeroVector;
    Params.IsDebugPersistent = bIsDebugPersistent;
    Params.IsCrosshairInsideAnyCandidate = false;

    //----------------------------------------------------------------------------------------------------------------------
    // Evaluate candidates actors
    //----------------------------------------------------------------------------------------------------------------------
    float MinScore = FLT_MAX;
    AActor* BestTarget = nullptr;

    bool bIsCrosshairInsideAnyTarget = false;
    for (int32 i = 0; i < Candidates.Num(); ++i)
    {
        AActor* Candidate = Candidates[i];
        if (Candidate == nullptr)
        {
            continue;
        }

        float CandidateScore = 0.0f;
        bool bIsCrosshairInsideCandidate = false;
        if (EvaluateCandidate(Candidate, Params, CandidateScore, bIsCrosshairInsideCandidate) == false)
        {
            continue;
        }

        //-----------------------------------------------------------------------------------------
        // If the crosshair is inside this candidate, we can discard subsequent candidates 
        // that are not inside but also the best previous candidate since we were not inside it.
        //-----------------------------------------------------------------------------------------
        if (bPrioritizeTargetWithCrosshairInsideThem && bIsCrosshairInsideCandidate && bIsCrosshairInsideAnyTarget == false)
        {
            BestTarget = nullptr;
            MinScore = FLT_MAX;
        }

        Params.IsCrosshairInsideAnyCandidate |= bIsCrosshairInsideCandidate;

        if (CandidateScore > MinScore)
        {
            continue;
        }

        MinScore = CandidateScore;
        BestTarget = Candidate;
    }

    if (BestTarget != nullptr)
    {
        Result.Target = BestTarget;
        Result.Score = MinScore;

        IF_COG(FCogDebugDraw::Point(LogCogTargetAcquisition, Controller, UCogSampleFunctionLibrary_Gameplay::GetActorTargetLocation(Result.Target), 20.0f, FColor::Green, Params.IsDebugPersistent, 100));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleTargetAcquisition::EvaluateCandidate(
    AActor* Candidate,
    const FCogSampleTargetTargetAcquisitionParams& EvalParams,
    float& CandidateScore,
    bool& bIsCrosshairInsideCandidate) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UCogSampleTargetAcquisition::EvaluateCandidate);

    if (EvalParams.TargetsToIgnore.Contains(Candidate))
    {
        return false;
    }

    const FVector CandidateTargetLocation = UCogSampleFunctionLibrary_Gameplay::GetActorTargetLocation(Candidate);

    FVector CandidateLocationDelta = CandidateTargetLocation - EvalParams.SourceLocation;
    if (EvalParams.bWorldDistanceIgnoreZ)
    {
        CandidateLocationDelta.Z = 0.0f;
    }
    const float CandidateWorldDistance = CandidateLocationDelta.Length();
    const FVector CandidateWorldDirection = CandidateWorldDistance > 0.0f ? CandidateLocationDelta / CandidateWorldDistance : FVector::ZeroVector;
    const float ScreenMagnitude = FMath::Min(EvalParams.ViewRect.Width(), EvalParams.ViewRect.Height());
    const bool IsSearchDirectionUsed = bUseSearchDirectionScore && EvalParams.SearchDirectionNormalized.IsNearlyZero() == false;

    //--------------------------------------------------------------------------------------------------------------
    // Filter by world distance limit
    //--------------------------------------------------------------------------------------------------------------        
    if (CandidateWorldDistance > EvalParams.MaxWorldDistance)
    {
        IF_COG(FCogDebugDraw::String(LogCogTargetAcquisition, EvalParams.Controller, FString::Printf(TEXT("Dist: %0.2f"), CandidateWorldDistance * 0.01f), CandidateTargetLocation, FColor::Red, EvalParams.IsDebugPersistent));
        return false;
    }

    //--------------------------------------------------------------------------------------------------------------
    // Filter candidates base on yaw limit when using yaw, otherwise filter anyone behind the character
    //--------------------------------------------------------------------------------------------------------------
    const FVector CandidateWorldDirectionFlat = CandidateWorldDirection.GetSafeNormal2D();
    const float CandidateDot = EvalParams.YawDirection.Dot(CandidateWorldDirectionFlat);
    const float CandidateYaw = FRotator::NormalizeAxis(FMath::RadiansToDegrees(FMath::Acos(CandidateDot)));
    if (bUseYawLimit && CandidateYaw > YawMax)
    {
        IF_COG(FCogDebugDraw::String(LogCogTargetAcquisition, EvalParams.Controller, FString::Printf(TEXT("Yaw: %0.2f"), CandidateYaw), CandidateTargetLocation, FColor::Red, EvalParams.IsDebugPersistent));
        return false;
    }

    //--------------------------------------------------------------------------------------------------------------
    // Find the candidate screen location. If the result distance is lower than zero, it means the crosshair is
    // inside the candidate capsules.
    //--------------------------------------------------------------------------------------------------------------
    FVector2D CandidateScreenLocation;
    FVector2D CandidateClosestScreenLocation;
    float CandidateClosestScreenDistance;
    const UCapsuleComponent* CandidateBestHitZone = nullptr;
    if (!ComputeCandidateScreenLocation(Candidate, EvalParams, CandidateTargetLocation, CandidateScreenLocation, CandidateClosestScreenLocation, CandidateClosestScreenDistance))
    {
        return false;
    }

    //--------------------------------------------------------------------------------------------------------------
    // Filter candidates not inside the screen distance limits.
    //--------------------------------------------------------------------------------------------------------------
    if (bUseScreenLimit)
    {
        if (!CheckCandidateWithinScreenDistance(EvalParams.Controller, EvalParams.ViewRect, CandidateTargetLocation, CandidateClosestScreenLocation, CandidateClosestScreenDistance, EvalParams.IsDebugPersistent))
        {
            return false;
        }
    }

    //--------------------------------------------------------------------------------------------------------------
    // Raycast to verify this target is not blocked by a collision.
    //--------------------------------------------------------------------------------------------------------------
    if (HasLineOfSightToTarget(EvalParams.Source, EvalParams.SourceLocation, Candidate, CandidateTargetLocation, EvalParams.BlockersParams) == false)
    {
        return false;
    }

    bIsCrosshairInsideCandidate = CandidateClosestScreenDistance < 0.0f;
    if (!IsSearchDirectionUsed && bPrioritizeTargetWithCrosshairInsideThem)
    {
        //--------------------------------------------------------------------------------------------------------------
        // We always prioritize the candidates if the crosshair is inside them. Thus if we are inside another candidate
        // but not inside this one, we discard it (unless we are switching the lock target)
        //--------------------------------------------------------------------------------------------------------------
        if (EvalParams.IsCrosshairInsideAnyCandidate && bIsCrosshairInsideCandidate == false)
        {
            return false;
        }
    }

    //--------------------------------------------------------------------------------------------------------------
    // Compute a score based on the world distance
    //--------------------------------------------------------------------------------------------------------------
    float CandidateWorldDistanceRatio = 0.0f;
    float CandidateWorldDistanceScore = 0.0f;
    if (bUseWorldDistanceScore)
    {
        CandidateWorldDistanceRatio = (WorldDistanceMax > 0.0f) ? CandidateWorldDistance / WorldDistanceMax : 0.0f;
        CandidateWorldDistanceScore = WorldDistanceScoreMultiplier * (WorldDistanceScoreCurve != nullptr ? WorldDistanceScoreCurve->GetFloatValue(CandidateWorldDistanceRatio) : CandidateWorldDistanceRatio);
    }

    //--------------------------------------------------------------------------------------------------------------
    // Compute a score based on screen distance
    //--------------------------------------------------------------------------------------------------------------
    float CandidateScreenDistanceRatio = 0.0f;
    float CandidateScreenDistanceScore = 0.0f;
    if (bUseScreenDistanceScore && IsSearchDirectionUsed == false)
    {
        CandidateScreenDistanceRatio = CandidateClosestScreenDistance / ScreenMagnitude;
        CandidateScreenDistanceScore = ScreenDistanceScoreMultiplier * (ScreenDistanceScoreCurve != nullptr ? ScreenDistanceScoreCurve->GetFloatValue(CandidateScreenDistanceRatio) : CandidateScreenDistanceRatio);
    }

    //--------------------------------------------------------------------------------------------------------------
    // Compute a score based on yaw
    //--------------------------------------------------------------------------------------------------------------
    float CandidateYawRatio = 0.0f;
    float CandidateYawScore = 0.0f;
    if (bUseYawScore)
    {
        CandidateYawRatio = YawMax > 0.0f ? CandidateYaw / YawMax : 0.0f;
        CandidateYawScore = YawScoreMultiplier * (YawScoreCurve != nullptr ? YawScoreCurve->GetFloatValue(CandidateYawRatio) : CandidateYawRatio);
    }

    //--------------------------------------------------------------------------------------------------------------
    // Compute a score based on the search direction. (for target lock switch)
    //--------------------------------------------------------------------------------------------------------------
    float SearchDirectionDistanceScore = 0.0f;
    float SearchDirectionAngleScore = 0.0f;
    float SearchDirectionDistanceRatio = 0.0f;
    float SearchDirectionAngleRatio = 0.0f;

    if (IsSearchDirectionUsed)
    {
        const float TargetAngleWithSearchDirection = UCogSampleFunctionLibrary_Gameplay::AngleBetweenVector2D(CandidateScreenLocation - EvalParams.SearchDirectionScreenOrigin, EvalParams.SearchDirectionNormalized);

        if (FMath::Abs(TargetAngleWithSearchDirection) > SearchDirectionMaxAngle)
        {
            IF_COG(FCogDebugDraw::String(LogCogTargetAcquisition, EvalParams.Controller, FString::Printf(TEXT("MaxAngle: %0.2f"), TargetAngleWithSearchDirection), CandidateTargetLocation, FColor::Red, EvalParams.IsDebugPersistent));
            return false;
        }

        SearchDirectionDistanceRatio = FVector2D::Distance(CandidateScreenLocation, EvalParams.SearchDirectionScreenOrigin) / ScreenMagnitude;
        SearchDirectionDistanceScore = SearchDirectionDistanceScoreMultiplier * (ScreenSearchDirectionDistanceScoreCurve != nullptr ? ScreenSearchDirectionDistanceScoreCurve->GetFloatValue(SearchDirectionDistanceRatio) : SearchDirectionDistanceRatio);
        SearchDirectionAngleRatio = SearchDirectionMaxAngle > 0.0f ? TargetAngleWithSearchDirection / SearchDirectionMaxAngle : 0.0f;
        SearchDirectionAngleScore = ScreenSearchDirectionAngleScoreMultiplier * (ScreenSearchDirectionAngleScoreCurve != nullptr ? ScreenSearchDirectionAngleScoreCurve->GetFloatValue(SearchDirectionAngleRatio) : SearchDirectionAngleRatio);
    }

    //--------------------------------------------------------------------------------------------------------------
    // Compute final score by summing all the scores. The best score is the smallest one.
    //--------------------------------------------------------------------------------------------------------------
    CandidateScore = CandidateWorldDistanceScore + CandidateScreenDistanceScore + CandidateYawScore + SearchDirectionDistanceScore + SearchDirectionAngleScore;

    //--------------------------------------------------------------------------------------------------------------
    // Draw the score of each candidate
    //--------------------------------------------------------------------------------------------------------------
#if ENABLE_COG

    if (FCogDebugLog::IsLogCategoryActive(LogCogTargetAcquisition))
    {
        FCogDebugDraw::Point(LogCogTargetAcquisition, EvalParams.Controller, CandidateTargetLocation, 8.0f, FColor::Blue, EvalParams.IsDebugPersistent, 0);

        FString Text;
        if (LogCogTargetAcquisition.GetVerbosity() == ELogVerbosity::VeryVerbose)
        {
            if (bUseScreenDistanceScore)
            {
                Text.Append(FString::Printf(TEXT
                (
                    "SD: %.0f => %.0f => %.0f \n"
                ),
                CandidateClosestScreenDistance, CandidateScreenDistanceRatio * 100, CandidateScreenDistanceScore * 100));
            }

            if (bUseWorldDistanceScore)
            {
                Text.Append(FString::Printf(TEXT("WD: %.0f => %.0f => %.0f \n"),
                    CandidateWorldDistance * 0.01f,
                    CandidateWorldDistanceRatio * 100,
                    CandidateWorldDistanceScore * 100));
            }

            if (bUseYawScore)
            {
                Text.Append(FString::Printf(TEXT("Y: %.1f => %.0f => %.0f \n"),
                    CandidateYaw,
                    CandidateYawRatio * 100,
                    CandidateYawScore * 100));
            }

            if (bUseSearchDirectionScore)
            {
                Text.Append(FString::Printf(TEXT("VD: %.0f => %.0f \n" "VA: %.0f => %.0f \n"),
                    SearchDirectionDistanceRatio * 100,
                    SearchDirectionDistanceScore * 100,
                    SearchDirectionAngleRatio * 100,
                    SearchDirectionAngleScore * 100));
            }

            Text.Append(FString::Printf(TEXT("==> %.0f \n"), CandidateScore * 100));
        }
        else
        {
            Text = FString::Printf(TEXT("%0.f"), CandidateScore * 100);
        }
        FCogDebugDraw::String(LogCogTargetAcquisition, EvalParams.Controller, Text, CandidateTargetLocation, FColor::White, EvalParams.IsDebugPersistent);
    }
    
#endif //ENABLE_COG

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleTargetAcquisition::ComputeCandidateScreenLocation(
    const AActor* CandidateActor,
    const FCogSampleTargetTargetAcquisitionParams& Params,
    const FVector& CandidateTargetLocation,
    FVector2D& CandidateScreenLocation,
    FVector2D& CandidateClosestScreenLocation,
    float& CandidateClosestScreenDistance) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UCogSampleTargetAcquisition::ComputeCandidateScreenLocation);

    check(CandidateActor);

    bool bFoundValidCandidate = false;
    CandidateScreenLocation = FVector2D::ZeroVector;
    CandidateClosestScreenDistance = FLT_MAX;

    if (const ICogSampleTargetableInterface* Targetable = Cast<ICogSampleTargetableInterface>(CandidateActor))
    {
        TArray<const UCapsuleComponent*> Capsules;
        Targetable->GetTargetCapsules(Capsules);

        for (const UCapsuleComponent* Capsule : Capsules)
        {
            const float Radius = Capsule->GetScaledCapsuleRadius();
            const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();

            const FVector CapsuleLocation = Capsule->GetComponentLocation();
            const FVector CapsuleTop = CapsuleLocation + FVector::UpVector * (HalfHeight - Radius);
            const FVector CapsuleBottom = CapsuleLocation - FVector::UpVector * (HalfHeight - Radius);
            const FVector CapsuleRight = CapsuleLocation - Params.CameraRight * Radius;

            FVector2D CapsuleTop2D;
            FVector2D CapsuleBot2D;
            FVector2D CapsuleRight2D;

            if (FSceneView::ProjectWorldToScreen(CapsuleTop, Params.ViewRect, Params.ViewProjectionMatrix, CapsuleTop2D) == false)
            {
                continue;
            }

            if (FSceneView::ProjectWorldToScreen(CapsuleBottom, Params.ViewRect, Params.ViewProjectionMatrix, CapsuleBot2D) == false)
            {
                continue;
            }

            if (FSceneView::ProjectWorldToScreen(CapsuleRight, Params.ViewRect, Params.ViewProjectionMatrix, CapsuleRight2D) == false)
            {
                continue;
            }

            const FVector2D CapsuleCenter2D = CapsuleBot2D + 0.5f * (CapsuleTop2D - CapsuleBot2D);
            const float CapsuleRadius2D = FVector2D::Distance(CapsuleCenter2D, CapsuleRight2D);

            FVector2D Projection;
            float Time;
            float ScreenCenterToCapsuleDistance;
            UCogSampleFunctionLibrary_Gameplay::FindCapsulePointDistance(CapsuleBot2D, CapsuleTop2D, CapsuleRadius2D, Params.CrosshairPosition, Projection, Time, ScreenCenterToCapsuleDistance);

            if (ScreenCenterToCapsuleDistance < CandidateClosestScreenDistance)
            {
                CandidateScreenLocation = CapsuleCenter2D;
                CandidateClosestScreenDistance = ScreenCenterToCapsuleDistance;
                CandidateClosestScreenLocation = Projection;
                bFoundValidCandidate = true;
            }

#if ENABLE_COG
            const FColor CapsuleColor = (ScreenCenterToCapsuleDistance > 0.0f) ? FColor(255, 255, 255, 100) : FColor(0, 255, 0, 200);
            FCogDebugDraw::Segment2D(LogCogTargetAcquisition, CandidateActor, CapsuleBot2D + FVector2D(CapsuleRadius2D, 0), CapsuleTop2D + FVector2D(CapsuleRadius2D, 0), CapsuleColor, Params.IsDebugPersistent);
            FCogDebugDraw::Segment2D(LogCogTargetAcquisition, CandidateActor, CapsuleBot2D - FVector2D(CapsuleRadius2D, 0), CapsuleTop2D - FVector2D(CapsuleRadius2D, 0), CapsuleColor, Params.IsDebugPersistent);
            FCogDebugDraw::Circle2D(LogCogTargetAcquisition, CandidateActor, CapsuleTop2D, CapsuleRadius2D, CapsuleColor, Params.IsDebugPersistent);
            FCogDebugDraw::Circle2D(LogCogTargetAcquisition, CandidateActor, CapsuleBot2D, CapsuleRadius2D, CapsuleColor, Params.IsDebugPersistent);
#endif //ENABLE_COG
        }
    }
    else
    {
        if (FSceneView::ProjectWorldToScreen(CandidateTargetLocation, Params.ViewRect, Params.ViewProjectionMatrix, CandidateScreenLocation))
        {
            CandidateClosestScreenDistance = CandidateScreenLocation.Length();
            CandidateClosestScreenLocation = CandidateScreenLocation;
            bFoundValidCandidate = true;
        }
    }

#if ENABLE_COG
    if (bFoundValidCandidate)
    {
        FCogDebugDraw::Circle2D(LogCogTargetAcquisition, CandidateActor, CandidateClosestScreenLocation, 2.0f, FColor(0, 255, 0, 255), Params.IsDebugPersistent);
    }
#endif //ENABLE_COG

    return bFoundValidCandidate;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleTargetAcquisition::CheckCandidateWithinScreenDistance(
    const APlayerController* Controller,
    const FIntRect& ViewRect,
    const FVector& CandidateLocation,
    const FVector2D& CandidateScreenLocation,
    const float CandidateScreenDistance,
    const bool bIsDebugPersistent) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UCogSampleTargetAcquisition::CheckCandidateWithinScreenDistance);

    const FVector2D ScreenSize(ViewRect.Width(), ViewRect.Height());
    const FVector2D CandidateViewportLocation = UCogSampleFunctionLibrary_Gameplay::ScreenToViewport(CandidateScreenLocation, ScreenSize);
    const float CandidateViewportDistance = UCogSampleFunctionLibrary_Gameplay::ScreenToViewport(CandidateScreenDistance, ScreenSize);

    //--------------------------------------------------------------------------------------------------------------
    // Filter by screen distance - within rectangle
    //--------------------------------------------------------------------------------------------------------------
    if (ScreenLimitType == ECogSampleTargetAcquisitionScreenLimitType::Rectangle)
    {
        if (FMath::Abs(CandidateViewportLocation.X) > ScreenMaxX)
        {
            IF_COG(FCogDebugDraw::String(LogCogTargetAcquisition, Controller, FString::Printf(TEXT("MaxX: %0.2f"), CandidateViewportLocation.X), CandidateLocation, FColor::Red, bIsDebugPersistent));
            return false;
        }

        if (FMath::Abs(CandidateViewportLocation.Y) > ScreenMaxY)
        {
            IF_COG(FCogDebugDraw::String(LogCogTargetAcquisition, Controller, FString::Printf(TEXT("MaxY: %0.2f"), CandidateViewportLocation.Y), CandidateLocation, FColor::Red, bIsDebugPersistent));
            return false;
        }
    }
    //--------------------------------------------------------------------------------------------------------------
    // Filter by screen distance - within circle
    //--------------------------------------------------------------------------------------------------------------
    else if (ScreenLimitType == ECogSampleTargetAcquisitionScreenLimitType::Circle)
    {
        if (CandidateViewportDistance > ScreenMaxX)
        {
            IF_COG(FCogDebugDraw::String(LogCogTargetAcquisition, Controller, FString::Printf(TEXT("Max: %0.2f"), CandidateViewportDistance), CandidateLocation, FColor::Red, bIsDebugPersistent));
            return false;
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleTargetAcquisition::CheckIfTargetValid(
    const APlayerController* Controller,
    const AActor* Source,
    const AActor* Target) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UCogSampleTargetAcquisition::CheckIfTargetValid);

    if (UCogSampleFunctionLibrary_Team::MatchAllegiance(Source, Target, Allegiance) == false)
    {
        return false;
    }

    if (UCogSampleFunctionLibrary_Gameplay::IsActorMatchingTags(Target, RequiredTags, IgnoredTags) == false)
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleTargetAcquisition::HasLineOfSightToTarget(
    const AActor* Source,
    const FVector& SourceLocation,
    const AActor* Target,
    const FVector& TargetLocation,
    const FCollisionObjectQueryParams& BlockersParams) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UCogSampleTargetAcquisition::HasLineOfSightToTarget);

    static const FName BlockerTraceTag(TEXT("FindLockTarget_Blocker"));
    FCollisionQueryParams TargetQueryParams(BlockerTraceTag, SCENE_QUERY_STAT_ONLY(CogSampleTargetAcquisition), false);
    TargetQueryParams.AddIgnoredActor(Source);
    TargetQueryParams.AddIgnoredActor(Target);

    return UCogSampleFunctionLibrary_Gameplay::HasLineOfSight(Source->GetWorld(), SourceLocation, TargetLocation, BlockersParams, TargetQueryParams, LogCogTargetAcquisition);
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleTargetAcquisition::HasLineOfSightToTargetBrokenForTooLong(
    const AActor* Source,
    const AActor* Target,
    const float DeltaTime,
    float& Timer) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UCogSampleTargetAcquisition::HasLineOfSightToTargetBrokenForTooLong);

    const FVector& SourceLocation = UCogSampleFunctionLibrary_Gameplay::GetActorTargetLocation(Source); 
    const FVector& TargetLocation = UCogSampleFunctionLibrary_Gameplay::GetActorTargetLocation(Target);

    const FCollisionObjectQueryParams BlockersParams = UCogSampleFunctionLibrary_Gameplay::ConfigureCollisionObjectParams(BlockerTypes);
    const bool HasLineOfSight = HasLineOfSightToTarget(Source, SourceLocation, Target, TargetLocation, BlockersParams);
    if (HasLineOfSight)
    {
        Timer = 0.0f;
    }
    else
    {
        Timer += DeltaTime;

        if (Timer > BreakLineOfSightDelay)
        {
            Timer = 0.0f;
            return true;
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
FVector UCogSampleTargetAcquisition::GetReferentialLocation(const ACogSampleCharacter* Character, ECogSampleTargetAcquisitionLocationReferential Referential)
{
    FVector Location;

    switch (Referential)
    {
        case ECogSampleTargetAcquisitionLocationReferential::Character:
        {
            Location = UCogSampleFunctionLibrary_Gameplay::GetActorBottomLocation(Character);
            break;
        }


        case ECogSampleTargetAcquisitionLocationReferential::Camera:
        {
            Location = Character->GetFollowCamera()->GetComponentLocation();
            break;
        }
    }

    return Location;
}

//--------------------------------------------------------------------------------------------------------------------------
FRotator UCogSampleTargetAcquisition::GetReferentialRotation(const ACogSampleCharacter* Character, ECogSampleTargetAcquisitionRotationReferential Referential)
{
    FRotator Rotation;

    switch (Referential)
    {
        case ECogSampleTargetAcquisitionRotationReferential::Character:
        {
            Rotation = Character->GetActorRotation();
            break;
        }

        case ECogSampleTargetAcquisitionRotationReferential::MoveInput:
        {
            const FVector WorldInput = Character->GetMoveInputInWorldSpace();
            if (WorldInput.IsNearlyZero())
            {
                Rotation = Character->GetActorRotation();
            }
            else
            {
                Rotation = WorldInput.GetSafeNormal().Rotation();
            }
            break;
        }

        case ECogSampleTargetAcquisitionRotationReferential::Camera:
        {
            Rotation = Character->GetFollowCamera()->GetComponentRotation();
            break;
        }

        case ECogSampleTargetAcquisitionRotationReferential::CameraFlatten:
        {
            const FVector CameraForwardFlat = Character->GetFollowCamera()->GetComponentQuat().GetForwardVector().GetSafeNormal2D();
            Rotation = CameraForwardFlat.Rotation();
            break;
        }
    }

    return Rotation;
}
