#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "CogDebug.generated.h"

class UObject;
class AActor;
class UWorld;

USTRUCT()
struct FCogDebugSettings
{
    GENERATED_BODY()

    UPROPERTY(Config)
    bool bIsFilteringBySelection = true;

    UPROPERTY(Config)
    bool Persistent = false;

    UPROPERTY(Config)
    bool TextShadow = true;

    UPROPERTY(Config)
    bool Fade2D = true;

    UPROPERTY(Config)
    float Duration = 3.0f;

    UPROPERTY(Config)
    int DepthPriority = 0;

    UPROPERTY(Config)
    int Segments = 12;

    UPROPERTY(Config)
    float Thickness = 0.0f;

    UPROPERTY(Config)
    float ServerThickness = 2.0f;

    UPROPERTY(Config)
    float ServerColorMultiplier = 0.8f;

    UPROPERTY(Config)
    float ArrowSize = 10.0f;

    UPROPERTY(Config)
    float AxesScale = 1.0f;

    UPROPERTY(Config)
    float GradientColorIntensity = 0.0f;

    UPROPERTY(Config)
    float GradientColorSpeed = 2.0f;

    UPROPERTY(Config)
    float TextSize = 1.0f;

    UPROPERTY(Config)
    float GizmoScale = 1.0f;

    UPROPERTY(Config)
    float GizmoAxisLength = 50.0f;

    UPROPERTY(Config)
    int GizmoZLow = 0;

    UPROPERTY(Config)
    int GizmoZHigh = 100;

    UPROPERTY(Config)
    float GizmoThicknessZLow = 1.0f;

    UPROPERTY(Config)
    float GizmoThicknessZHigh = 0.0f;

    UPROPERTY(Config)
    float GizmoCursorDraggingThreshold = 4.0f;

    UPROPERTY(Config)
    float GizmoCursorSelectionThreshold = 10.0f;

    UPROPERTY(Config)
    float GizmoPlaneOffset = 25.0f;

    UPROPERTY(Config)
    float GizmoPlaneExtent = 5.0f;

    UPROPERTY(Config)
    float GizmoScaleBoxExtent = 2.0f;

    UPROPERTY(Config)
    float GizmoScaleSpeed = 0.5f;

    UPROPERTY(Config)
    float GizmoScaleMin = 0.001f;

    UPROPERTY(Config)
    float GizmoRotationRadius = 15.0f;

    UPROPERTY(Config)
    int GizmoRotationSegments = 12;

    UPROPERTY(Config)
    float GizmoGroundRaycastLength = 100000.0f;

    UPROPERTY(Config)
    TEnumAsByte<ECollisionChannel> GizmoGroundRaycastChannel = ECollisionChannel::ECC_WorldStatic;

	UPROPERTY(Config)
    float GizmoGroundRaycastCircleRadius = 5.0f;

    UPROPERTY(Config)
    FColor GizmoAxisColorsZHighX = FColor(255, 50, 50, 255);

    UPROPERTY(Config)
    FColor GizmoAxisColorsZHighY = FColor(50, 255, 50, 255);

    UPROPERTY(Config)
    FColor GizmoAxisColorsZHighZ = FColor(50, 50, 255, 255);

    UPROPERTY(Config)
    FColor GizmoAxisColorsZHighW = FColor(255, 255, 255, 255);

    UPROPERTY(Config)
    FColor GizmoAxisColorsZLowX = FColor(128, 0, 0, 255);

    UPROPERTY(Config)
    FColor GizmoAxisColorsZLowY = FColor(0, 128, 0, 255);

    UPROPERTY(Config)
    FColor GizmoAxisColorsZLowZ = FColor(0, 0, 128, 255);

    UPROPERTY(Config)
    FColor GizmoAxisColorsZLowW = FColor(128, 128, 128, 255);

    UPROPERTY(Config)
    FColor GizmoAxisColorsSelectionX = FColor(255, 255, 0, 255);

    UPROPERTY(Config)
    FColor GizmoAxisColorsSelectionY = FColor(255, 255, 0, 255);

    UPROPERTY(Config)
    FColor GizmoAxisColorsSelectionZ = FColor(255, 255, 0, 255);

    UPROPERTY(Config)
    FColor GizmoAxisColorsSelectionW = FColor(255, 255, 0, 255);

    UPROPERTY(Config)
    FColor GizmoGroundRaycastColor = FColor(128, 128, 128, 255);

    UPROPERTY(Config)
    FColor GizmoGroundRaycastCircleColor = FColor(128, 128, 128, 255);

    UPROPERTY(Config)
    TArray<FString> SecondaryBoneWildcards = {
    "interaction",
    "center_of_mass",
    "ik_*",
    "index_*",
    "middle_*",
    "pinky_*",
    "ring_*",
    "thumb_*",
    "wrist_*",
    "*_bck_*",
    "*_fwd_*",
    "*_in_*",
    "*_out_*",
    "*_pec_*",
    "*_scap_*",
    "*_bicep_*",
    "*_tricep_*",
    "*ankle*",
    "*knee*",
    "*corrective*",
    "*twist*",
    "*latissimus*",
    };
};

struct COGDEBUG_API FCogDebug
{
public:

    //----------------------------------------------------------------------------------------------------------------------
    static bool IsDebugActiveForObject(const UObject* WorldContextObject);
    
    static bool IsReplicatedDebugActiveForObject(const UObject* WorldContextObject, const AActor* ServerSelection, bool IsServerFilteringBySelection);

    static AActor* GetSelection();
    
    static void SetSelection(UWorld* World, AActor* Value);
    
    static bool GetIsFilteringBySelection();

    static void SetIsFilteringBySelection(UWorld* World, bool Value);

    static bool GetDebugPersistent(bool bPersistent);
    
    static float GetDebugDuration(bool bPersistent);
    
    static float GetDebugTextDuration(bool bPersistent);
    
    static int GetCircleSegments();
    
    static int GetDebugSegments();
    
    static float GetDebugThickness(float Thickness);
    
    static float GetDebugServerThickness(float Thickness);
    
    static uint8 GetDebugDepthPriority(float DepthPriority);
    
    static FColor ModulateDebugColor(const UWorld* World, const FColor& Color, bool bPersistent = true);
    
    static FColor ModulateServerColor(const FColor& Color);
    
    static bool IsSecondarySkeletonBone(FName BoneName);

    static void Reset();

    static FCogDebugSettings Settings;

private:

    static bool IsDebugActiveForObject_Internal(const UObject* WorldContextObject, const AActor* InSelection, bool InIsFilteringBySelection);

    static TWeakObjectPtr<AActor> Selection;
};
