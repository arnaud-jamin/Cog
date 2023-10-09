#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "CogSampleTask_ListenForGameplayEffectChanges.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCogSampleOnGameplayEffectChangesDelegate, FActiveGameplayEffectHandle, Handle)
;
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class UCogSampleTask_ListenForGameplayEffectChanges : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()
public:

    UPROPERTY(BlueprintAssignable)
    FCogSampleOnGameplayEffectChangesDelegate OnAdded;

    UPROPERTY(BlueprintAssignable)
    FCogSampleOnGameplayEffectChangesDelegate OnRemoved;

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
    static UCogSampleTask_ListenForGameplayEffectChanges* ListenForGameplayEffectChanges(UAbilitySystemComponent* AbilitySystemComponent);

    virtual void Activate() override;

    UFUNCTION(BlueprintCallable)
    void EndTask();

protected:

    UPROPERTY()
    UAbilitySystemComponent* AbilitySystemComponent = nullptr;

    void OnGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle Handle);
    void OnGameplayEffectRemoved(const FActiveGameplayEffect& RemovedGameplayEffect);

};