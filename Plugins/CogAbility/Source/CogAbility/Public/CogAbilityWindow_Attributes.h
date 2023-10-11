#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Attributes.generated.h"

class UCogAbilityDataAsset;

UCLASS(Config = Cog)
class COGABILITY_API UCogAbilityWindow_Attributes : public UCogWindow
{
    GENERATED_BODY()

public:

        UCogAbilityWindow_Attributes();

        TWeakObjectPtr<const UCogAbilityDataAsset> Asset;

protected:

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
};
