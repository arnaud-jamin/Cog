#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Attributes.generated.h"

class UAbilitySystemComponent;
class UCogAbilityDataAsset;
struct FGameplayAttribute;

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
