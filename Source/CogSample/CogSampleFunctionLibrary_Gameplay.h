#pragma once

#include "CoreMinimal.h"
#include "CogSampleFunctionLibrary_Gameplay.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
struct FGameplayAbilitySpecHandle;
struct FGameplayAttribute;
struct FGameplayAttributeData;
struct FGameplayCueNotify_SpawnResult;
struct FGameplayCueParameters;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) 

UCLASS(meta = (ScriptName = "CogSampleFunctionLibrary_Gameplay"))
class UCogSampleFunctionLibrary_Gameplay : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    static void AdjustAttributeForMaxChange(UAbilitySystemComponent* AbilityComponent, FGameplayAttributeData& AffectedAttribute, float OldValue, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

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
};
