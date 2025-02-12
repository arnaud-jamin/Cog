#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogImguiKeyInfo.h"
#include "CogWindow.h"
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

    virtual void RenderShortcut(const char* Label, FCogImGuiKeyInfo& KeyInfo);

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
    FCogImGuiKeyInfo ToggleImGuiInputShortcut =  FCogImGuiKeyInfo(EKeys::F1);

    UPROPERTY(Config)
    FCogImGuiKeyInfo ToggleSelectionShortcut = FCogImGuiKeyInfo(EKeys::F5);

    UPROPERTY(Config)
    TArray<FCogImGuiKeyInfo> LoadLayoutShortcuts = { FCogImGuiKeyInfo(EKeys::F2), FCogImGuiKeyInfo(EKeys::F3), FCogImGuiKeyInfo(EKeys::F4), FCogImGuiKeyInfo()};

    UPROPERTY(Config)
    TArray<FCogImGuiKeyInfo> SaveLayoutShortcuts = { FCogImGuiKeyInfo(), FCogImGuiKeyInfo(), FCogImGuiKeyInfo(), FCogImGuiKeyInfo()};
    
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
         bDisableConflictingCommands = true;
        bDisableShortcutsWhenImGuiWantTextInput = false;
        //bNavEnableGamepad = false;
        //bNavNoCaptureInput = true;


        ToggleImGuiInputShortcut =  FCogImGuiKeyInfo(EKeys::F1);
        ToggleSelectionShortcut = FCogImGuiKeyInfo(EKeys::F5);
        LoadLayoutShortcuts = { FCogImGuiKeyInfo(EKeys::F2), FCogImGuiKeyInfo(EKeys::F3), FCogImGuiKeyInfo(EKeys::F4), FCogImGuiKeyInfo()};
        SaveLayoutShortcuts = { FCogImGuiKeyInfo(), FCogImGuiKeyInfo(), FCogImGuiKeyInfo(), FCogImGuiKeyInfo()};
    }
};