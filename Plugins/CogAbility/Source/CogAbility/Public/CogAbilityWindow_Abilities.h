#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Abilities.generated.h"

class UGameplayAbility;
class UCogAbilityDataAsset_Abilities;
struct FGameplayAbilitySpec;

UCLASS()
class COGABILITY_API UCogAbilityWindow_Abilities : public UCogWindow
{
    GENERATED_BODY()

public:
    
    TWeakObjectPtr<UCogAbilityDataAsset_Abilities> AbilitiesAsset;

protected:

    virtual void RenderHelp() override;

    virtual void PreRender(ImGuiWindowFlags& WindowFlags) override;

    virtual void RenderTick(float DetlaTime) override;

    virtual void RenderContent() override;

    virtual void GameTick(float DeltaTime) override;

    virtual void RenderAbiltiesMenu(AActor* Selection);

    virtual FString GetAbilityName(const UGameplayAbility* Ability);

    virtual void RenderAbilitiesTable(UAbilitySystemComponent& AbilitySystemComponent);

    virtual void RenderAbilityCooldown(const UAbilitySystemComponent& AbilitySystemComponent, UGameplayAbility& Ability);

    virtual void RenderAbilityContextMenu(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec, int Index);

    virtual void RenderOpenAbilities();

    virtual void RenderAbilityInfo(FGameplayAbilitySpec& Spec);

    virtual void ProcessAbilityActivation(FGameplayAbilitySpecHandle Handle);

    virtual void ActivateAbility(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec);

    virtual void DeactivateAbility(UAbilitySystemComponent& AbilitySystemComponent, FGameplayAbilitySpec& Spec);

    virtual void OpenAbility(FGameplayAbilitySpecHandle Handle);

    virtual void CloseAbility(FGameplayAbilitySpecHandle Handle);

private:

    FGameplayAbilitySpecHandle AbilityHandleToActivate;

    TArray<FGameplayAbilitySpecHandle> OpenedAbilities;
};
