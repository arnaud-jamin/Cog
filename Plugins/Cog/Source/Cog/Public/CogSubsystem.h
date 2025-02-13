#pragma once

#include "CoreMinimal.h"
#include "CogImguiContext.h"
#include "CogWindow_Settings.h"
#include "imgui.h"
#include "Engine/GameInstance.h"
#include "CogSubsystem.generated.h"

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

UCLASS()
class COG_API UCogSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    virtual void Deinitialize() override;

    void Activate();

    virtual void SortMainMenu();

    virtual void AddWindow(FCogWindow* Window, const FString& Name);

    template<class T>
    T* AddWindow(const FString& Name);

    virtual FCogWindow* FindWindowByID(ImGuiID ID);

    virtual void CloseAllWindows();

    virtual void ResetLayout();

    virtual void LoadLayout(int32 LayoutIndex);

    virtual void SaveLayout(int32 LayoutIndex);

    virtual void SetActivateSelectionMode(bool Value);

    virtual bool GetActivateSelectionMode() const;

    virtual void ResetAllWindowsConfig();

    const UCogWindowConfig_Settings* GetSettings() const { return Settings.Get(); }

    UCogCommonConfig* GetConfig(const TSubclassOf<UCogCommonConfig>& ConfigClass);

    template<class T>
    T* GetConfig();

    const UObject* GetAsset(const TSubclassOf<UObject>& AssetClass) const;

    template<typename T> 
    T* GetAsset();

    const FCogImguiContext& GetContext() const { return Context; }

    FCogImguiContext& GetContext() { return Context; }

    void OnShortcutsDefined();

    bool IsRenderingMainMenu() const { return IsRenderingInMainMenu; }

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

    virtual void Render(float DeltaTime);

    virtual void Tick(UWorld* InTickedWorld, ELevelTick InTickType, float InDeltaTime);
    
    virtual void TryInitialize(UWorld* World);

    virtual void InitializeWindow(FCogWindow* Window);

    virtual void Shutdown();

    virtual void RenderMainMenu();

    virtual FMenu* AddMenu(const FString& Name);

    virtual void RenderOptionMenu(FMenu& Menu);

    virtual void RenderMenuItem(FCogWindow& Window, const char* MenuItemName);

    virtual void RenderMenuItemHelp(FCogWindow& Window);

    virtual void ToggleInputMode();

    virtual void DisableInputMode();
    
    virtual void HandleInputs(const UPlayerInput& PlayerInput);

    virtual void RenderWidgets();

    virtual void SaveAllSettings();

    virtual void ReloadAllSettings();

    static void SettingsHandler_ClearAll(ImGuiContext* ctx, ImGuiSettingsHandler*);

    static void SettingsHandler_ApplyAll(ImGuiContext* ctx, ImGuiSettingsHandler*);

    static void* SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name);

    static void SettingsHandler_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line);

    static void SettingsHandler_WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);

    static FString EnableCommand;
    
    static FString ToggleInputCommand;

    static FString DisableInputCommand;
    
    static FString LoadLayoutCommand;
    
    static FString SaveLayoutCommand;

    static FString ResetLayoutCommand;

    UPROPERTY()
    TWeakObjectPtr<UWorld> CurrentWorld;
    
    UPROPERTY()
    mutable TArray<TObjectPtr<UCogCommonConfig>> Configs;

    UPROPERTY()
    mutable TArray<TObjectPtr<const UObject>> Assets;

    FCogImguiContext Context;

    TArray<FCogWindow*> Windows;

    TArray<FCogWindow*> Widgets;

    int32 WidgetsOrderIndex = 0;

    TArray<FCogWindow*> SpaceWindows;

    FCogWindow_Settings* SettingsWindow = nullptr;
    
    TWeakObjectPtr<UCogWindowConfig_Settings> Settings;

    FCogWindow_Layouts* LayoutsWindow = nullptr;

    FMenu MainMenu;

    int32 LayoutToLoad = -1;

    int32 SelectionModeActiveCounter = 0;
    
    bool bIsInputEnabledBeforeEnteringSelectionMode = false;

    bool bEnable = false;

    bool bIsSelectionModeActive = false;

    bool IsInitialized = false;
    
    bool IsRenderingInMainMenu = false;
    
    int32 NumExecBindingsChecked = 0;
    
};

//--------------------------------------------------------------------------------------------------------------------------
template<class T>
T* UCogSubsystem::AddWindow(const FString& Name)
{
    T* Window = new T();
    AddWindow(Window, Name);
    return Window;
}

//--------------------------------------------------------------------------------------------------------------------------
template<class T>
T* UCogSubsystem::GetConfig()
{
    static_assert(TPointerIsConvertibleFromTo<T, const UCogCommonConfig>::Value);
    return Cast<T>(GetConfig(T::StaticClass()));
}

//--------------------------------------------------------------------------------------------------------------------------
template<typename T>
T* UCogSubsystem::GetAsset()
{
    return Cast<T>(GetAsset(T::StaticClass()));
}