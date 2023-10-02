#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Effects.generated.h"

UCLASS()
class COGABILITY_API UCogAbilityWindow_Effects : public UCogWindow
{
    GENERATED_BODY()

public:

    FGameplayTag NegativeEffectTag;

    FGameplayTag PositiveEffectTag;

protected:

    virtual void PreRender(ImGuiWindowFlags& WindowFlags) override;

    virtual void RenderContent() override;

    virtual void DrawEffectRow(const UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffectHandle& ActiveHandle, int32 Index, int32& Selected);

    virtual void DrawEffectInfo(const UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffect& ActiveEffect, const UGameplayEffect& Effect);
    
    virtual void DrawTagContainer(const FGameplayTagContainer& Container);

    virtual FString GetEffectName(const UGameplayEffect& Effect);

    virtual void DrawRemainingTime(const UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffect& ActiveEffect);

    virtual void DrawStacks(const FActiveGameplayEffect& ActiveEffect, const UGameplayEffect& Effect);

    virtual void DrawPrediction(const FActiveGameplayEffect& ActiveEffect, bool Short);
};
