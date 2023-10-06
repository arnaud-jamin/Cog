#pragma once

#include "CoreMinimal.h"
#include "CogSampleFunctionLibrary_Team.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "WorldCollision.h"
#include "CogSampleTargetAcquisition.generated.h"

class ACogSampleCharacter;
class APlayerController;
class UCurveFloat;
struct FCogSampleTargetCandidateEvaluationResult;
struct FCogSampleTargetCandidateEvaluationParameters;

//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogSampleTargetAcquisitionType : uint8
{
    Melee,
    Range
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogSampleTargetAcquisitionScreenLimitType : uint8
{
    Rectangle,
    Circle,
};


//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogSampleTargetAcquisitionLocationReferential : uint8
{
    Character,
    Camera,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogSampleTargetAcquisitionRotationReferential : uint8
{
    Camera,
    CameraFlatten,
    Character,
    MoveInput,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogSampleTargetAcquisitionCrosshairReferential : uint8
{
    Centered,
    Offseted,
};

//--------------------------------------------------------------------------------------------------------------------------
struct FCogSampleTargetAcquisitionResult
{
public:
    AActor* Target = nullptr;

    float Score = FLT_MAX;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS()
class UCogSampleTargetAcquisition : public UDataAsset
{
    GENERATED_BODY()

public:

    //--------------------------------------------------------------------------------------------------------------
    // General
    //--------------------------------------------------------------------------------------------------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
    TArray<TEnumAsByte<EObjectTypeQuery>> BlockerTypes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
    float BreakLineOfSightDelay = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (Bitmask, BitmaskEnum = "/Script/CogSample.ECogSampleAllegianceFilter"))
    int32 Allegiance = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
    bool AcceptDead = false;

    //--------------------------------------------------------------------------------------------------------------
    // Detection
    //--------------------------------------------------------------------------------------------------------------

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    ECogSampleTargetAcquisitionLocationReferential DetectionLocation = ECogSampleTargetAcquisitionLocationReferential::Camera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    ECogSampleTargetAcquisitionRotationReferential DetectionRotation = ECogSampleTargetAcquisitionRotationReferential::Camera;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Detection")
    float DetectionLength = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Detection")
    float DetectionRadius = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Detection")
    bool UseAsyncDetection = true;

    //--------------------------------------------------------------------------------------------------------------
    // Screen Limit
    //--------------------------------------------------------------------------------------------------------------

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limit|Screen", meta = (ToolTip = "Limit based on the screen distance. The screen distance is computed between the crosshair and the candidate screen position"))
    bool bUseScreenLimit = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limit|Screen", meta = (EditCondition = "bUseScreenLimit", EditConditionHides))
    bool bPrioritizeInsideHitZones = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limit|Screen", meta = (EditCondition = "bUseScreenLimit", EditConditionHides))
    bool ScreenTestUseAspectRatio = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limit|Screen", meta = (EditCondition = "bUseScreenLimit", EditConditionHides))
    ECogSampleTargetAcquisitionCrosshairReferential CrosshairReferential = ECogSampleTargetAcquisitionCrosshairReferential::Centered;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limit|Screen", meta = (EditCondition = "bUseScreenLimit", EditConditionHides))
    ECogSampleTargetAcquisitionScreenLimitType ScreenLimitType = ECogSampleTargetAcquisitionScreenLimitType::Rectangle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limit|Screen", meta = (EditCondition = "bUseScreenLimit", EditConditionHides))
    float ScreenMaxX = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limit|Screen", meta = (EditCondition = "bUseScreenLimit", EditConditionHides))
    float ScreenMaxY = 1.0f;

    //--------------------------------------------------------------------------------------------------------------
    // Yaw Limit
    //--------------------------------------------------------------------------------------------------------------

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limit|Yaw", meta = (ToolTip = "Limit based on the yaw angle. The yaw angle is computed between the camera forward vector and the direction between the player character and the candidate"))
    bool bUseYawLimit = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Limit|Yaw", meta = (EditCondition = "bUseYawLimit", EditConditionHides))
    float YawMax = 90.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Limit|Yaw")
    ECogSampleTargetAcquisitionRotationReferential YawReferential = ECogSampleTargetAcquisitionRotationReferential::Camera;

    //--------------------------------------------------------------------------------------------------------------
    // World Distance
    //--------------------------------------------------------------------------------------------------------------

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|World Distance", meta = (ToolTip = "The world distance is the distance between the character position and the candidate position"))
    bool bUseWorldDistanceScore = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|World Distance", meta = (EditCondition = "bUseWorldDistanceScore", EditConditionHides))
    float WorldDistanceMax = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|World Distance", meta = (EditCondition = "bUseWorldDistanceScore", EditConditionHides))
    UCurveFloat* WorldDistanceScoreCurve = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|World Distance", meta = (EditCondition = "bUseWorldDistanceScore", EditConditionHides))
    float WorldDistanceScoreMultiplier = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|World Distance", meta = (EditCondition = "bUseWorldDistanceScore", EditConditionHides))
    bool WorldDistanceIgnoreZ = false;

    //--------------------------------------------------------------------------------------------------------------
    // Screen Distance Score
    //--------------------------------------------------------------------------------------------------------------
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|Screen Distance", meta = (ToolTip = "The screen distance is the distance between the crosshair and the candidate screen position"))
    bool bUseScreenDistanceScore = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|Screen Distance", meta = (EditCondition = "bUseScreenDistanceScore", EditConditionHides))
    float ScreenDistanceScoreMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|Screen Distance", meta = (EditCondition = "bUseScreenDistanceScore", EditConditionHides))
    UCurveFloat* ScreenDistanceScoreCurve = nullptr;

    //--------------------------------------------------------------------------------------------------------------
    // Yaw Scoring
    //--------------------------------------------------------------------------------------------------------------

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|Yaw", meta = (ToolTip = "The yaw angle is computed between the camera forward vector and the direction between the player character and the candidate"))
    bool bUseYawScore = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|Yaw", meta = (EditCondition = "bUseYawScore", EditConditionHides))
    float YawScoreMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|Yaw", meta = (EditCondition = "bUseYawScore", EditConditionHides))
    UCurveFloat* YawScoreCurve = nullptr;

    //--------------------------------------------------------------------------------------------------------------
    // Search Direction 
    //--------------------------------------------------------------------------------------------------------------

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|Search Direction", meta = (ToolTip = "The search direction is used when the player has already a locked target and request a target switch with a direction performed by the stick or the mouse"))
    bool bUseSearchDirectionScore = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|Search Direction", meta = (EditCondition = "bUseSearchDirectionScore", EditConditionHides))
    float SearchDirectionMaxAngle = 60.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|Search Direction", meta = (EditCondition = "bUseSearchDirectionScore", EditConditionHides))
    UCurveFloat* ScreenSearchDirectionAngleScoreCurve = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|Search Direction", meta = (EditCondition = "bUseSearchDirectionScore", EditConditionHides))
    float ScreenSearchDirectionAngleScoreMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|Search Direction", meta = (EditCondition = "bUseSearchDirectionScore", EditConditionHides))
    UCurveFloat* ScreenSearchDirectionDistanceScoreCurve = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring|Search Direction", meta = (EditCondition = "bUseSearchDirectionScore", EditConditionHides))
    float SearchDirectionDistanceScoreMultiplier = 1.0f;

    //--------------------------------------------------------------------------------------------------------------

    virtual void FindBestTargets(
        const APlayerController* Controller,
        const int32 TargetCount,
        const TArray<AActor*>& TargetsToIgnore,
        const AActor* CurrentLockedTarget,
        const bool bForceSynchronousDetection,
        const FVector2D ScreenSearchDirection,
        const bool bIsDebugPersistent,
        TArray<FCogSampleTargetAcquisitionResult>& Results) const;

    virtual bool HasLineOfSightToTargetBrokenForTooLong(
        const AActor* Source,
        const AActor* Target,
        const float DeltaTime,
        float& Timer) const;

    virtual bool CheckIfTargetValid(
        const APlayerController* Controller,
        const AActor* Source,
        const AActor* Target) const;

    //--------------------------------------------------------------------------------------------------------------
    // Utility
    //--------------------------------------------------------------------------------------------------------------

    void FindBestTargetInCandidates(
        const APlayerController* Controller,
        const TArray<AActor*>& TargetsToIgnore,
        const TArray<AActor*>& Candidates,
        const FVector2D ScreenSearchDirection,
        const FVector2D SearchDirectionViewportOrigin,
        const bool bIsDebugPersistent,
        FCogSampleTargetAcquisitionResult& Result) const;

    void FindBestTarget(
        const APlayerController* Controller,
        const TArray<AActor*>& TargetsToIgnore,
        const AActor* CurrentLockedTarget,
        const bool bForceSynchronousDetection,
        const FVector2D ScreenSearchDirection,
        const bool bIsDebugPersistent,
        FCogSampleTargetAcquisitionResult& Result) const;

    bool HasLineOfSightToTarget(
        const AActor* Source,
        const AActor* Target,
        const FCollisionObjectQueryParams& BlockersParams) const;

    bool EvaluateCandidate(
        AActor* CandidateTarget,
        const FCogSampleTargetCandidateEvaluationParameters& EvaluationParameters,
        FCogSampleTargetCandidateEvaluationResult& EvaluationResult) const;

    bool CheckCandidateWithinScreenDistance(
        const APlayerController* Controller,
        const FIntRect& viewRect,
        const FVector& candidateLocation,
        const FVector2D& candidateScreenLocation,
        const float candidateScreenDistance,
        const bool bIsDebugPersistent) const;

    bool ComputeCandidateScreenLocation(
        const AActor* CandidateTarget,
        const FCogSampleTargetCandidateEvaluationParameters& EvalParams,
        const FVector& CandidateTargetLocation,
        FVector2D& CandidateScreenLocation,
        FVector2D& CandidateClosestScreenLocation,
        float& CandidateClosestScreenDistance) const;

    bool GetViewInfo(
        const APlayerController* Controller,
        FMatrix& viewProjectionMatrix,
        FIntRect& viewRect) const;

    static FVector GetReferentialLocation(const ACogSampleCharacter* Character, ECogSampleTargetAcquisitionLocationReferential Referential);

    static FRotator GetReferentialRotation(const ACogSampleCharacter* Character, ECogSampleTargetAcquisitionRotationReferential Referential);
};