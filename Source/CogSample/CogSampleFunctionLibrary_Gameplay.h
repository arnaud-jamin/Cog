#pragma once

#include "CoreMinimal.h"
#include "CogSampleDefines.h"
#include "CollisionQueryParams.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CogSampleFunctionLibrary_Gameplay.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;
struct FGameplayAbilitySpecHandle;
struct FGameplayAttribute;
struct FGameplayAttributeData;
struct FGameplayCueNotify_SpawnResult;
struct FGameplayCueParameters;
struct FGameplayEffectSpecHandle;
struct FGameplayTagContainer;

//--------------------------------------------------------------------------------------------------------------------------
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) 

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(meta = (ScriptName = "CogSampleFunctionLibrary_Gameplay"))
class UCogSampleFunctionLibrary_Gameplay : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure)
    static FVector GetActorBottomLocation(const AActor* Actor);

    UFUNCTION(BlueprintCallable)
    static bool FindAbilitySpecHandleFromClass(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> AbilityClass, FGameplayAbilitySpecHandle& SpecHandle);

    UFUNCTION(BlueprintCallable)
    static void ApplyAllGameplayEffectContextValues(const FGameplayCueParameters& Parameters, const FGameplayCueNotify_SpawnResult& SpawnResult);

    /* Same as UAbilitySystemBlueprintLibrary::SendGameplayEventToActor but doesn't log an error when the ability system
    * is not found. When a montage is played in the montage editor the notifications are executed and if a notification
    * uses SendGameplayEventToActor it will log an error. */
    UFUNCTION(BlueprintCallable)
    static void SendMontageEvent(AActor* Actor, FGameplayTag EventTag, FGameplayEventData Payload);

    UFUNCTION(BlueprintPure)
    static float GetFloatValue(const FScalableFloat& ScalableFloat, int32 Level);

    UFUNCTION(BlueprintPure)
    static bool GetBoolValue(const FScalableFloat& ScalableFloat, int32 Level);

    UFUNCTION(BlueprintPure)
    static int32 GetIntValue(const FScalableFloat& ScalableFloat, int32 Level);

    UFUNCTION(BlueprintPure)
    static FVector GetActorTargetLocation(const AActor* Actor);

    UFUNCTION(BlueprintPure)
    static float AngleBetweenVector2D(FVector2D A, FVector2D B);

    UFUNCTION(BlueprintPure)
    static void FindSegmentPointDistance(const FVector2D& Segment1, const FVector2D& Segment2, const FVector2D& Point, FVector2D& Projection, float& Time, float& Distance);

    UFUNCTION(BlueprintPure)
    static void FindCapsulePointDistance(const FVector2D& CapsulePoint1, const FVector2D& CapsulePoint2, const float CapsuleRadius, const FVector2D& Point, FVector2D& Projection, float& Time, float& Distance);

    UFUNCTION(BlueprintPure)
    static AActor* GetInstigator(const AActor* Actor);

    UFUNCTION(BlueprintPure)
    static int32 GetProgressionLevel(const AActor* Actor);
    
    UFUNCTION(BlueprintCallable)
    static void SetProgressionLevel(AActor* Actor, int32 Value);

    UFUNCTION(BlueprintPure)
    static AActor* GetCreator(const AActor* Actor);
    
    UFUNCTION(BlueprintCallable)
    static void SetCreator(AActor* Actor, AActor* Value);

    UFUNCTION(BlueprintCallable)
    static bool IsAlive(const AActor* Actor);

    UFUNCTION(BlueprintCallable)
    static bool IsDead(const AActor* Actor);

    UFUNCTION(BlueprintCallable)
    static const UGameplayEffectComponent* GetEffectComponent(const UGameplayEffect* Effect, TSubclassOf<UGameplayEffectComponent> ClassToFind);

    static void AdjustAttributeForMaxChange(UAbilitySystemComponent* AbilityComponent, FGameplayAttributeData& AffectedAttribute, float OldValue, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

    static FCollisionObjectQueryParams ConfigureCollisionObjectParams(const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes);

    static FVector2D ViewportToScreen(const FVector2D& value, const FVector2D& displaySize);

    static float ViewportToScreen(const float value, const FVector2D& displaySize);

    static FVector2D ScreenToViewport(const FVector2D& value, const FVector2D& displaySize);

    static float ScreenToViewport(const float value, const FVector2D& displaySize);

    static bool HasLineOfSight(const UWorld* World, const FVector& Start, const FVector& End, const FCollisionObjectQueryParams& BlockersParams, const FCollisionQueryParams& QueryParams, const COG_LOG_CATEGORY& LogCategory);

    static bool IsActorAbilitySystemMatchingTags(const UAbilitySystemComponent* AbilitySystem, const FGameplayTagContainer& RequiredTags, const FGameplayTagContainer& IgnoredTags);

    static bool IsActorMatchingTags(const AActor* Actor, const FGameplayTagContainer& RequiredTags, const FGameplayTagContainer& IgnoredTags);

    static void MakeOutgoingSpecs(const AActor* Actor, const TArray<TSubclassOf<UGameplayEffect>>& Effects, const TArray<FGameplayEffectSpecHandle>& BakedEffects, TMap<TSubclassOf<UGameplayEffect>, FGameplayEffectSpecHandle>& Result);

    static APlayerController* GetFirstLocalPlayerController(UObject* WorldContextObject);
};
