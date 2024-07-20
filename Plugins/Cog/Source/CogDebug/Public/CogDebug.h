#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "CogDebug.generated.h"

class AActor;
class UObject;
class UWorld;
struct FCogDebugDrawLineTraceParams;
struct FCogDebugDrawOverlapParams;
struct FCogDebugDrawSweepParams;

UENUM()
enum ECogDebugRecolorMode : uint8
{
    CDRM_None,
    CDRM_Color,
    CDRM_HueOverTime,
    CDRM_HueOverFrames,
};

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
    TEnumAsByte<ECogDebugRecolorMode> RecolorMode = CDRM_None;

    UPROPERTY(Config)
    float RecolorIntensity = 0.0f;

    UPROPERTY(Config)
    FColor RecolorColor = FColor(255, 0, 0, 255);

    UPROPERTY(Config)
    float RecolorTimeSpeed = 2.0f;

    UPROPERTY(Config)
    int32 RecolorFrameCycle = 6;

    UPROPERTY(Config)
    float TextSize = 1.0f;

    UPROPERTY(Config)
    bool ActorNameUseLabel = true;

    UPROPERTY(Config)
    float GizmoScale = 1.0f;

    UPROPERTY(Config)
    bool GizmoUseLocalSpace = false;

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
    float GizmoTranslationAxisLength = 80.0f;

    UPROPERTY(Config)
    bool GizmoTranslationSnapEnable = false;

    UPROPERTY(Config)
    float GizmoTranslationSnapValue = 10.0f;

    UPROPERTY(Config)
    float GizmoTranslationPlaneOffset = 18.0f;

    UPROPERTY(Config)
    float GizmoTranslationPlaneExtent = 5.0f;

    UPROPERTY(Config)
    bool GizmoRotationSnapEnable = false;

    UPROPERTY(Config)
    float GizmoRotationSnapValue = 10.0f;

    UPROPERTY(Config)
    float GizmoRotationSpeed = 1.0f;

    UPROPERTY(Config)
    float GizmoRotationRadius = 40.0f;

    UPROPERTY(Config)
    int GizmoRotationSegments = 8;

    UPROPERTY(Config)
    bool GizmoScaleSnapEnable = false;

    UPROPERTY(Config)
    float GizmoScaleSnapValue = 1.0f;

    UPROPERTY(Config)
    float GizmoScaleBoxOffset = 85.0f;

    UPROPERTY(Config)
    float GizmoScaleBoxExtent = 5.0f;

    UPROPERTY(Config)
    float GizmoScaleSpeed = 0.01f;

    UPROPERTY(Config)
    float GizmoScaleMin = 0.001f;

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
    FColor GizmoTextColor = FColor(255, 255, 255, 255);

    UPROPERTY(Config)
    FColor CollisionQueryHitColor = FColor::Green;

    UPROPERTY(Config)
    FColor CollisionQueryNoHitColor = FColor::Red;

    UPROPERTY(Config)
    bool CollisionQueryDrawHitPrimitives = true;

    UPROPERTY(Config)
    bool CollisionQueryDrawHitPrimitiveActorsName = false;

    UPROPERTY(Config)
    bool CollisionQueryHitPrimitiveActorsNameShadow = true;

    UPROPERTY(Config)
    float CollisionQueryHitPrimitiveActorsNameSize = 1.0f;

    UPROPERTY(Config)
    bool CollisionQueryDrawHitLocation = true;

    UPROPERTY(Config)
    bool CollisionQueryDrawHitImpactPoints = true;

    UPROPERTY(Config)
    bool CollisionQueryDrawHitNormals = true;

    UPROPERTY(Config)
    bool CollisionQueryDrawHitImpactNormals = true;

    UPROPERTY(Config)
    float CollisionQueryHitPointSize = 5.0f;

    UPROPERTY(Config)
    FColor CollisionQueryNormalColor = FColor::Yellow;

    UPROPERTY(Config)
    FColor CollisionQueryImpactNormalColor = FColor::Cyan;

    UPROPERTY(Config)
    bool CollisionQueryDrawHitShapes = true;

    UPROPERTY(Config)
    FColor ChannelColorWorldStatic = FColor(255, 0, 0, 5);

    UPROPERTY(Config)
    FColor ChannelColorWorldDynamic = FColor(255, 0, 188, 5);

    UPROPERTY(Config)
    FColor ChannelColorPawn = FColor(105, 0, 255, 5);

    UPROPERTY(Config)
    FColor ChannelColorVisibility = FColor(0, 15, 255, 5);

    UPROPERTY(Config)
    FColor ChannelColorCamera = FColor(0, 105, 255, 5);

    UPROPERTY(Config)
    FColor ChannelColorPhysicsBody = FColor(0, 255, 208, 5);

    UPROPERTY(Config)
    FColor ChannelColorVehicle = FColor(52, 255, 0, 5);

    UPROPERTY(Config)
    FColor ChannelColorDestructible = FColor(255, 255, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorEngineTraceChannel1 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorEngineTraceChannel2 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorEngineTraceChannel3 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorEngineTraceChannel4 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorEngineTraceChannel5 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorEngineTraceChannel6 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel1 = FColor(255, 105, 0, 5);

	UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel2 = FColor(255, 30, 0, 5);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel3 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel4 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel5 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel6 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel7 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel8 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel9 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel10 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel11 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel12 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel13 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel14 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel15 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel16 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel17 = FColor(0, 0, 0, 0);

    UPROPERTY(Config)
    FColor ChannelColorGameTraceChannel18 = FColor(0, 0, 0, 0);

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
    
    static void SetSelection(const UWorld* World, AActor* Value);
    
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

    static void GetDebugChannelColors(FColor ChannelColors[ECC_MAX]);

    static void GetDebugDrawOverlapSettings(FCogDebugDrawOverlapParams& Params);

    static void GetDebugDrawLineTraceSettings(FCogDebugDrawLineTraceParams& Params);

    static void GetDebugDrawSweepSettings(FCogDebugDrawSweepParams& Params);

    static FCogDebugSettings Settings;

private:

    static bool IsDebugActiveForObject_Internal(const UObject* WorldContextObject, const AActor* InSelection, bool InIsFilteringBySelection);

    static TWeakObjectPtr<AActor> Selection;
};
