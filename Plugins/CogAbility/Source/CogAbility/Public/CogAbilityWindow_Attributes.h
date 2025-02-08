#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "AttributeSet.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Attributes.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class UCogAbilityConfig_Attributes;
class UCogAbilityConfig_Alignment;
struct FModifierSpec;
struct FGameplayModifierInfo;

//--------------------------------------------------------------------------------------------------------------------------
class COGABILITY_API FCogAbilityWindow_Attributes : public FCogWindow
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

    virtual void RenderAttributeDetails(const UAbilitySystemComponent& AbilitySystemComponent, const char* AttributeSetName, const FGameplayAttribute& Attribute, bool IsForTooltip);

    virtual void RenderOpenAttributes();

    virtual void FormatAttributeSetName(FString& AttributeSetName);

    virtual void OpenAttributeDetails(const FGameplayAttribute& InAttribute);

    virtual void CloseAttributeDetails(const FGameplayAttribute& InAttribute);

    virtual void RenderAttributeContextMenu(UAbilitySystemComponent& AbilitySystemComponent, const FGameplayAttribute& InAttribute, int Index);

private:

    ImGuiTextFilter Filter;

    TObjectPtr<UCogAbilityConfig_Attributes> Config = nullptr;

    TObjectPtr<UCogAbilityConfig_Alignment> AlignmentConfig = nullptr;

    TArray<FGameplayAttribute> OpenedAttributes;
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

    UPROPERTY(Config)
    FString AttributeSetPrefixes;

    UPROPERTY(Config)
    FVector4f CategoryColor = FVector4f(1.0f, 1.0f, 1.0f, 0.5f);

    UPROPERTY(Config)
    FVector4f AttributeSetColor = FVector4f(1.0f, 1.0f, 1.0f, 0.5f);

    virtual void Reset() override
    {
        Super::Reset();

        SortByName = true;
        GroupByAttributeSet = false;
        GroupByCategory = false;
        ShowOnlyModified = false;
        AttributeSetPrefixes = FString();
        CategoryColor = FVector4f(1.0f, 1.0f, 1.0f, 0.5f);
        AttributeSetColor = FVector4f(1.0f, 1.0f, 1.0f, 0.5f);
    }
};