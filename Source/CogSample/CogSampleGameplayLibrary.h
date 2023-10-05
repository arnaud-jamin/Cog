#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CogSampleGameplayLibrary.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
struct FGameplayAbilitySpecHandle;
struct FGameplayCueParameters;
struct FGameplayCueNotify_SpawnResult;

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(meta = (ScriptName = "CogSampleGameplayLibrary"))
class UCogSampleGameplayLibrary : public UBlueprintFunctionLibrary
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

};

