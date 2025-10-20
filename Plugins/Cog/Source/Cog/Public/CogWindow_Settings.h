#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogInputChord.h"
#include "CogWindow.h"
#include "Framework/Commands/InputChord.h"
#include "CogWindow_Settings.generated.h"

class UCogEngineConfig_Settings;

//--------------------------------------------------------------------------------------------------------------------------
class COG_API FCogWindow_Settings : public FCogWindow
{
    typedef FCogWindow Super;

public:

    FCogWindow_Settings();
    
    virtual void Initialize() override;

    virtual void RenderTick(float DeltaTime) override;

    const UCogWindowConfig_Settings* GetSettingsConfig() const { return Config; }

    void SetDPIScale(float Value) const;

protected: 

    virtual void RenderContent() override;

    virtual void PreSaveConfig() override;

    TObjectPtr<UCogWindowConfig_Settings> Config = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogWidgetAlignment
{
    Left = 0,
    Center = 1,
    Right = 2,
    Manual = 3
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

    UPROPERTY(Config)
    bool bDisableConflictingCommands = true;

    UPROPERTY(Config)
    bool bDisableShortcutsWhenImGuiWantTextInput = false;

    UPROPERTY(Config)
    ECogWidgetAlignment WidgetAlignment = ECogWidgetAlignment::Right;

    UPROPERTY(Config)
    bool ShowWidgetBorders = false;

    UPROPERTY(Config)
    FString Font;

    UPROPERTY(Config)
    FCogInputChord Shortcut_ToggleImguiInput = FCogInputChord(EKeys::F1, true);

    UPROPERTY(Config)
    FCogInputChord Shortcut_LoadLayout1 = FCogInputChord(EKeys::F2, true);

    UPROPERTY(Config)
    FCogInputChord Shortcut_LoadLayout2 = FCogInputChord(EKeys::F3, true);

    UPROPERTY(Config)
    FCogInputChord Shortcut_LoadLayout3 = FCogInputChord(EKeys::F4, true);

    UPROPERTY(Config)
    FCogInputChord Shortcut_LoadLayout4 = FCogInputChord();

    UPROPERTY(Config)
    FCogInputChord Shortcut_SaveLayout1 = FCogInputChord();

    UPROPERTY(Config)
    FCogInputChord Shortcut_SaveLayout2 = FCogInputChord();

    UPROPERTY(Config)
    FCogInputChord Shortcut_SaveLayout3 = FCogInputChord();

    UPROPERTY(Config)
    FCogInputChord Shortcut_SaveLayout4 = FCogInputChord();
    
    UPROPERTY(Config)
    FCogInputChord Shortcut_ResetLayout = FCogInputChord();

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
        bShareMouseWithGameplay = false;
        bShareKeyboard = false;
        bNavEnableKeyboard = false;
        bDisableConflictingCommands = true;
        bDisableShortcutsWhenImGuiWantTextInput = false;
        WidgetAlignment = ECogWidgetAlignment::Right;
        ShowWidgetBorders = false;
        Shortcut_ToggleImguiInput = FCogInputChord(EKeys::F1, true);
        Shortcut_LoadLayout1 = FCogInputChord(EKeys::F2, true);
        Shortcut_LoadLayout2 = FCogInputChord(EKeys::F3, true);
        Shortcut_LoadLayout3 = FCogInputChord(EKeys::F4, true);
        Shortcut_LoadLayout4 = FCogInputChord();
        Shortcut_SaveLayout1 = FCogInputChord();
        Shortcut_SaveLayout2 = FCogInputChord();
        Shortcut_SaveLayout3 = FCogInputChord();
        Shortcut_SaveLayout4 = FCogInputChord();
        Shortcut_ResetLayout = FCogInputChord();
        Font = FString();
    }
};