#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAIWindow_Blackboard.generated.h"

UCLASS(Config = Cog)
class COGAI_API UCogAIWindow_Blackboard : public UCogWindow
{
    GENERATED_BODY()

public:

    UCogAIWindow_Blackboard();

protected:

    void RenderHelp();

    virtual void RenderContent() override;

private:

    UPROPERTY(Config)
    bool bSortByName = true;

    ImGuiTextFilter Filter;
};
