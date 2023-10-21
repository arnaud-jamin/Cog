#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "GameplayAbilitySpecHandle.h"
#include "CogAbilityWindow_Abilities.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UCogAbilityDataAsset;
struct FGameplayAbilitySpec;

UCLASS()
class COGABILITY_API UCogAbilityWindow_Abilities : public UCogWindow
{
    GENERATED_BODY()

public:

    UCogAbilityWindow_Abilities();

    const UCogAbilityDataAsset* GetAsset() const { return Asset.Get(); }

    void SetAsset(const UCogAbilityDataAsset* Value) { Asset = Value; }

protected:

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

private:

    FGameplayAbilitySpecHandle AbilityHandleToActivate;

    FGameplayAbilitySpecHandle AbilityHandleToRemove;

    TArray<FGameplayAbilitySpecHandle> OpenedAbilities;

    UPROPERTY()
    TObjectPtr<const UCogAbilityDataAsset> Asset = nullptr;
};
