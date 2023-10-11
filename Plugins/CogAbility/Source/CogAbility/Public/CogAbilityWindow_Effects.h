#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Effects.generated.h"

class UCogAbilityDataAsset;

UCLASS()
class COGABILITY_API UCogAbilityWindow_Effects : public UCogWindow
{
    GENERATED_BODY()

public:

    UCogAbilityWindow_Effects();

    TWeakObjectPtr<const UCogAbilityDataAsset> Asset;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void RenderEffectsTable();

    virtual void RenderEffectRow(const UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffectHandle& ActiveHandle, int32 Index, int32& Selected);

    virtual void RenderEffectInfo(const UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffect& ActiveEffect, const UGameplayEffect& Effect);
    
    virtual void RenderTagContainer(const FGameplayTagContainer& Container);

    virtual void RenderRemainingTime(const UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffect& ActiveEffect);

    virtual void RenderStacks(const FActiveGameplayEffect& ActiveEffect, const UGameplayEffect& Effect);

    virtual void RenderPrediction(const FActiveGameplayEffect& ActiveEffect, bool Short);

    virtual FString GetEffectName(const UGameplayEffect& Effect);
};
