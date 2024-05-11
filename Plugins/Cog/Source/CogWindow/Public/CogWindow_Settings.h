#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "CogWindow_Settings.generated.h"

class UCogEngineConfig_Settings;

//--------------------------------------------------------------------------------------------------------------------------
class COGWINDOW_API FCogWindow_Settings : public FCogWindow
{
    typedef FCogWindow Super;

public:
    
    virtual void Initialize() override;

    virtual void RenderTick(float DeltaTime) override;

    const UCogWindowConfig_Settings* GetSettingsConfig() const { return Config; }

    void SetDPIScale(float Value) const;

protected: 

    virtual void RenderContent() override;

    virtual void PreSaveConfig() override;

    virtual void ResetConfig() override;

    TObjectPtr<UCogWindowConfig_Settings> Config = nullptr;
};


//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogWindowConfig_Settings : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    float DPIScale = 1.0f;

    UPROPERTY(Config)
    bool bEnableViewports = false;

    UPROPERTY(Config)
    bool bCompactMode = false;

    UPROPERTY(Config)
    bool bTransparentMode = false;

    UPROPERTY(Config)
    bool bShowHelp = true;

    UPROPERTY(Config)
    bool bShowWindowsInMainMenu = true;

    UPROPERTY(Config)
    bool bEnableInput = false;

    UPROPERTY(Config)
    bool bShareMouse = false;

    UPROPERTY(Config)
    bool bShareMouseWithGameplay = false;

    UPROPERTY(Config)
    bool bShareKeyboard = false;

    UPROPERTY(Config)
    bool bNavEnableKeyboard = false;

    //UPROPERTY(Config)
    //bool bNavEnableGamepad = false;

    //UPROPERTY(Config)
    //bool bNavNoCaptureInput = true;

    virtual void Reset() override
    {
        Super::Reset();

        DPIScale = 1.0f;
        bEnableViewports = false;
        bCompactMode = false;
        bTransparentMode = false;
        bShowHelp = true;
        bShowWindowsInMainMenu = true;
        bEnableInput = false;
        bShareMouse = false;
        bShareKeyboard = false;
        bNavEnableKeyboard = false;
        //bNavEnableGamepad = false;
        //bNavNoCaptureInput = true;
    }
};