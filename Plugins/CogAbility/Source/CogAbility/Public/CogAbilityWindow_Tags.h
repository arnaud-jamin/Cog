#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Tags.generated.h"

UCLASS()
class COGABILITY_API UCogAbilityWindow_Tags : public UCogWindow
{
    GENERATED_BODY()

public:
    UCogAbilityWindow_Tags();

    virtual void PreRender(ImGuiWindowFlags& WindowFlags) override;
    virtual void RenderContent() override;
};
