#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "CogEngineWindow_NetImGui.generated.h"

class UCogEngineConfig_NetImGui;

class COGENGINE_API FCogEngineWindow_NetImGui : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

    virtual void Shutdown() override;

protected:

    virtual void ResetConfig() override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

private:

    TWeakObjectPtr<UCogEngineConfig_NetImGui> Config = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_NetImGui : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    virtual void Reset() override;

    UPROPERTY(Config)
    FString ClientName = FString("cog");
    
    UPROPERTY(Config)
    FString ServerName = FString("localhost");

    UPROPERTY(Config)
    int32 ServerPort = 8888;

    UPROPERTY(Config)
    int32 ClientPort = 8889;
};