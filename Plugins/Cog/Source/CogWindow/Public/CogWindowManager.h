#pragma once

#include "CoreMinimal.h"
#include "CogImguiWidget.h"
#include "imgui.h"
#include "CogWindowManager.generated.h"

class FCogWindow;
class FCogWindow_Inputs;
class FCogWindow_Layouts;
class FCogWindow_Settings;
class IConsoleObject;
class SCogImguiWidget;
class UCogWindowConfig;
class UPlayerInput;
class UWorld;
struct ImGuiSettingsHandler;
struct ImGuiTextBuffer;

UCLASS(Config = Cog)
class COGWINDOW_API UCogWindowManager : public UObject
{
    GENERATED_BODY()

public:

    UCogWindowManager();

    virtual void PostInitProperties() override;

    virtual void Shutdown();

    virtual void SortMainMenu();

    virtual void Render(float DeltaTime);

    virtual void Tick(float DeltaTime);

    virtual void AddWindow(FCogWindow* Window, const FString& Name, bool AddToMainMenu = true);

    template<class T>
    T* AddWindow(const FString& Name, bool AddToMainMenu = true);

    virtual FCogWindow* FindWindowByID(ImGuiID ID);

    virtual void CloseAllWindows();

    virtual void ResetLayout();

    virtual void LoadLayout(int32 LayoutIndex);

    virtual void SaveLayout(int32 LayoutIndex);

    virtual bool GetHideAllWindows() const { return bHideAllWindows; }

    virtual void SetHideAllWindows(bool Value);

    virtual float GetDPIScale() const { return DPIScale; }

    virtual void SetDPIScale(float Value);
    
    virtual bool GetCompactMode() const { return bCompactMode; }

    virtual void SetCompactMode(bool Value) { bCompactMode = Value; }

    virtual bool GetShowHelp() const { return bShowHelp; }

    virtual void SetShowHelp(bool Value) { bShowHelp = Value; }

    virtual bool GetPreviewWindowsInMenu() const { return bShowWindowsInMainMenu; }

    virtual void SetPreviewWindowsInMenu(bool Value) { bShowWindowsInMainMenu = Value; }

    virtual void ResetAllWindowsConfig();

    virtual bool RegisterDefaultCommands();
    
    UCogWindowConfig* GetConfig(const TSubclassOf<UCogWindowConfig> ConfigClass);

    template<class T>
    T* GetConfig();

    const UObject* GetAsset(const TSubclassOf<UObject> AssetClass);

    template<typename T> 
    T* GetAsset();

    TSharedPtr<SCogImguiWidget> GetImGuiWidget() const { return ImGuiWidget; }

    static void AddCommand(UPlayerInput* PlayerInput, const FString& Command, const FKey& Key);

    static void SortCommands(UPlayerInput* PlayerInput);

protected:

    friend class FCogWindow_Inputs;
    friend class FCogWindow_Layouts;
    friend class FCogWindow_Settings;

    struct FMenu
    {
        FString Name;
        FCogWindow* Window = nullptr;
        TArray<FMenu> SubMenus;
    };

    virtual void InitializeInternal();

    virtual void RefreshDPIScale();

    virtual void RenderMainMenu();
    
    virtual FMenu* AddMenu(const FString& Name);

    virtual void RenderOptionMenu(FMenu& Menu);

    virtual void RenderMenuItem(FCogWindow& Window, const char* MenuItemName);

    virtual void TickDPI();

    virtual void ToggleInputMode();

    static void SettingsHandler_ClearAll(ImGuiContext* ctx, ImGuiSettingsHandler*);

    static void SettingsHandler_ApplyAll(ImGuiContext* ctx, ImGuiSettingsHandler*);

    static void* SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name);

    static void SettingsHandler_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line);

    static void SettingsHandler_WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);

    static FString ToggleInputCommand;
    
    static FString LoadLayoutCommand;
    
    static FString SaveLayoutCommand;

    static FString ResetLayoutCommand;

    UPROPERTY()
    mutable TArray<UCogWindowConfig*> Configs;

    UPROPERTY()
    mutable TArray<const UObject*> Assets;

    UPROPERTY(Config)
    bool bEnableInput = false;

    UPROPERTY(Config)
    bool bShareGamepad = true;

    UPROPERTY(Config)
    bool bShareKeyboard = false;

    UPROPERTY(Config)
    bool bShareMouse = false;

    UPROPERTY(Config)
    bool bCompactMode = false;

    UPROPERTY(Config)
    bool bTransparentMode = false;

    UPROPERTY(Config)
    float DPIScale = 1.0f;

    UPROPERTY(Config)
    bool bShowHelp = true;

    UPROPERTY(Config)
    bool bShowWindowsInMainMenu = true;

    UPROPERTY(Config)
    bool bRegisterDefaultCommands = true;

    UPROPERTY(Config)
    bool bAuthorizeInactiveInput = true;

    UPROPERTY(Config)
    bool bAuthorizeExclusiveInput = true;

    UPROPERTY(Config)
    bool bAuthorizeSharedInput = true;

    UPROPERTY(Config)
    bool bHideMainMenuOnGameInput = true;

    TSharedPtr<SCogImguiWidget> ImGuiWidget = nullptr;

    TArray<FCogWindow*> Windows;

    TArray<FCogWindow*> Widgets;

    int32 WidgetsOrderIndex = 0;

    TArray<FCogWindow*> SpaceWindows;

    FCogWindow_Inputs* InputsWindow = nullptr;

    FCogWindow_Settings* SettingsWindow = nullptr;

    FCogWindow_Layouts* LayoutsWindow = nullptr;

    FMenu MainMenu;

    int32 LayoutToLoad = -1;

    int32 HideAllWindowsCounter = 0;

    bool bHideAllWindows = false;

    bool bRefreshDPIScale = false;

    TArray<IConsoleObject*> ConsoleCommands;
};

//--------------------------------------------------------------------------------------------------------------------------
template<class T>
T* UCogWindowManager::AddWindow(const FString& Name, bool AddToMainMenu)
{
    T* Window = new T();
    AddWindow(Window, Name, AddToMainMenu);
    return Window;
}

//--------------------------------------------------------------------------------------------------------------------------
template<class T>
T* UCogWindowManager::GetConfig()
{
    static_assert(TPointerIsConvertibleFromTo<T, const UCogWindowConfig>::Value);
    return Cast<T>(&GetConfig(T::StaticClass()));
}

//--------------------------------------------------------------------------------------------------------------------------
template<typename T>
T* UCogWindowManager::GetAsset()
{
    return Cast<T>(GetAsset(T::StaticClass()));
}