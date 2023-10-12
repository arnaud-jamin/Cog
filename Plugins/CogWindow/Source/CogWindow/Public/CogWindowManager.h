#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "imgui.h"
#include "CogWindowManager.generated.h"

class UCogWindow;
class UCogWindow_Settings;
class UWorld;
class SCogImguiWidget;
struct ImGuiSettingsHandler;
struct ImGuiTextBuffer;

UCLASS(Config = Cog)
class COGWINDOW_API UCogWindowManager : public UObject
{
    GENERATED_BODY()

public:
    UCogWindowManager();

    void Shutdown();

    void SortMainMenu();

    void Render(float DeltaTime);

    void Tick(float DeltaTime);

    template<class T>
    T* CreateWindow(const FString& Name, bool AddToMainMenu = true)
    {
        T* Window = NewObject<T>(this);
        Window->SetFullName(Name);
        Window->Initialize();
        AddWindow(Window, AddToMainMenu);
        return Window;
    }

    void AddWindow(UCogWindow* Window, bool AddToMainMenu = true);

    void AddMainMenuWidget(UCogWindow* Window);

    UCogWindow* FindWindowByID(ImGuiID ID);

    void CloseAllWindows();

    void ResetLayout();

    void LoadLayout(int LayoutIndex);

    void SaveLayout(int LayoutIndex);

    bool GetHideAllWindows() const { return bHideAllWindows; }

    void SetHideAllWindows(bool Value);

    float GetDPIScale() const { return DPIScale; }

    void SetDPIScale(float Value);
    
    bool GetCompactMode() const { return bCompactMode; }

    void SetCompactMode(bool Value) { bCompactMode = Value; }

    bool GetShowHelp() const { return bShowHelp; }

    void SetShowHelp(bool Value) { bShowHelp = Value; }

    bool GetPreviewWindowsInMenu() const { return bShowWindowsInMainMenu; }

    void SetPreviewWindowsInMenu(bool Value) { bShowWindowsInMainMenu = Value; }

    void ResetAllWindowsConfig();

protected:

    friend class UCogWindow_Settings;

    struct FMenu
    {
        FString Name;
        UCogWindow* Window = nullptr;
        TArray<FMenu> SubMenus;
    };

    void InitializeInternal();

    void RefreshDPIScale();

    void DrawMainMenu();
    
    FMenu* AddMenu(const FString& Name);

    void DrawMenu(FMenu& Menu);

    void DrawMenuItem(UCogWindow& Window, const char* MenuItemName);

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

    TSharedPtr<SCogImguiWidget> ImGuiWidget = nullptr;

    FMenu MainMenu;

    int32 LayoutToLoad = -1;

    int32 HideAllWindowsCounter = 0;

    bool bHideAllWindows = false;

    bool bRefreshDPIScale = false;
};
