#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogWindowConfig.h"
#include "GameplayAbilitySpecHandle.h"
#include "CogAbilityWindow_Abilities.generated.h"

class UAbilitySystemComponent;
class UCogAbilityConfig_Abilities;
class UCogAbilityDataAsset;
class UGameplayAbility;
struct FGameplayAbilitySpec;

class COGABILITY_API FCogAbilityWindow_Abilities : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void ResetConfig() override;
    
    virtual void RenderHelp() override;

    virtual void RenderTick(float DetlaTime) override;

    virtual void RenderContent() override;

    virtual void GameTick(float DeltaTime) override;

    virtual void RenderAbiltiesMenu(AActor* Selection);

    virtual FString GetAbilityName(const UGameplayAbility* Ability);

    virtual void RenderAbilitiesTable(UAbilitySystemComponent& AbilitySystemComponent);

    virtual void RenderAbilityCooldown(const UAbilitySystemComponent& AbilitySystemComponent, UGameplayAbility& Ability);

    virtual void RenderAbilityContextMenu(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec, int Index);

    virtual void RenderOpenAbilities();

    virtual void RenderAbilityInfo(const UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec);

    virtual void ProcessAbilityActivation(const FGameplayAbilitySpecHandle& Handle);

    virtual void ActivateAbility(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec);

    virtual void DeactivateAbility(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec);

    virtual void OpenAbility(const FGameplayAbilitySpecHandle& Handle);

    virtual void CloseAbility(const FGameplayAbilitySpecHandle& Handle);

    virtual ImVec4 GetAbilityColor(const UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec);

    FGameplayAbilitySpecHandle AbilityHandleToActivate;

    FGameplayAbilitySpecHandle AbilityHandleToRemove;

    TArray<FGameplayAbilitySpecHandle> OpenedAbilities;

    TObjectPtr<const UCogAbilityDataAsset> Asset = nullptr;

    TObjectPtr<UCogAbilityConfig_Abilities> Config = nullptr;

    ImGuiTextFilter Filter;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAbilityConfig_Abilities : public UCogWindowConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool SortByName = false;

    UPROPERTY(Config)
    bool ShowActive = true;

    UPROPERTY(Config)
    bool ShowInactive = true;

    UPROPERTY(Config)
    bool ShowBlocked = true;

    UPROPERTY(Config)
    FVector4f ActiveAbilityColor = FVector4f(0.0f, 1.0f, 0.5f, 1.0f);

    UPROPERTY(Config)
    FVector4f InactiveAbilityColor = FVector4f(1.0f, 1.0f, 1.0f, 1.0f);

    UPROPERTY(Config)
    FVector4f BlockedAbilityColor = FVector4f(1.0f, 0.5f, 0.5f, 1.0f);

    UPROPERTY(Config)
    FVector4f DefaultTagsColor = FVector4f(0.6f, 0.6f, 0.6f, 1.0f);

    UPROPERTY(Config)
    FVector4f BlockedTagsColor = FVector4f(1.0f, 0.5f, 0.5f, 1.0f);

    virtual void Reset() override
    {
        Super::Reset();

        SortByName = false;
        ShowActive = true;
        ShowInactive = true;
        ShowBlocked = true;
        ActiveAbilityColor = FVector4f(0.0f, 1.0f, 0.5f, 1.0f);
        InactiveAbilityColor = FVector4f(1.0f, 1.0f, 1.0f, 1.0f);
        BlockedAbilityColor = FVector4f(1.0f, 0.5f, 0.5f, 1.0f);
        DefaultTagsColor = FVector4f(0.6f, 0.6f, 0.6f, 1.0f);
        BlockedTagsColor = FVector4f(1.0f, 0.5f, 0.5f, 1.0f);
    }
};