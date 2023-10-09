#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "CogSampleAbilitySystemGlobals.generated.h"

UCLASS()
class UCogSampleAbilitySystemGlobals : public UAbilitySystemGlobals
{
    GENERATED_BODY()

    virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
