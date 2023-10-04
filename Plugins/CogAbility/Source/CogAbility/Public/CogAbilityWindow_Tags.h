#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Tags.generated.h"

UCLASS()
class COGABILITY_API UCogAbilityWindow_Tags : public UCogWindow
{
    GENERATED_BODY()

protected:
    virtual void RenderHelp() override;

    virtual void PreRender(ImGuiWindowFlags& WindowFlags) override;

    virtual void RenderContent() override;

    virtual void DrawTagContainer(const FString& Title, const UAbilitySystemComponent& AbilitySystemComponent, FGameplayTagContainer& TagContainer);

    virtual void DrawTag(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTag& Tag);
};
