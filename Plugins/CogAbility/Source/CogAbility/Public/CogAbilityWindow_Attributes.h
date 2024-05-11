#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Attributes.generated.h"

class UAbilitySystemComponent;
class UCogAbilityConfig_Attributes;
class UCogAbilityConfig_Alignment;
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

private:

    ImGuiTextFilter Filter;

    TObjectPtr<UCogAbilityConfig_Attributes> Config = nullptr;

    TObjectPtr<UCogAbilityConfig_Alignment> AlignmentConfig = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAbilityConfig_Attributes : public UCogCommonConfig
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