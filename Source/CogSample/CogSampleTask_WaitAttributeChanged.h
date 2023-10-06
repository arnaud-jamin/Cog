#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "CogSampleTask_WaitAttributeChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChangedDelegate, FGameplayAttribute, Attribute, float, NewValue, float, OldValue)
;
/**
 * Blueprint node to automatically register a listener for all attribute changes in an AbilitySystemComponent.
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class UCogSampleTask_WaitAttributeChanged : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()
public:

    UPROPERTY(BlueprintAssignable)
    FOnAttributeChangedDelegate OnAttributeChanged;

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
    static UCogSampleTask_WaitAttributeChanged* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute);

    virtual void Activate() override;

    UFUNCTION(BlueprintCallable)
    void EndTask();

protected:

    UPROPERTY()
    UAbilitySystemComponent* AbilitySystemComponent = nullptr;

    FGameplayAttribute AttributeToListenFor;

    void AttributeChanged(const FOnAttributeChangeData& Data);

};