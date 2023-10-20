#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "imgui.h"
#include "CogWindowManager.generated.h"

class IConsoleObject;
class SCogImguiWidget;
class UCogWindow;
class UCogWindow_Settings;
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

    template<class T>
    T* CreateWindow(const FString& Name, bool AddToMainMenu = true)
    {
        T* Window = NewObject<T>(this);
        Window->SetFullName(Name);
        Window->Initialize();
        AddWindow(Window, AddToMainMenu);
        return Window;
    }

    virtual void AddWindow(UCogWindow* Window, bool AddToMainMenu = true);

    virtual void AddMainMenuWidget(UCogWindow* Window);

    virtual UCogWindow* FindWindowByID(ImGuiID ID);

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

protected:

    friend class UCogWindow_Settings;

    struct FMenu
    {
        FString Name;
        UCogWindow* Window = nullptr;
        TArray<FMenu> SubMenus;
    };

    virtual void InitializeInternal();

    virtual void RefreshDPIScale();

    virtual void RenderMainMenu();
    
    virtual FMenu* AddMenu(const FString& Name);

    virtual void RenderOptionMenu(FMenu& Menu);

    virtual void RenderMenuItem(UCogWindow& Window, const char* MenuItemName);

    static void SettingsHandler_ClearAll(ImGuiContext* ctx, ImGuiSettingsHandler*);

    static void SettingsHandler_ApplyAll(ImGuiContext* ctx, ImGuiSettingsHandler*);

    static void* SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name);

    static void SettingsHandler_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line);

    static void SettingsHandler_WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);
    
    void TickDPI();

    UPROPERTY()
    TArray<UCogWindow*> Windows;

    UPROPERTY()
    TArray<UCogWindow*> SpaceWindows;

    UPROPERTY()
    UCogWindow_Settings* SettingsWindow = nullptr;
    
    UPROPERTY()
    TArray<UCogWindow*> MainMenuWidgets;

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

    FMenu MainMenu;

    int32 LayoutToLoad = -1;

    int32 HideAllWindowsCounter = 0;

    bool bHideAllWindows = false;

    bool bRefreshDPIScale = false;

    TArray<IConsoleObject*> ConsoleCommands;
};
