#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Tags.generated.h"

class UAbilitySystemComponent;
struct FGameplayTagContainer;
struct FGameplayTag;

UCLASS()
class COGABILITY_API UCogAbilityWindow_Tags : public UCogWindow
{
    GENERATED_BODY()

public:

    UCogAbilityWindow_Tags();

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void DrawTagContainer(const FString& TagContainerName, const UAbilitySystemComponent& AbilitySystemComponent, FGameplayTagContainer& TagContainer);

    virtual void DrawTag(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTag& Tag);
};
