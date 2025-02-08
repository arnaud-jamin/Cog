#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "imgui.h"
#include "CogAbilityWindow_Effects.generated.h"

class UAbilitySystemComponent;
class UCogAbilityConfig_Alignment;
class UCogAbilityConfig_Effects;
class UCogAbilityDataAsset;
class UGameplayEffect;
namespace EGameplayModOp { enum Type : int; };
struct FActiveGameplayEffect;
struct FGameplayModifierInfo;
struct FModifierSpec;

//--------------------------------------------------------------------------------------------------------------------------
class COGABILITY_API FCogAbilityWindow_Effects : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void RenderTick(float DeltaTime) override;

    virtual void PreBegin(ImGuiWindowFlags& WindowFlags) override;

    virtual void PostBegin() override;

    virtual void RenderEffectsTable();

    virtual void RenderEffectRow(UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffectHandle& ActiveHandle, int32 Index, int32& Selected);

    virtual void RenderEffectInfo(const UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffect& ActiveEffect, const UGameplayEffect& Effect);
    
    virtual void RenderRemainingTime(const UAbilitySystemComponent& AbilitySystemComponent, const FActiveGameplayEffect& ActiveEffect);

    virtual void RenderStacks(const FActiveGameplayEffect& ActiveEffect, const UGameplayEffect& Effect);

    virtual void RenderPrediction(const FActiveGameplayEffect& ActiveEffect, bool Short);

    virtual void RenderInhibition(const FActiveGameplayEffect& ActiveEffect, bool Short);

    virtual FString GetEffectName(const UGameplayEffect& Effect);
    
    virtual FString GetEffectNameSafe(const UGameplayEffect* Effect);

    virtual void RenderOpenEffects();

    virtual void OpenEffect(const FActiveGameplayEffectHandle& Handle);

    virtual void CloseEffect(const FActiveGameplayEffectHandle& Handle);

    TObjectPtr<const UCogAbilityDataAsset> Asset = nullptr;

    TObjectPtr<UCogAbilityConfig_Effects> Config = nullptr;

    TObjectPtr<UCogAbilityConfig_Alignment> AlignmentConfig = nullptr;

    TArray<FActiveGameplayEffectHandle> OpenedEffects;

    ImGuiTextFilter Filter;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAbilityConfig_Effects : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool SortByName = true;

    UPROPERTY(Config)
    bool SortByAlignment = true;

    virtual void Reset() override
    {
        Super::Reset();

        SortByName = true;
        SortByAlignment = true;
    }
};