#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Attributes.generated.h"

class UAbilitySystemComponent;
class UCogAbilityDataAsset;
struct FGameplayAttribute;
struct FModifierSpec;
struct FGameplayModifierInfo;

UCLASS(Config = Cog)
class COGABILITY_API UCogAbilityWindow_Attributes : public UCogWindow
{
    GENERATED_BODY()

public:

    UCogAbilityWindow_Attributes();

    const UCogAbilityDataAsset* GetAsset() const { return Asset.Get(); }

    void SetAsset(const UCogAbilityDataAsset* Value) { Asset = Value; }

protected:

    virtual void ResetConfig() override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void DrawAttributeInfo(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayAttribute& Attribute);

    virtual ImVec4 GetEffectModifierColor(const FModifierSpec& ModSpec, const FGameplayModifierInfo& ModInfo, float BaseValue) const;

    virtual ImVec4 GetAttributeColor(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayAttribute& Attribute) const;

private:

    UPROPERTY(Config)
    bool bSortByNameSetting = true;

    UPROPERTY(Config)
    bool bGroupByAttributeSetSetting = false;

    UPROPERTY(Config)
    bool bGroupByCategorySetting = false;

    UPROPERTY(Config)
    bool bShowOnlyModified = false;

    ImGuiTextFilter Filter;

    UPROPERTY()
    TWeakObjectPtr<const UCogAbilityDataAsset> Asset = nullptr;
};
