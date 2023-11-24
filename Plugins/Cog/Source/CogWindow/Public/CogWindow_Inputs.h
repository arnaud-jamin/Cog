#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogWindowConfig.h"
#include "CogWindow_Inputs.generated.h"

class UCogEngineConfig_Inputs;

class COGWINDOW_API FCogWindow_Inputs : public FCogWindow
{
    typedef FCogWindow Super;

public:
    
    virtual void Initialize() override;

    virtual void PreSaveConfig() override;

protected: 

    virtual void RenderContent() override;

    TObjectPtr<UCogEngineConfig_Inputs> Config = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_Inputs : public UCogWindowConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool bEnableInput = false;

    UPROPERTY(Config)
    bool bShareMouse = true;

    UPROPERTY(Config)
    bool bNavEnableKeyboard = true;

    UPROPERTY(Config)
    bool bNavEnableGamepad = true;

    UPROPERTY(Config)
    bool bNavNoCaptureInput = true;

    virtual void Reset() override
    {
        Super::Reset();

        bEnableInput = false;
        bShareMouse = true;
        bNavEnableKeyboard = true;
        bNavEnableGamepad = true;
        bNavNoCaptureInput = true;
    }
};