#include "CogSampleAbilitySystemGlobals.h"

#include "CogSampleGameplayEffectContext.h"

FGameplayEffectContext* UCogSampleAbilitySystemGlobals::AllocGameplayEffectContext() const
{
    return new FCogSampleGameplayEffectContext();
}