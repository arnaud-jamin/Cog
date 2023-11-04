#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogWindowConfig.h"
#include "CogAbilityWindow_Tags.generated.h"

struct FGameplayTagContainer;
struct FGameplayTag;
class UCogAbilityConfig_Tags;
class UAbilitySystemComponent;

//--------------------------------------------------------------------------------------------------------------------------
class COGABILITY_API FCogAbilityWindow_Tags : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void ResetConfig();

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void RenderTagContainer(const FString& TagContainerName, const UAbilitySystemComponent& AbilitySystemComponent, FGameplayTagContainer& TagContainer);

    virtual void RenderTag(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTag& Tag);

    virtual void RenderMenu();

    TObjectPtr<UCogAbilityConfig_Tags> Config = nullptr;

    ImGuiTextFilter Filter;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAbilityConfig_Tags : public UCogWindowConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool SortByName = false;

    virtual void Reset() override
    {
        Super::Reset();

        SortByName = false;
    }
};