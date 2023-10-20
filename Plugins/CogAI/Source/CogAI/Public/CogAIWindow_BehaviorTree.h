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

    virtual void GameTick(float DeltaTime) override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void RenderNode(UBehaviorTreeComponent& BehaviorTreeComponent, UBTNode* Node, bool OpenAllChildren);

private:

    UPROPERTY(Config)
    FVector4f ActiveColor = FVector4f(1.0f, 1.0f, 1.0f, 1.0f);

    UPROPERTY(Config)
    FVector4f InactiveColor = FVector4f(1.0f, 1.0f, 1.0f, 0.2f);

    ImGuiTextFilter Filter;
};
