#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
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

    virtual void GetTagContainer(FGameplayTagContainer& TagContainer) {}

    virtual void RenderHelp() override;

    virtual void PreBegin(ImGuiWindowFlags& WindowFlags) override;
    
    virtual void PostBegin() override;

    virtual void RenderContent() override;

    virtual void RenderTagContainer(const UAbilitySystemComponent& AbilitySystemComponent, FGameplayTagContainer& TagContainer);

    virtual void RenderTag(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTag& Tag);

    virtual void RenderMenu();

    TObjectPtr<UCogAbilityConfig_Tags> Config = nullptr;

    ImGuiTextFilter Filter;
};

//--------------------------------------------------------------------------------------------------------------------------
class COGABILITY_API FCogAbilityWindow_OwnedTags : public FCogAbilityWindow_Tags
{
    typedef FCogAbilityWindow_Tags Super;

    virtual void Initialize() override;

    virtual void RenderHelp() override;

    virtual void GetTagContainer(FGameplayTagContainer& TagContainer) override;
};


//--------------------------------------------------------------------------------------------------------------------------
class COGABILITY_API FCogAbilityWindow_BlockedTags : public FCogAbilityWindow_Tags
{
    typedef FCogAbilityWindow_Tags Super;

    virtual void Initialize() override;

    virtual void RenderHelp() override;

    virtual void GetTagContainer(FGameplayTagContainer& TagContainer) override;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAbilityConfig_Tags : public UCogCommonConfig
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

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAbilityConfig_OwnedTags : public UCogAbilityConfig_Tags
{
    GENERATED_BODY()
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAbilityConfig_BlockedAbilitiesTags : public UCogAbilityConfig_Tags
{
    GENERATED_BODY()
};