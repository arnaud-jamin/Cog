#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_LogCategories.generated.h"

UCLASS()
class COGENGINE_API UCogEngineWindow_LogCategories : public UCogWindow
{
    GENERATED_BODY()

public:
    virtual void PreRender(ImGuiWindowFlags& WindowFlags) override;

    virtual void RenderContent() override;
};
