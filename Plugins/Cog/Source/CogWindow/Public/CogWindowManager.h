#pragma once

#include "CoreMinimal.h"
#include "CogImguiContext.h"
#include "imgui.h"
#include "CogWindowManager.generated.h"

class UCogCommonConfig;
class FCogWindow;
class FCogWindow_Layouts;
class FCogWindow_Settings;
class IConsoleObject;
class SCogImguiWidget;
class UPlayerInput;
class UWorld;
struct ImGuiSettingsHandler;
struct ImGuiTextBuffer;
struct FKey;

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

    virtual bool GetHideAllWindows() const { return bIsSelectionModeActive; }

    virtual void SetActivateSelectionMode(bool Value);

    virtual void ResetAllWindowsConfig();

    virtual bool RegisterDefaultCommandBindings();
    
    const FCogWindow_Settings* GetSettingsWindow() const { return SettingsWindow; }

    UCogCommonConfig* GetConfig(const TSubclassOf<UCogCommonConfig> ConfigClass);

    template<class T>
    T* GetConfig();

    const UObject* GetAsset(const TSubclassOf<UObject> AssetClass) const;

    template<typename T> 
    T* GetAsset();

    const FCogImguiContext& GetContext() const { return Context; }

    FCogImguiContext& GetContext() { return Context; }

    static void AddCommand(UPlayerInput* PlayerInput, const FString& Command, const FKey& Key);

    static void SortCommands(UPlayerInput* PlayerInput);

protected:

    friend class FCogWindow_Layouts;
    friend class FCogWindow_Settings;

    struct FMenu
    {
        FString Name;
        FCogWindow* Window = nullptr;
        TArray<FMenu> SubMenus;
    };

    virtual void InitializeInternal();

    virtual void RenderMainMenu();
    
    virtual FMenu* AddMenu(const FString& Name);

    virtual void RenderOptionMenu(FMenu& Menu);

    virtual void RenderMenuItem(FCogWindow& Window, const char* MenuItemName);

    virtual void RenderMenuItemHelp(FCogWindow& Window);

    virtual void ToggleInputMode();

    virtual void DisableInputMode();

    static void SettingsHandler_ClearAll(ImGuiContext* ctx, ImGuiSettingsHandler*);

    static void SettingsHandler_ApplyAll(ImGuiContext* ctx, ImGuiSettingsHandler*);

    static void* SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name);

    static void SettingsHandler_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line);

    static void SettingsHandler_WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);

    static FString ToggleInputCommand;

    static FString DisableInputCommand;
    
    static FString LoadLayoutCommand;
    
    static FString SaveLayoutCommand;

    static FString ResetLayoutCommand;

    UPROPERTY()
    mutable TArray<UCogCommonConfig*> Configs;

    UPROPERTY()
    mutable TArray<const UObject*> Assets;

    UPROPERTY(Config)
    bool bRegisterDefaultCommands = true;

    FCogImguiContext Context;

    TArray<FCogWindow*> Windows;

    TArray<FCogWindow*> Widgets;

    int32 WidgetsOrderIndex = 0;

    TArray<FCogWindow*> SpaceWindows;

    FCogWindow_Settings* SettingsWindow = nullptr;

    FCogWindow_Layouts* LayoutsWindow = nullptr;

    FMenu MainMenu;

    int32 LayoutToLoad = -1;

    int32 SelectionModeActiveCounter = 0;

    bool bIsSelectionModeActive = false;

    bool IsInitialized = false;
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
    static_assert(TPointerIsConvertibleFromTo<T, const UCogCommonConfig>::Value);
    return Cast<T>(&GetConfig(T::StaticClass()));
}

//--------------------------------------------------------------------------------------------------------------------------
template<typename T>
T* UCogWindowManager::GetAsset()
{
    return Cast<T>(GetAsset(T::StaticClass()));
}