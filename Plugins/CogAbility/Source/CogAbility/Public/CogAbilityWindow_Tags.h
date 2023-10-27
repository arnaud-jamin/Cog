#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"

class UAbilitySystemComponent;
struct FGameplayTagContainer;
struct FGameplayTag;

//--------------------------------------------------------------------------------------------------------------------------
class COGABILITY_API FCogAbilityWindow_Tags : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void DrawTagContainer(const FString& TagContainerName, const UAbilitySystemComponent& AbilitySystemComponent, FGameplayTagContainer& TagContainer);

    virtual void DrawTag(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTag& Tag);
};
