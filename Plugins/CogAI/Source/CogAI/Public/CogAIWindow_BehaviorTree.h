#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAIWindow_BehaviorTree.generated.h"

class UBehaviorTreeComponent;
class UBTNode;

UCLASS(Config = Cog)
class COGAI_API UCogAIWindow_BehaviorTree : public UCogWindow
{
    GENERATED_BODY()

public:

    UCogAIWindow_BehaviorTree();

protected:

    void RenderHelp();

    virtual void RenderContent() override;

    virtual void RenderNode(UBehaviorTreeComponent& BehaviorTreeComponent, UBTNode* Node, bool OpenAllChildren);

private:

    ImGuiTextFilter Filter;
};
