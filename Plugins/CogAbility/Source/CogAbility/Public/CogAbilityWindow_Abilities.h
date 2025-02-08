#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
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

    virtual void RenderHelp() override;

    virtual void PreBegin(ImGuiWindowFlags& WindowFlags) override;

    virtual void PostBegin() override;

    virtual void RenderTick(float DeltaTime) override;

    virtual void RenderContent() override;

    virtual void GameTick(float DeltaTime) override;

    virtual void RenderAbilitiesMenu(AActor* Selection);

    virtual void RenderAbilitiesMenuFilters();

    virtual void RenderAbilitiesMenuColorSettings();

    virtual FString GetAbilityName(const UGameplayAbility* Ability);
    
    virtual void RenderAbilitiesTable(UAbilitySystemComponent& AbilitySystemComponent);

    virtual bool RenderAbilitiesTableHeader(UAbilitySystemComponent& AbilitySystemComponent);

    virtual void RenderAbilitiesTableRow(UAbilitySystemComponent& AbilitySystemComponent, int& SelectedIndex, int Index,
        FGameplayAbilitySpec& Spec, UGameplayAbility* Ability);

    virtual void RenderAbilityActivation(FGameplayAbilitySpec& Spec);

    virtual void RenderAbilitiesTableAbilityName(UAbilitySystemComponent& AbilitySystemComponent, int& SelectedIndex, int Index, FGameplayAbilitySpec& Spec, UGameplayAbility* Ability);

    virtual void RenderAbilitiesTableAbilityBlocking(UAbilitySystemComponent& AbilitySystemComponent, UGameplayAbility* Ability);

    virtual void RenderAbilityLevel(FGameplayAbilitySpec& Spec);

    virtual void RenderAbilityInputPressed(FGameplayAbilitySpec& Spec);

    virtual void RenderAbilityCooldown(const UAbilitySystemComponent& AbilitySystemComponent, UGameplayAbility& Ability);

    virtual void RenderAbilityContextMenu(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec, int Index);

    virtual void RenderOpenAbilities();

    virtual void RenderAbilityInfo(const UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec);

    virtual void RenderAbilityAdditionalInfo(const UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec, UGameplayAbility& Ability, const ImVec4& TextColor);

    virtual void ProcessAbilityActivation(const FGameplayAbilitySpecHandle& Handle);

    virtual void ActivateAbility(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec);

    virtual void DeactivateAbility(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec);

    virtual void OpenAbilityDetails(const FGameplayAbilitySpecHandle& Handle);

    virtual void CloseAbilityDetails(const FGameplayAbilitySpecHandle& Handle);

    virtual ImVec4 GetAbilityColor(const UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec);

    virtual bool ShouldShowAbility(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec,
        const UGameplayAbility* Ability);

    FGameplayAbilitySpecHandle AbilityHandleToActivate;

    FGameplayAbilitySpecHandle AbilityHandleToRemove;

    TArray<FGameplayAbilitySpecHandle> OpenedAbilities;

    TObjectPtr<const UCogAbilityDataAsset> Asset = nullptr;

    TObjectPtr<UCogAbilityConfig_Abilities> Config = nullptr;

    ImGuiTextFilter Filter;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAbilityConfig_Abilities : public UCogCommonConfig
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
    bool ShowPressed = true;

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

    UPROPERTY(Config)
    FVector4f InputPressedColor = FVector4f(0.0f, 1.0f, 0.5f, 1.0f);

    virtual void Reset() override
    {
        Super::Reset();

        SortByName = false;
        ShowActive = true;
        ShowInactive = true;
        ShowPressed = true;
        ShowBlocked = true;
        ActiveAbilityColor = FVector4f(0.0f, 1.0f, 0.5f, 1.0f);
        InactiveAbilityColor = FVector4f(1.0f, 1.0f, 1.0f, 1.0f);
        BlockedAbilityColor = FVector4f(1.0f, 0.5f, 0.5f, 1.0f);
        DefaultTagsColor = FVector4f(0.6f, 0.6f, 0.6f, 1.0f);
        BlockedTagsColor = FVector4f(1.0f, 0.5f, 0.5f, 1.0f);
        InputPressedColor = FVector4f(0.0f, 1.0f, 0.5f, 1.0f);
    }
};