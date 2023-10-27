#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogWindowConfig.h"
#include "CogAbilityWindow_Attributes.generated.h"

class UAbilitySystemComponent;
class UCogAbilityDataAsset;
struct FGameplayAttribute;
struct FModifierSpec;
struct FGameplayModifierInfo;

//--------------------------------------------------------------------------------------------------------------------------
class COGABILITY_API FCogAbilityWindow_Attributes : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void ResetConfig() override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void DrawAttributeInfo(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayAttribute& Attribute);

    virtual ImVec4 GetEffectModifierColor(const FModifierSpec& ModSpec, const FGameplayModifierInfo& ModInfo, float BaseValue) const;

    virtual ImVec4 GetAttributeColor(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayAttribute& Attribute) const;

private:

    ImGuiTextFilter Filter;

    TObjectPtr<const UCogAbilityDataAsset> Asset = nullptr;
    
    TObjectPtr<UCogAbilityConfig_Attributes> Config = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAbilityConfig_Attributes : public UCogWindowConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool SortByName = true;

    UPROPERTY(Config)
    bool GroupByAttributeSet = false;

    UPROPERTY(Config)
    bool GroupByCategory = false;

    UPROPERTY(Config)
    bool ShowOnlyModified = false;

    virtual void Reset() override
    {
        Super::Reset();

        SortByName = true;
        GroupByAttributeSet = false;
        GroupByCategory = false;
        ShowOnlyModified = false;
    }
};