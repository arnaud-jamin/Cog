#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogDebug.h"
#include "CogWindow.h"
#include "CogEngineWindow_DebugSettings.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_DebugSettings : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void PreSaveConfig() override;

    virtual void RenderContent() override;

private:

    TWeakObjectPtr<UCogEngineConfig_DebugSettings> Config = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_DebugSettings : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    FCogDebugSettings Data;

    virtual void Reset() override
    {
        Super::Reset();
        Data = FCogDebugSettings();
    }
};