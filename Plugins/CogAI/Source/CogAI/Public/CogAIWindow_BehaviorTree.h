#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "CogAIWindow_BehaviorTree.generated.h"

class UBehaviorTreeComponent;
class UBTNode;
class UCogAIConfig_BehaviorTree;

//--------------------------------------------------------------------------------------------------------------------------
class COGAI_API FCogAIWindow_BehaviorTree : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void GameTick(float DeltaTime) override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void RenderNode(UBehaviorTreeComponent& BehaviorTreeComponent, UBTNode* Node, bool OpenAllChildren);

private:

    ImGuiTextFilter Filter;

    TObjectPtr<UCogAIConfig_BehaviorTree> Config = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAIConfig_BehaviorTree : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    FVector4f ActiveColor = FVector4f(1.0f, 1.0f, 1.0f, 1.0f);

    UPROPERTY(Config)
    FVector4f InactiveColor = FVector4f(1.0f, 1.0f, 1.0f, 0.2f);

    virtual void Reset() override
    {
        Super::Reset();

        ActiveColor = FVector4f(1.0f, 1.0f, 1.0f, 1.0f);
        InactiveColor = FVector4f(1.0f, 1.0f, 1.0f, 0.2f);
    }
};