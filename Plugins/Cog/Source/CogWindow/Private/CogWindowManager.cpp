#include "CogWindowManager.h"

#include "CogDebugDrawImGui.h"
#include "CogImguiModule.h"
#include "CogImguiWidget.h"
#include "CogWindow_Settings.h"
#include "CogWindow_Spacing.h"
#include "CogWindowWidgets.h"
#include "Engine/Engine.h"
#include "HAL/IConsoleManager.h"
#include "imgui_internal.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogWindowManager::UCogWindowManager()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::InitializeInternal()
{
    ImGuiWidget = FCogImguiModule::Get().CreateImGuiViewport(GEngine->GameViewport, [this](float DeltaTime) { Render(DeltaTime); });

    ImGuiSettingsHandler IniHandler;
    IniHandler.TypeName = "Cog";
    IniHandler.TypeHash = ImHashStr("Cog");
    IniHandler.ClearAllFn = UCogWindowManager::SettingsHandler_ClearAll;
    IniHandler.ReadOpenFn = UCogWindowManager::SettingsHandler_ReadOpen;
    IniHandler.ReadLineFn = UCogWindowManager::SettingsHandler_ReadLine;
    IniHandler.ApplyAllFn = UCogWindowManager::SettingsHandler_ApplyAll;
    IniHandler.WriteAllFn = UCogWindowManager::SettingsHandler_WriteAll;
    IniHandler.UserData = this;
    ImGui::AddSettingsHandler(&IniHandler);

    SpaceWindows.Add(CreateWindow<UCogWindow_Spacing>("Spacing 1", false));
    SpaceWindows.Add(CreateWindow<UCogWindow_Spacing>("Spacing 2", false));
    SpaceWindows.Add(CreateWindow<UCogWindow_Spacing>("Spacing 3", false));
    SpaceWindows.Add(CreateWindow<UCogWindow_Spacing>("Spacing 4", false));

    SettingsWindow = CreateWindow<UCogWindow_Settings>("Window.Settings", false);

    ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("Cog.ToggleInput"), 
        TEXT("Toggle the input focus between the Game and ImGui"),
        FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args) { FCogImguiModule::Get().ToggleEnableInput(); }), 
        ECVF_Cheat));

    ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("Cog.LoadLayout"), 
        TEXT("Load the layout. Cog.LoadLayout <Index>"),
        FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args) { if (Args.Num() > 0) { LoadLayout(FCString::Atoi(*Args[0])); }}), 
        ECVF_Cheat));

    ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("Cog.SaveLayout"), 
        TEXT("Save the layout. Cog.SaveLayout <Index>"),
        FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args) { if (Args.Num() > 0) { SaveLayout(FCString::Atoi(*Args[0])); }}), 
        ECVF_Cheat));

}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::Shutdown()
{
    for (UCogWindow* Window : Windows)
    {
        Window->PreSaveConfig();
        Window->SaveConfig();
    }

    for (IConsoleObject* ConsoleCommand : ConsoleCommands)
    {
        IConsoleManager::Get().UnregisterConsoleObject(ConsoleCommand);
    }

    SaveConfig();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::Tick(float DeltaTime)
{
    if (GEngine->GameViewport == nullptr)
    {
        return;
    }

    if (ImGuiWidget.IsValid() == false)
    {
        InitializeInternal();
    }
    
    if (LayoutToLoad != -1)
    {
        FString Filename = FCogImguiHelper::GetIniFilePath(FString::Printf(TEXT("ImGui_Layout_%d"), LayoutToLoad));
        ImGui::LoadIniSettingsFromDisk(TCHAR_TO_ANSI(*Filename));
        LayoutToLoad = -1;
    }

    if (bRefreshDPIScale)
    {
        RefreshDPIScale();
        bRefreshDPIScale = false;
    }

    for (UCogWindow* Window : Windows)
    {
        Window->GameTick(DeltaTime);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::Render(float DeltaTime)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::DockSpaceOverViewport(0, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_AutoHideTabBar);
    ImGui::PopStyleColor(1);

    bool bCompactSaved = bCompactMode;
    if (bCompactSaved)
    {
        FCogWindowWidgets::PushStyleCompact();
    }

    if (bHideAllWindows == false)
    {
        if (FCogImguiModule::Get().GetEnableInput())
        {
            RenderMainMenu();
        }
    }

    for (UCogWindow* Window : Windows)
    {
        Window->RenderTick(DeltaTime);

        if (Window->GetIsVisible() && bHideAllWindows == false)
        {
            if (bTransparentMode)
            {
                ImGui::SetNextWindowBgAlpha(0.35f);
            }

            ImGui::DockSpaceOverViewport(0, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_AutoHideTabBar);
            Window->Render(DeltaTime);
        }
    }

    if (bCompactSaved)
    {
        FCogWindowWidgets::PopStyleCompact();
    }

    TickDPI();

    FCogDebugDrawImGui::Draw();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::AddWindow(UCogWindow* Window, bool AddToMainMenu /*= true*/)
{
    Window->SetOwner(this);
    Windows.Add(Window);

    if (AddToMainMenu)
    {
        if (FMenu* Menu = AddMenu(Window->GetFullName()))
        {
            Menu->Window = Window;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::AddMainMenuWidget(UCogWindow* Window)
{
    Window->SetOwner(this);
    MainMenuWidgets.Add(Window);
}

//--------------------------------------------------------------------------------------------------------------------------
UCogWindow* UCogWindowManager::FindWindowByID(ImGuiID ID)
{
    for (UCogWindow* Window : Windows)
    {
        if (Window->GetID() == ID)
        {
            return Window;
        }
    }
    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::SetHideAllWindows(bool Value)
{
    HideAllWindowsCounter = FMath::Max(HideAllWindowsCounter + (Value ? +1 : -1), 0);
    bHideAllWindows = HideAllWindowsCounter > 0;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::ResetLayout()
{
    for (UCogWindow* Window : Windows)
    {
        ImGui::SetWindowPos(TCHAR_TO_ANSI(*Window->GetName()), ImVec2(10, 10), ImGuiCond_Always);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::CloseAllWindows()
{
    for (UCogWindow* Window : Windows)
    {
        Window->SetIsVisible(false);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::LoadLayout(int32 LayoutIndex)
{
    for (UCogWindow* Window : Windows)
    {
        Window->SetIsVisible(false);
    }

    LayoutToLoad = LayoutIndex;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::SaveLayout(int32 LayoutIndex)
{
    FString Filename = *FCogImguiHelper::GetIniFilePath(FString::Printf(TEXT("imgui_layout_%d"), LayoutIndex));
    ImGui::SaveIniSettingsToDisk(TCHAR_TO_ANSI(*Filename));
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::SortMainMenu()
{
    MainMenu.SubMenus.Empty();

    Windows.Sort();

    TArray<UCogWindow*> SortedWindows = Windows;
    SortedWindows.Sort([](UCogWindow& Lhs, UCogWindow& Rhs) { return Lhs.GetFullName() < Rhs.GetFullName(); });

    for (UCogWindow* Window : SortedWindows)
    {
        if (FMenu* Menu = AddMenu(Window->GetFullName()))
        {
            Menu->Window = Window;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
UCogWindowManager::FMenu* UCogWindowManager::AddMenu(const FString& Name)
{
    TArray<FString> Path;
    Name.ParseIntoArray(Path, TEXT("."));

    UCogWindowManager::FMenu* CurrentMenu = &MainMenu;
    for (int i = 0; i < Path.Num(); ++i)
    {
        FString MenuName = Path[i];

        int SubMenuIndex = CurrentMenu->SubMenus.IndexOfByPredicate([&](const UCogWindowManager::FMenu& Menu) { return Menu.Name == MenuName; });
        if (SubMenuIndex != -1)
        {
            CurrentMenu = &CurrentMenu->SubMenus[SubMenuIndex];
        }
        else
        {
            CurrentMenu = &CurrentMenu->SubMenus.AddDefaulted_GetRef();
            CurrentMenu->Name = MenuName;
        }
    }

    return CurrentMenu;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::RenderMainMenu()
{
    if (ImGui::BeginMainMenuBar())
    {
        for (UCogWindowManager::FMenu& Menu : MainMenu.SubMenus)
        {
            RenderOptionMenu(Menu);
        }

        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Close All Windows"))
            {
                CloseAllWindows();
            }

            if (ImGui::MenuItem("Reset Window Layout"))
            {
                ResetLayout();
            }

            if (ImGui::BeginMenu("Load Window Layout"))
            {
                for (int32 i = 1; i <= 4; ++i)
                {
                    if (ImGui::MenuItem(TCHAR_TO_ANSI(*FString::Printf(TEXT("Load Layout %d"), i))))
                    {
                        LoadLayout(i);
                    }
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Save Window Layout"))
            {
                for (int32 i = 1; i <= 4; ++i)
                {
                    if (ImGui::MenuItem(TCHAR_TO_ANSI(*FString::Printf(TEXT("Save Layout %d"), i))))
                    {
                        SaveLayout(i);
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("Spacing"))
            {
                for (UCogWindow* SpaceWindow : SpaceWindows)
                {
                    bool bSpaceVisible = SpaceWindow->GetIsVisible();
                    if (ImGui::MenuItem(TCHAR_TO_ANSI(*SpaceWindow->GetName()), nullptr, &bSpaceVisible))
                    {
                        SpaceWindow->SetIsVisible(bSpaceVisible);
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();

            RenderMenuItem(*SettingsWindow, "Settings");
 
            
            ImGui::EndMenu();
        }

        const float MinCursorX = ImGui::GetCursorPosX();
        float CursorX = ImGui::GetWindowWidth();

        for (UCogWindow* Window : MainMenuWidgets)
        {
            float Width = 0.0f;
            Window->RenderMainMenuWidget(false, Width);

            //-------------------------------------------
            // Stop drawing if there is not enough room
            //-------------------------------------------
            if (CursorX - Width < MinCursorX)
            {
                break;
            }

            CursorX -= Width;
            ImGui::SetCursorPosX(CursorX);
            Window->RenderMainMenuWidget(true, Width);

            CursorX -= ImGui::GetStyle().ItemSpacing.x;
        }

        ImGui::EndMainMenuBar();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::RenderOptionMenu(UCogWindowManager::FMenu& Menu)
{
    if (Menu.Window != nullptr)
    {
        RenderMenuItem(*Menu.Window, TCHAR_TO_ANSI(*Menu.Name));
    }
    else
    {
        if (ImGui::BeginMenu(TCHAR_TO_ANSI(*Menu.Name)))
        {
            for (UCogWindowManager::FMenu& SubMenu : Menu.SubMenus)
            {
                RenderOptionMenu(SubMenu);
            }
            ImGui::EndMenu();
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::RenderMenuItem(UCogWindow& Window, const char* MenuItemName)
{
    if (bShowWindowsInMainMenu)
    {
        ImGui::SetNextWindowSizeConstraints(
            ImVec2(FCogWindowWidgets::GetFontWidth() * 40, ImGui::GetTextLineHeightWithSpacing() * 5),
            ImVec2(FCogWindowWidgets::GetFontWidth() * 50, ImGui::GetTextLineHeightWithSpacing() * 60));

        if (ImGui::BeginMenu(MenuItemName))
        {
            Window.RenderContent();
            ImGui::EndMenu();
        }

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        {
            Window.SetIsVisible(!Window.GetIsVisible());
        }
    }
    else
    {
        bool bIsVisible = Window.GetIsVisible();
        if (ImGui::MenuItem(MenuItemName, nullptr, &bIsVisible))
        {
            Window.SetIsVisible(bIsVisible);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::TickDPI()
{
    const float MouseWheel = ImGui::GetIO().MouseWheel;
    const bool IsControlDown = ImGui::GetIO().KeyCtrl;
    if (IsControlDown && MouseWheel != 0)
    {
        SetDPIScale(FMath::Clamp(DPIScale + (MouseWheel > 0 ? 0.1f : -0.1f), 0.5f, 2.0f));
    }

    const bool IsMiddleMouseClicked = ImGui::GetIO().MouseClicked[2];
    if (IsControlDown && IsMiddleMouseClicked)
    {
        SetDPIScale(1.0f);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::RefreshDPIScale()
{
    if (SCogImguiWidget* Widget = ImGuiWidget.Get())
    {
        Widget->SetDPIScale(DPIScale);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::SettingsHandler_ClearAll(ImGuiContext* Context, ImGuiSettingsHandler* Handler)
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::SettingsHandler_ApplyAll(ImGuiContext* Context, ImGuiSettingsHandler* Handler)
{
}

//--------------------------------------------------------------------------------------------------------------------------
void* UCogWindowManager::SettingsHandler_ReadOpen(ImGuiContext* Context, ImGuiSettingsHandler* Handler, const char* Name)
{
    return (void*)1;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::SettingsHandler_ReadLine(ImGuiContext* Context, ImGuiSettingsHandler* Handler, void* Entry, const char* Line)
{
    ImGuiID Id;
    if (sscanf_s(Line, "0x%08X", &Id) == 1)
    {
        UCogWindowManager* Manager = (UCogWindowManager*)Handler->UserData;
        if (UCogWindow* Window = Manager->FindWindowByID(Id))
        {
            Window->SetIsVisible(true);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::SettingsHandler_WriteAll(ImGuiContext* Context, ImGuiSettingsHandler* Handler, ImGuiTextBuffer* Buffer)
{
    const UCogWindowManager* Manager = (UCogWindowManager*)Handler->UserData;

    Buffer->appendf("[%s][Windows]\n", Handler->TypeName);
    for (UCogWindow* Window : Manager->Windows)
    {
        if (Window->GetIsVisible())
        {
            Buffer->appendf("0x%08X\n", Window->GetID());
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::SetDPIScale(float Value) 
{
    if (DPIScale == Value)
    {
        return;
    }

    DPIScale = Value; 
    bRefreshDPIScale = true; 
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::ResetAllWindowsConfig()
{
    for (UCogWindow* Window : Windows)
    {
        Window->ResetConfig();
    }
}