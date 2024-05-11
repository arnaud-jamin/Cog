#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "CogAIWindow_Blackboard.generated.h"

class UCogAIConfig_Blackboard;

//--------------------------------------------------------------------------------------------------------------------------
class COGAI_API FCogAIWindow_Blackboard : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void ResetConfig() override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

private:

    ImGuiTextFilter Filter;

    TObjectPtr<UCogAIConfig_Blackboard> Config;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAIConfig_Blackboard : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool bSortByName = true;

    virtual void Reset() override
    {
        Super::Reset();

        bSortByName = true;
    }
};