#include "CogWindowManager.h"

#include "CogDebugDrawImGui.h"
#include "CogImguiInputHelper.h"
#include "CogImguiModule.h"
#include "CogWindow_Inputs.h"
#include "CogWindow_Settings.h"
#include "CogWindow_Spacing.h"
#include "CogWindowConfig.h"
#include "CogWindowHelper.h"
#include "CogWindowWidgets.h"
#include "Engine/Engine.h"
#include "HAL/IConsoleManager.h"
#include "imgui_internal.h"

FString UCogWindowManager::ToggleInputCommand   = TEXT("Cog.ToggleInput");
FString UCogWindowManager::LoadLayoutCommand    = TEXT("Cog.LoadLayout");
FString UCogWindowManager::SaveLayoutCommand    = TEXT("Cog.SaveLayout");
FString UCogWindowManager::ResetLayoutCommand   = TEXT("Cog.ResetLayout");

//--------------------------------------------------------------------------------------------------------------------------
UCogWindowManager::UCogWindowManager()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::PostInitProperties()
{
    Super::PostInitProperties();

    if (bRegisterDefaultCommands)
    {
        if (RegisterDefaultCommands())
        {
            bRegisterDefaultCommands = false;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::InitializeInternal()
{
    ImGuiWidget = FCogImguiModule::Get().CreateImGuiWidget(GEngine->GameViewport, [this](float DeltaTime) { Render(DeltaTime); });
    ImGuiWidget->SetEnableInput(bEnableInput);
    ImGuiWidget->SetShareGamepad(bShareGamepad);
    ImGuiWidget->SetShareKeyboard(bShareKeyboard);
    ImGuiWidget->SetShareMouse(bShareMouse);

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

    SpaceWindows.Add(AddWindow<FCogWindow_Spacing>("Spacing 1", false));
    SpaceWindows.Add(AddWindow<FCogWindow_Spacing>("Spacing 2", false));
    SpaceWindows.Add(AddWindow<FCogWindow_Spacing>("Spacing 3", false));
    SpaceWindows.Add(AddWindow<FCogWindow_Spacing>("Spacing 4", false));

    InputsWindow = AddWindow<FCogWindow_Inputs>("Window.Inputs", false);

    SettingsWindow = AddWindow<FCogWindow_Settings>("Window.Settings", false);

    ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
        *ToggleInputCommand,
        TEXT("Toggle the input focus between the Game and ImGui"),
        FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args) { ToggleInputMode(); }), 
        ECVF_Cheat));

    ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
        *ResetLayoutCommand,
        TEXT("Reset the layout."),
        FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args) { if (Args.Num() > 0) { ResetLayout(); }}),
        ECVF_Cheat));

    ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
        *LoadLayoutCommand,
        TEXT("Load the layout. Cog.LoadLayout <Index>"),
        FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args) { if (Args.Num() > 0) { LoadLayout(FCString::Atoi(*Args[0])); }}), 
        ECVF_Cheat));

    ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
        *SaveLayoutCommand, 
        TEXT("Save the layout. Cog.SaveLayout <Index>"),
        FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args) { if (Args.Num() > 0) { SaveLayout(FCString::Atoi(*Args[0])); }}), 
        ECVF_Cheat));

}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::Shutdown()
{
    //------------------------------------------------------------
    // To save the input mode for the next session
    //------------------------------------------------------------
    bEnableInput = ImGuiWidget->GetEnableInput();
    bShareGamepad = ImGuiWidget->GetEnableInput();
    bShareKeyboard = ImGuiWidget->GetEnableInput();
    bShareMouse = ImGuiWidget->GetEnableInput();

    //------------------------------------------------------------
    // Destroy ImGui before destroying the windows to make sure 
    // imgui serialize their visibility state in imgui.ini
    //------------------------------------------------------------
    ImGuiWidget->DestroyImGuiContext();

    SaveConfig();

    for (FCogWindow* Window : Windows)
    {
        Window->PreSaveConfig();
        Window->Shutdown();
        delete Window;
    }
    Windows.Empty();

    for (UCogWindowConfig* Config : Configs)
    {
        Config->SaveConfig();
    }

    for (IConsoleObject* ConsoleCommand : ConsoleCommands)
    {
        IConsoleManager::Get().UnregisterConsoleObject(ConsoleCommand);
    }

    FCogImguiModule::Get().DestroyImGuiWidget(ImGuiWidget);
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

    for (FCogWindow* Window : Windows)
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
        if (ImGuiWidget->GetEnableInput() || bHideMainMenuOnGameInput == false)
        {
            RenderMainMenu();
        }
    }

    for (FCogWindow* Window : Windows)
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
void UCogWindowManager::AddWindow(FCogWindow* Window, const FString& Name, bool AddToMainMenu /*= true*/)
{
    Window->SetFullName(Name);
    Window->SetOwner(this);
    Window->Initialize();
    Windows.Add(Window);

    if (Window->HasWidget())
    {
        Widgets.Add(Window);
        //Widgets.Sort()
    }

    if (AddToMainMenu)
    {
        if (FMenu* Menu = AddMenu(Window->GetFullName()))
        {
            Menu->Window = Window;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FCogWindow* UCogWindowManager::FindWindowByID(ImGuiID ID)
{
    for (FCogWindow* Window : Windows)
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
    for (FCogWindow* Window : Windows)
    {
        ImGui::SetWindowPos(TCHAR_TO_ANSI(*Window->GetName()), ImVec2(10, 10), ImGuiCond_Always);
    }

    ImGui::ClearIniSettings();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::CloseAllWindows()
{
    for (FCogWindow* Window : Windows)
    {
        Window->SetIsVisible(false);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::LoadLayout(int32 LayoutIndex)
{
    for (FCogWindow* Window : Windows)
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

    TArray<FCogWindow*> SortedWindows = Windows;
    SortedWindows.Sort([](FCogWindow& Lhs, FCogWindow& Rhs) { return Lhs.GetFullName() < Rhs.GetFullName(); });

    for (FCogWindow* Window : SortedWindows)
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
    const UPlayerInput* PlayerInput = FCogImguiInputHelper::GetPlayerInput(*GetWorld());

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
                    RenderLoadLayoutMenuItem(PlayerInput, i);
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Save Window Layout"))
            {
                for (int32 i = 1; i <= 4; ++i)
                {
                    RenderSaveLayoutMenuItem(PlayerInput, i);
                }

                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("Spacing"))
            {
                for (FCogWindow* SpaceWindow : SpaceWindows)
                {
                    bool bSpaceVisible = SpaceWindow->GetIsVisible();
                    if (ImGui::MenuItem(TCHAR_TO_ANSI(*SpaceWindow->GetName()), nullptr, &bSpaceVisible))
                    {
                        SpaceWindow->SetIsVisible(bSpaceVisible);
                    }
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Widgets"))
            {
                for (int32 i = 0; i < Widgets.Num(); ++i)
                {
                    FCogWindow* Window = Widgets[i];

                    ImGui::PushID(i);

                    bool Visible = Window->GetIsWidgetVisible();
                    if (ImGui::Checkbox(TCHAR_TO_ANSI(*Window->GetName()), &Visible))
                    {
                        Window->SetIsWidgetVisible(Visible);
                    }

                    if (ImGui::IsItemActive() && ImGui::IsItemHovered() == false)
                    {
                        int iNext = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                        if (iNext >= 0 && iNext < Widgets.Num())
                        {
                            Widgets[i] = Widgets[iNext];
                            Widgets[iNext] = Window;
                            ImGui::ResetMouseDragDelta();
                        }
                    }

                    if (i == 0)
                    {
                        ImGui::SameLine();
                        FCogWindowWidgets::HelpMarker("Drag and drop the widget names to reorder them.");
                    }

                    ImGui::PopID();
                }


                ImGui::EndMenu();
            }

            RenderMenuItem(*InputsWindow, "Inputs");

            RenderMenuItem(*SettingsWindow, "Settings");
 
            
            ImGui::EndMenu();
        }

        const float MinCursorX = ImGui::GetCursorPosX();
        float CursorX = ImGui::GetWindowWidth();
        
        //------------------------------------------------------------
        // Render in reverse order because it makes more sense 
        // when looking at the widget ordered list in the UI.
        //------------------------------------------------------------
        for (int32 WindowIndex = Widgets.Num() - 1;  WindowIndex >= 0; WindowIndex--)
        {
            FCogWindow* Window = Widgets[WindowIndex];

            if (Window->GetIsWidgetVisible() == false)
            {
                continue;
            }

            TArray<float> SubWidgetsWidths;
            float SimCursorX = CursorX;
            for (int32 SubWidgetIndex = 0; ; ++SubWidgetIndex)
            {
                const float MaxWidth = SimCursorX - MinCursorX;
                float SubWidgetWidth = Window->GetMainMenuWidgetWidth(SubWidgetIndex, MaxWidth);
                if (SubWidgetWidth == -1)
                {
                    break;
                }

                SimCursorX -= SubWidgetWidth;
                SubWidgetsWidths.Add(SubWidgetWidth);
            }

            bool Stop = false;
            for (int32 SubWidgetIndex = SubWidgetsWidths.Num() - 1; SubWidgetIndex >= 0; SubWidgetIndex--)
            {
                const float SubWidgetWidth = SubWidgetsWidths[SubWidgetIndex];
                const float MaxWidth = CursorX - MinCursorX;

                //-------------------------------------------
                // Bypass this subwidget if its width is 0
                //-------------------------------------------
                if (SubWidgetWidth == 0)
                {
                    continue;
                }

                //-------------------------------------------
                // Stop drawing if there is not enough room
                //-------------------------------------------
                if (SubWidgetWidth > MaxWidth)
                {
                    Stop = true;
                    break;
                }

                CursorX -= SubWidgetWidth;
                ImGui::SetCursorPosX(CursorX);
                
                Window->RenderMainMenuWidget(SubWidgetIndex, SubWidgetWidth);
            }

            if (Stop)
            {
                break;
            }

            CursorX -= ImGui::GetStyle().ItemSpacing.x;
        }

        ImGui::EndMainMenuBar();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::RenderLoadLayoutMenuItem(const UPlayerInput* PlayerInput, int LayoutIndex)
{
    FString Command = FString::Printf(TEXT("%s %d"), *LoadLayoutCommand, LayoutIndex);
    FString Shortcut = FCogImguiInputHelper::CommandToString(PlayerInput, Command);
    if (ImGui::MenuItem(TCHAR_TO_ANSI(*FString::Printf(TEXT("Load Layout %d"), LayoutIndex)), TCHAR_TO_ANSI(*Shortcut)))
    {
        LoadLayout(LayoutIndex);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::RenderSaveLayoutMenuItem(const UPlayerInput* PlayerInput, int LayoutIndex)
{
    FString Command = FString::Printf(TEXT("%s %d"), *SaveLayoutCommand, LayoutIndex);
    FString Shortcut = FCogImguiInputHelper::CommandToString(PlayerInput, Command);
    if (ImGui::MenuItem(TCHAR_TO_ANSI(*FString::Printf(TEXT("Save Layout %d"), LayoutIndex)), TCHAR_TO_ANSI(*Shortcut)))
    {
        SaveLayout(LayoutIndex);
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
void UCogWindowManager::RenderMenuItem(FCogWindow& Window, const char* MenuItemName)
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
    UCogWindowManager* Manager = (UCogWindowManager*)Handler->UserData;

    Manager->Widgets.Sort([](const FCogWindow& Window1, const FCogWindow& Window2)
    {
        return Window1.GetWidgetOrderIndex() < Window2.GetWidgetOrderIndex();
    });
}

//--------------------------------------------------------------------------------------------------------------------------
void* UCogWindowManager::SettingsHandler_ReadOpen(ImGuiContext* Context, ImGuiSettingsHandler* Handler, const char* Name)
{
    if (strcmp(Name, "Windows") == 0)
    {
        return (void*)1;
    }

    if (strcmp(Name, "Widgets") == 0)
    {
        UCogWindowManager* Manager = (UCogWindowManager*)Handler->UserData;
        Manager->WidgetsOrderIndex = 0;

        return (void*)2;
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::SettingsHandler_ReadLine(ImGuiContext* Context, ImGuiSettingsHandler* Handler, void* Entry, const char* Line)
{
    if (Entry == (void*)1)
    {
        ImGuiID Id;
        if (sscanf_s(Line, "0x%08X", &Id) == 1)
        {
            UCogWindowManager* Manager = (UCogWindowManager*)Handler->UserData;
            if (FCogWindow* Window = Manager->FindWindowByID(Id))
            {
                Window->SetIsVisible(true);
            }
        }
    }
    else if (Entry == (void*)2)
    {
        ImGuiID Id;
        int32 Visible = false;
        if (sscanf_s(Line, "0x%08X %d", &Id, &Visible) == 2)
        {
            UCogWindowManager* Manager = (UCogWindowManager*)Handler->UserData;
            if (FCogWindow* Window = Manager->FindWindowByID(Id))
            {
                Window->SetWidgetOrderIndex(Manager->WidgetsOrderIndex);
                Window->SetIsWidgetVisible(Visible > 0);
            }

            Manager->WidgetsOrderIndex++;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::SettingsHandler_WriteAll(ImGuiContext* Context, ImGuiSettingsHandler* Handler, ImGuiTextBuffer* Buffer)
{
    const UCogWindowManager* Manager = (UCogWindowManager*)Handler->UserData;

    Buffer->appendf("[%s][Windows]\n", Handler->TypeName);
    for (FCogWindow* Window : Manager->Windows)
    {
        if (Window->GetIsVisible())
        {
            Buffer->appendf("0x%08X\n", Window->GetID());
        }
    }
    Buffer->append("\n");

    Buffer->appendf("[%s][Widgets]\n", Handler->TypeName);
    for (FCogWindow* Window : Manager->Widgets)
    {
        Buffer->appendf("0x%08X %d\n", Window->GetID(), Window->GetIsWidgetVisible());
    }
    Buffer->append("\n");
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
    for (FCogWindow* Window : Windows)
    {
        Window->ResetConfig();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogWindowManager::RegisterDefaultCommands()
{
    if (GetWorld() == nullptr)
    {
        return false;
    }

    UPlayerInput* PlayerInput = FCogImguiInputHelper::GetPlayerInput(*GetWorld());

    AddCommand(PlayerInput, "Cog.ToggleInput", EKeys::Tab);
    AddCommand(PlayerInput, "Cog.LoadLayout 1", EKeys::F1);
    AddCommand(PlayerInput, "Cog.LoadLayout 2", EKeys::F2);
    AddCommand(PlayerInput, "Cog.LoadLayout 3", EKeys::F3);
    AddCommand(PlayerInput, "Cog.LoadLayout 4", EKeys::F4);
    AddCommand(PlayerInput, "Cog.ToggleSelectionMode", EKeys::F5);

    SortCommands(PlayerInput);
    PlayerInput->SaveConfig();

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::AddCommand(UPlayerInput* PlayerInput, const FString& Command, const FKey& Key)
{
    //---------------------------------------------------
    // Reassign conflicting commands
    //---------------------------------------------------
    for (FKeyBind& KeyBind : PlayerInput->DebugExecBindings)
    {
        if (KeyBind.Key == Key && KeyBind.Command != Command)
        {
            KeyBind.Control = true;
            KeyBind.bIgnoreCtrl = false;
        }
    }

    //---------------------------------------------------
    // Find or add desired command
    //---------------------------------------------------
    FKeyBind* ExistingKeyBind = PlayerInput->DebugExecBindings.FindByPredicate([Command](const FKeyBind& KeyBind) { return KeyBind.Command == Command; });
    if (ExistingKeyBind == nullptr)
    {
        ExistingKeyBind = &PlayerInput->DebugExecBindings.AddDefaulted_GetRef();
    }

    //---------------------------------------------------
    // Assign the key to the command
    //---------------------------------------------------
    FKeyBind CogKeyBind;
    CogKeyBind.Command = Command;
    CogKeyBind.Control = false;
    CogKeyBind.bIgnoreCtrl = true;
    CogKeyBind.Key = Key;

    *ExistingKeyBind = CogKeyBind;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::SortCommands(UPlayerInput* PlayerInput)
{
    PlayerInput->DebugExecBindings.Sort([](const FKeyBind& Key1, const FKeyBind& Key2)
    {
        return Key1.Command.Compare(Key2.Command) < 0;
    });
}

//--------------------------------------------------------------------------------------------------------------------------
UCogWindowConfig* UCogWindowManager::GetConfig(const TSubclassOf<UCogWindowConfig> ConfigClass)
{
    const UClass* Class = ConfigClass.Get();

    for (UCogWindowConfig* Config : Configs)
    {
        if (Config && Config->IsA(Class))
        {
            return Cast<UCogWindowConfig>(Config);
        }
    }

    UCogWindowConfig* Config = NewObject<UCogWindowConfig>(this, Class);
    Configs.Add(Config);
    return Config;
}

//--------------------------------------------------------------------------------------------------------------------------
const UObject* UCogWindowManager::GetAsset(const TSubclassOf<UObject> AssetClass) 
{
    const UClass* Class = AssetClass.Get();

    for (const UObject* Asset : Assets)
    {
        if (Asset && Asset->IsA(Class))
        {
            return Asset;
        }
    }

    const UObject* Asset = FCogWindowHelper::GetFirstAssetByClass(AssetClass);
    if (Asset == nullptr)
    {
        return nullptr;
    }

    Assets.Add(Asset);

    return Asset;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogWindowManager::ToggleInputMode()
{
    ImGuiWidget->SetEnableInput(!ImGuiWidget->GetEnableInput());
}