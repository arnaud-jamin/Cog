#pragma once

#include "CoreMinimal.h"

struct COGDEBUG_API FCogDebugSettings
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

    static bool Persistent;

    static bool TextShadow;

    static bool Fade2D;

    static float Duration;

    static int DepthPriority;

    static int Segments;

    static float Thickness;

    static float ServerThickness;

    static float ServerColorMultiplier;

    static float ArrowSize;

    static float AxesScale;

    static float GradientColorIntensity;

    static float GradientColorSpeed;

    static float TextSize;

    static TArray<FString> SecondaryBoneWildcards;

private:

    static bool IsDebugActiveForObject_Internal(const UObject* WorldContextObject, const AActor* InSelection, bool InIsFilteringBySelection);

    static TWeakObjectPtr<AActor> Selection;

    static bool bIsFilteringBySelection;
};
