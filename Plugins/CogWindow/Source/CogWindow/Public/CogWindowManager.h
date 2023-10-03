#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "imgui.h"
#include "CogWindowManager.generated.h"

class UCogWindow;
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

    void Initialize(UWorld* World, TSharedPtr<SCogImguiWidget> InImGuiWidget);

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

    void LoadLayout(int LayoutIndex);

    void SaveLayout(int LayoutIndex);

    bool GetHideAllWindows() const { return bHideAllWindows; }

    void SetHideAllWindows(bool Value);

    bool GetCompactMode() const { return bCompactMode; }

private:

    struct FMenu
    {
        FString Name;
        UCogWindow* Window = nullptr;
        TArray<FMenu> SubMenus;
    };

    void RefreshDPIScale();

    void DrawMainMenu();
    
    FMenu* AddMenu(const FString& Name);

    void DrawMenu(FMenu& Menu);

    static void SettingsHandler_ClearAll(ImGuiContext* ctx, ImGuiSettingsHandler*);
    static void SettingsHandler_ApplyAll(ImGuiContext* ctx, ImGuiSettingsHandler*);
    static void* SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name);
    static void SettingsHandler_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line);
    static void SettingsHandler_WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);
    
    void TickDPI();

    UPROPERTY()
    TArray<UCogWindow*> Windows;

    UPROPERTY()
    TArray<UCogWindow*> MainMenuWidgets;

    UPROPERTY(Config)
    bool bCompactMode = false;

    UPROPERTY(Config)
    bool bTransparentMode = false;

    UPROPERTY(Config)
    float DPIScale = 1.0f;

    TSharedPtr<SCogImguiWidget> ImGuiWidget;

    TWeakObjectPtr<UWorld> World;
    FMenu MainMenu;
    int32 LayoutToLoad = -1;
    int32 HideAllWindowsCounter = 0;
    bool bHideAllWindows = false;
    bool bRefreshDPIScale;
};
