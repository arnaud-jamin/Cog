#pragma once

#include "CoreMinimal.h"
#include "imgui.h"
#include "CogWindowManager.generated.h"

class IConsoleObject;
class SCogImguiWidget;
class FCogWindow;
class FCogWindow_Settings;
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

    static FString ToggleInputCommand;
    static FString LoadLayoutCommand;
    static FString SaveLayoutCommand;
    static FString ResetLayoutCommand;

    UCogWindowManager();

    virtual void PostInitProperties() override;

    virtual void Shutdown();

    virtual void SortMainMenu();

    virtual void Render(float DeltaTime);

    virtual void Tick(float DeltaTime);

    virtual void AddWindow(FCogWindow* Window, const FString& Name, bool AddToMainMenu = true);

    virtual void AddMainMenuWidget(FCogWindow* Window);

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

    static void AddCommand(UPlayerInput* PlayerInput, const FString& Command, const FKey& Key);

    static void SortCommands(UPlayerInput* PlayerInput);

    UCogWindowConfig* GetConfig(const TSubclassOf<UCogWindowConfig> ConfigClass);

    const UObject* GetAsset(const TSubclassOf<UObject> AssetClass);

    template<class T>
    T* AddWindow(const FString& Name, bool AddToMainMenu = true)
    {
        T* Window = new T();
        AddWindow(Window, Name, AddToMainMenu);
        return Window;
    }

    template<class T>
    T* GetConfig()
    { 
        static_assert(TPointerIsConvertibleFromTo<T, const UCogWindowConfig>::Value);
        return Cast<T>(&GetConfig(T::StaticClass()));
    }

    template<typename T>
    static T* GetAsset()
    {
        return Cast<T>(GetAsset(T::StaticClass()));
    }

protected:

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

    virtual void RenderLoadLayoutMenuItem(const UPlayerInput* PlayerInput, int LayoutIndex);

    virtual void RenderSaveLayoutMenuItem(const UPlayerInput* PlayerInput, int LayoutIndex);

    static void SettingsHandler_ClearAll(ImGuiContext* ctx, ImGuiSettingsHandler*);

    static void SettingsHandler_ApplyAll(ImGuiContext* ctx, ImGuiSettingsHandler*);

    static void* SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name);

    static void SettingsHandler_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line);

    static void SettingsHandler_WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);
    
    void TickDPI();

    UPROPERTY()
    mutable TArray<UCogWindowConfig*> Configs;

    UPROPERTY()
    mutable TArray<const UObject*> Assets;

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

    TSharedPtr<SCogImguiWidget> ImGuiWidget = nullptr;

    TArray<FCogWindow*> Windows;

    TArray<FCogWindow*> SpaceWindows;

    FCogWindow_Settings* SettingsWindow = nullptr;

    TArray<FCogWindow*> MainMenuWidgets;

    FMenu MainMenu;

    int32 LayoutToLoad = -1;

    int32 HideAllWindowsCounter = 0;

    bool bHideAllWindows = false;

    bool bRefreshDPIScale = false;

    TArray<IConsoleObject*> ConsoleCommands;
};
