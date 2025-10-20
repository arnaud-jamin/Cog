#include "CogSubsystem.h"

#include "CogCommon.h"
#include "CogDebugDrawImGui.h"
#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "CogWindow_Layouts.h"
#include "CogWindow_Settings.h"
#include "CogWindow_Spacing.h"
#include "CogConsoleCommandManager.h"
#include "CogDebugPluginSubsystem.h"
#include "CogHelper.h"
#include "CogWidgets.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerInput.h"
#include "imgui_internal.h"
#include "Misc/CoreMisc.h"
#include "NetImgui_Api.h"
#include "GameFramework/PlayerController.h"

FString UCogSubsystem::ToggleInputCommand   = TEXT("Cog.ToggleInput");
FString UCogSubsystem::DisableInputCommand  = TEXT("Cog.DisableInput");
FString UCogSubsystem::LoadLayoutCommand    = TEXT("Cog.LoadLayout");
FString UCogSubsystem::SaveLayoutCommand    = TEXT("Cog.SaveLayout");
FString UCogSubsystem::ResetLayoutCommand   = TEXT("Cog.ResetLayout");

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    { return false; }

#if ENABLE_COG
    return true;
#else
    return false;
#endif
}

//--------------------------------------------------------------------------------------------------------------------------
TStatId UCogSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UCogSubsystemBase, STATGROUP_Tickables);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::PostInitialize()
{
    Super::PostInitialize();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::Deinitialize()
{
    Shutdown();

    Super::Deinitialize();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::TryInitialize(UWorld& World)
{
    if (bIsInitialized)
    { return; }

    FWorldContext* WorldContext = GEngine->GetWorldContextFromWorld(&World);
    if (WorldContext == nullptr)
    { return; }

    if (WorldContext->GameViewport == nullptr && IsRunningDedicatedServer() == false)
    { return; }

    UE_LOG(LogCogImGui, Verbose, TEXT("UCogSubsystem::TryInitialize | World:%s %p"), *World.GetName(), &World);
    
    Context.Initialize(WorldContext->GameViewport.Get());

    FCogImGuiContextScope ImGuiContextScope(Context);
    
    ImGuiSettingsHandler IniHandler;
    IniHandler.TypeName = "Cog";
    IniHandler.TypeHash = ImHashStr("Cog");
    IniHandler.ClearAllFn = SettingsHandler_ClearAll;
    IniHandler.ReadOpenFn = SettingsHandler_ReadOpen;
    IniHandler.ReadLineFn = SettingsHandler_ReadLine;
    IniHandler.ApplyAllFn = SettingsHandler_ApplyAll;
    IniHandler.WriteAllFn = SettingsHandler_WriteAll;
    IniHandler.UserData = this;
    ImGui::AddSettingsHandler(&IniHandler);
    
    SpaceWindows.Add(AddWindow<FCogWindow_Spacing>("Spacing 1"));
    SpaceWindows.Add(AddWindow<FCogWindow_Spacing>("Spacing 2"));
    SpaceWindows.Add(AddWindow<FCogWindow_Spacing>("Spacing 3"));
    SpaceWindows.Add(AddWindow<FCogWindow_Spacing>("Spacing 4"));
    
     Settings = GetConfig<UCogWindowConfig_Settings>();
    
    UCogWindowConfig_Settings* SettingsPtr = Settings.Get();
    AddShortcut(SettingsPtr, &UCogWindowConfig_Settings::Shortcut_ToggleImguiInput, FSimpleDelegate::CreateLambda([this] () { ToggleInputMode(); }));
    AddShortcut(SettingsPtr, &UCogWindowConfig_Settings::Shortcut_LoadLayout1,      FSimpleDelegate::CreateLambda([this] () { LoadLayout(1); }));
    AddShortcut(SettingsPtr, &UCogWindowConfig_Settings::Shortcut_LoadLayout2,      FSimpleDelegate::CreateLambda([this] () { LoadLayout(2); }));
    AddShortcut(SettingsPtr, &UCogWindowConfig_Settings::Shortcut_LoadLayout3,      FSimpleDelegate::CreateLambda([this] () { LoadLayout(3); }));
    AddShortcut(SettingsPtr, &UCogWindowConfig_Settings::Shortcut_LoadLayout4,      FSimpleDelegate::CreateLambda([this] () { LoadLayout(4); }));
    AddShortcut(SettingsPtr, &UCogWindowConfig_Settings::Shortcut_SaveLayout1,      FSimpleDelegate::CreateLambda([this] () { SaveLayout(1); }));
    AddShortcut(SettingsPtr, &UCogWindowConfig_Settings::Shortcut_SaveLayout2,      FSimpleDelegate::CreateLambda([this] () { SaveLayout(2); }));
    AddShortcut(SettingsPtr, &UCogWindowConfig_Settings::Shortcut_SaveLayout3,      FSimpleDelegate::CreateLambda([this] () { SaveLayout(3); }));
    AddShortcut(SettingsPtr, &UCogWindowConfig_Settings::Shortcut_SaveLayout4,      FSimpleDelegate::CreateLambda([this] () { SaveLayout(4); }));
    AddShortcut(SettingsPtr, &UCogWindowConfig_Settings::Shortcut_ResetLayout,      FSimpleDelegate::CreateLambda([this] () { ResetLayout(); }));
    
     LayoutsWindow = AddWindow<FCogWindow_Layouts>("Window.Layouts");
     SettingsWindow = AddWindow<FCogWindow_Settings>("Window.Settings");

    for (FCogWindow* Window : Windows)
    {
        InitializeWindow(Window);   
    }
    
    FCogConsoleCommandManager::RegisterWorldConsoleCommand(
        *ToggleInputCommand, 
        TEXT("Toggle the input focus between the Game and ImGui"),
        GetWorld(),
        FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
        {
            ToggleInputMode();
        }));
    
    FCogConsoleCommandManager::RegisterWorldConsoleCommand(
        *DisableInputCommand,
        TEXT("Disable ImGui input"), 
        GetWorld(),
        FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
        {
            DisableInputMode();
        }));
    
    FCogConsoleCommandManager::RegisterWorldConsoleCommand(
        *ResetLayoutCommand,
        TEXT("Reset the layout."),
        GetWorld(),
        FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
        {
            if (InArgs.Num() > 0)
            {
                ResetLayout();
            }
        }));
    
    FCogConsoleCommandManager::RegisterWorldConsoleCommand(
        *LoadLayoutCommand,
        TEXT("Load the layout. Cog.LoadLayout <Index>"),
        GetWorld(),
        FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
        {
            if (InArgs.Num() > 0)
            {
                LoadLayout(FCString::Atoi(*InArgs[0]));
            }
        }));
    
    FCogConsoleCommandManager::RegisterWorldConsoleCommand(
        *SaveLayoutCommand,
        TEXT("Save the layout. Cog.SaveLayout <Index>"),
        GetWorld(),
        FCogWindowConsoleCommandDelegate::CreateLambda([this](const TArray<FString>& InArgs, UWorld* InWorld)
        {
            if (InArgs.Num() > 0)
            {
                SaveLayout(FCString::Atoi(*InArgs[0]));
            }
        }));


    bIsInitialized = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::Shutdown()
{
    FCogImGuiContextScope ImGuiContextScope(Context);

    if (bIsInitialized)
    {
        Context.SaveSettings();
    }
    
    for (FCogWindow* Window : Windows)
    {
        Window->Shutdown();
        delete Window;
    }
    Windows.Empty();

    if (bIsInitialized)
    {
        // Prevent ImGuiContextScope to contain dangling pointers when the contexts get destroyed.
        ImGuiContextScope.ClearPreviousContexts();

        Context.Shutdown();
    }
    
    FCogConsoleCommandManager::UnregisterAllWorldConsoleCommands(GetWorld());
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::SaveAllSettings()
{
    //------------------------------------------------------------------
    // Call PreSaveConfig before destroying imgui context
    // if PreSaveConfig needs to read ImGui IO for example
    //------------------------------------------------------------------
    for (FCogWindow* Window : Windows)
    {
        Window->PreSaveConfig();
    }
    
    for (UCogCommonConfig* Config : Configs)
    {
        Config->SaveConfig();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::ReloadAllSettings()
{
    for (UCogCommonConfig* Config : Configs)
    {
        Config->ReloadConfig();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::Tick(float InDeltaTime)
{
    UWorld* World = GetWorld();

    const float RealDeltaTime = World->DeltaRealTimeSeconds;

    //----------------------------------------------------------------------------------------------
    // When changing world the DebugExecBindings can change. 
    //----------------------------------------------------------------------------------------------
    if (World != CurrentWorld.Get())
    {
        RequestDisableCommandsConflictingWithShortcuts();
    }
    CurrentWorld = World;
    
    if (World == nullptr)
    { return; }
    
    if (bIsInitialized == false)
    {
        TryInitialize(*World);
        return;
    }

    FCogImGuiContextScope ImGuiContextScope(Context);

    UpdatePlayerControllers(*World);

    if (auto* LocalPlayerControllerPtr = LocalPlayerController.Get())
    {
        if (UPlayerInput* PlayerInput = LocalPlayerControllerPtr->PlayerInput)
        {
            //------------------------------------------------------------------
            // We must wait for the player input to be valid to disable
            // DebugExecBindings  conflicting with our shortcuts.
            //------------------------------------------------------------------
            TryDisableCommandsConflictingWithShortcuts(PlayerInput);
        }
    }
    
    if (LayoutToLoad != -1)
    {
	    const FString Filename = FCogImguiHelper::GetIniFilePath(FString::Printf(TEXT("ImGui_Layout_%d"), LayoutToLoad));
        ImGui::LoadIniSettingsFromDisk(COG_TCHAR_TO_CHAR(*Filename));
        LayoutToLoad = -1;
    }

    for (FCogWindow* Window : Windows)
    {
        Window->GameTick(RealDeltaTime);
    }

    const bool ShouldSkipRendering = NetImgui::IsConnected() && bIsSelectionModeActive == false;
    Context.SetSkipRendering(ShouldSkipRendering);

    if (Context.BeginFrame(RealDeltaTime))
    {
        Render(RealDeltaTime);
        Context.EndFrame();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::RequestDisableCommandsConflictingWithShortcuts()
{
    NumExecBindingsChecked = INDEX_NONE;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::SetLocalPlayerController(APlayerController& PlayerController)
{
    if (LocalPlayerController.Get() == &PlayerController)
    { return; }

    LocalPlayerController = &PlayerController;
    InputComponent.Reset();
    
    if (UInputComponent* InputComponentPtr = NewObject<UInputComponent>(&PlayerController, TEXT("Cog_Input")))
    {
        InputComponent = InputComponentPtr;
        PlayerController.PushInputComponent(InputComponentPtr);
    }
                
    for (FCogShortcut& Shortcut : Shortcuts)
    {
        BindShortcut(Shortcut);
    }

    if (LocalPlayerController->PlayerInput != nullptr)
    {
        FCogImguiInputHelper::DisableCommandsConflictingWithShortcuts(*LocalPlayerController->PlayerInput);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::UpdatePlayerControllers(UWorld& World)
{
    TArray<UCogDebugPluginSubsystem*> PluginSubsystems;

    ServerPlayerControllers.RemoveAll([] (TWeakObjectPtr<APlayerController> PlayerController)
    {
        return PlayerController.IsValid() == false;
    });

    for (FConstPlayerControllerIterator It = World.GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PlayerController = It->Get();
        if (IsValid(PlayerController) == false)
        { continue; }

        if (PlayerController->IsLocalController())
        {
            SetLocalPlayerController(*PlayerController);
        }

        if (World.GetNetMode() != NM_Client)
        {
            if (ServerPlayerControllers.Contains(PlayerController))
            { continue; }

            ServerPlayerControllers.Add(PlayerController);
        
            if (PluginSubsystems.IsEmpty())
            {
                PluginSubsystems = World.GetGameInstance()->GetSubsystemArrayCopy<UCogDebugPluginSubsystem>();
            }
        
            for (UCogDebugPluginSubsystem* PluginSubsystem : PluginSubsystems)
            {
                PluginSubsystem->OnPlayerControllerReady(PlayerController);
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::Render(float DeltaTime)
{
    FCogImGuiContextScope ImGuiContextScope(Context);
    
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingOverCentralNode | ImGuiDockNodeFlags_AutoHideTabBar);
    ImGui::PopStyleColor(1);

    const bool bCompactSaved = Settings->bCompactMode;
    if (bCompactSaved)
    {
        FCogWidgets::PushStyleCompact();
    }

    //----------------------------------------------------------------------
    // There is no need to have Imgui input enabled if the imgui rendering 
    // is only done on the NetImgui server. So we disable imgui input.
    //----------------------------------------------------------------------
    if (Context.GetEnableInput() && NetImgui::IsConnected() && bIsSelectionModeActive == false)
    {
        Context.SetEnableInput(false);
    }

    if ((Context.GetEnableInput() || NetImgui::IsConnected()) && bIsSelectionModeActive == false)
    {
        RenderMainMenu();
    }

    for (FCogWindow* Window : Windows)
    {
        Window->RenderTick(DeltaTime);

        if (Window->GetIsVisible() && bIsSelectionModeActive == false)
        {
            if (Settings->bTransparentMode)
            {
                ImGui::SetNextWindowBgAlpha(0.35f);
            }

            Window->Render(DeltaTime);
        }
    }
    
    if (bCompactSaved)
    {
        FCogWidgets::PopStyleCompact();
    }

    FCogDebugDrawImGui::Draw();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::AddWindow(FCogWindow* Window, const FString& Name)
{
    if (Windows.ContainsByPredicate([&](const FCogWindow* w) { return w->GetName() == Name; }))
    {
        COG_LOG_FUNC(LogCogImGui, ELogVerbosity::Warning, TEXT("Trying to add a window, but one already exist with the same name: %s"), *Name);
        return;
    }
    
    Window->SetFullName(Name);
    Window->SetOwner(this);
    Windows.Add(Window);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::InitializeWindow(FCogWindow* Window)
{
    Window->Initialize();

    if (Window->HasWidget())
    {
        Widgets.Add(Window);
        //Widgets.Sort()
    }

    if (Window->bShowInMainMenu)
    {
        if (FMenu* Menu = AddMenu(Window->GetFullName()))
        {
            Menu->Window = Window;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FCogWindow* UCogSubsystem::FindWindowByID(const ImGuiID ID)
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
void UCogSubsystem::ResetLayout()
{
    FCogImGuiContextScope ImGuiContextScope(Context);

    for (const FCogWindow* Window : Windows)
    {
        ImGui::SetWindowPos(COG_TCHAR_TO_CHAR(*Window->GetName()), ImVec2(10, 10), ImGuiCond_Always);
    }

    ImGui::ClearIniSettings();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::CloseAllWindows()
{
    for (FCogWindow* Window : Windows)
    {
        Window->SetIsVisible(false);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::LoadLayout(const int32 LayoutIndex)
{
    for (FCogWindow* Window : Windows)
    {
        Window->SetIsVisible(false);
    }

    LayoutToLoad = LayoutIndex;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::SaveLayout(const int32 LayoutIndex)
{
    FCogImGuiContextScope ImGuiContextScope(Context);

	const FString Filename = *FCogImguiHelper::GetIniFilePath(FString::Printf(TEXT("imgui_layout_%d"), LayoutIndex));
    ImGui::SaveIniSettingsToDisk(COG_TCHAR_TO_CHAR(*Filename));
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::SortMainMenu()
{
    MainMenu.SubMenus.Empty();

    TArray<FCogWindow*> SortedWindows = Windows;
    SortedWindows.Sort([](const FCogWindow& Lhs, const FCogWindow& Rhs) { return Lhs.GetFullName() < Rhs.GetFullName(); });

    for (FCogWindow* Window : SortedWindows)
    {
        if (FMenu* Menu = AddMenu(Window->GetFullName()))
        {
            Menu->Window = Window;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
UCogSubsystem::FMenu* UCogSubsystem::AddMenu(const FString& Name)
{
    TArray<FString> Path;
    Name.ParseIntoArray(Path, TEXT("."));

    FMenu* CurrentMenu = &MainMenu;
    for (int i = 0; i < Path.Num(); ++i)
    {
        FString MenuName = Path[i];

        int SubMenuIndex = CurrentMenu->SubMenus.IndexOfByPredicate([&](const FMenu& Menu) { return Menu.Name == MenuName; });
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
void UCogSubsystem::RenderMainMenu()
{
    bIsRenderingInMainMenu = true;

    //-----------------------------------------------------------------------------------------------
    // Prevent having a small gap on the right of the main menu, where some widgets are displayed
    //-----------------------------------------------------------------------------------------------
    ImGui::PushStyleVarX(ImGuiStyleVar_WindowPadding, 0.0f);
    
    const bool ShowMainMenu = ImGui::BeginMainMenuBar();
    
    ImGui::PopStyleVar();

    if (ShowMainMenu)
    {
        for (FMenu& Menu : MainMenu.SubMenus)
        {
            RenderOptionMenu(Menu);
        }

        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Close All Windows"))
            {
                CloseAllWindows();
            }
            
            ImGui::Separator();

            RenderMenuItem(*LayoutsWindow, "Layouts");
            RenderMenuItem(*SettingsWindow, "Settings");

            if (ImGui::BeginMenu("Spacing"))
            {
                for (FCogWindow* SpaceWindow : SpaceWindows)
                {
                    bool bSpaceVisible = SpaceWindow->GetIsVisible();
                    if (ImGui::MenuItem(COG_TCHAR_TO_CHAR(*SpaceWindow->GetName()), nullptr, &bSpaceVisible))
                    {
                        SpaceWindow->SetIsVisible(bSpaceVisible);
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
        
        RenderWidgets();
        ImGui::EndMainMenuBar();
    }    
    
    bIsRenderingInMainMenu = false;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::RenderWidgets()
{
    int32 NumVisibleWidgets = 0;
    for (int i = 0; i < Widgets.Num(); ++i)
    {
        FCogWindow* Window = Widgets[i];
        if (Window->GetIsWidgetVisible())
        {
            NumVisibleWidgets++;
        }
    }

    if (NumVisibleWidgets == 0)
    { return; }
    
    const bool AddLeftColumn = Settings->WidgetAlignment == ECogWidgetAlignment::Right
                    || Settings->WidgetAlignment == ECogWidgetAlignment::Manual
                    || Settings->WidgetAlignment == ECogWidgetAlignment::Center;

    const bool AddRightColumn = Settings->WidgetAlignment == ECogWidgetAlignment::Center;

    int32 NumColumns = NumVisibleWidgets;
    if (AddLeftColumn)
    {
        NumColumns++;
    }
    
    if (AddRightColumn)
    {
        NumColumns ++;
    }
    
    ImGuiTableFlags Flags = ImGuiTableFlags_None;
    if (Settings->ShowWidgetBorders)
    {
        Flags |= ImGuiTableFlags_BordersInnerV;
        if (Settings->WidgetAlignment == ECogWidgetAlignment::Left)
        {
            Flags |= ImGuiTableFlags_BordersOuterV;
        }
    } 

    if (Settings->WidgetAlignment == ECogWidgetAlignment::Manual)
    {
        Flags |= ImGuiTableFlags_Resizable;
        if (Settings->ShowWidgetBorders == false)
        {
            Flags |= ImGuiTableFlags_NoBordersInBodyUntilResize;
        }
    }

    //ImGui::PushStyleVarX(ImGuiStyleVar_CellPadding, 0.0f);
    
    if (ImGui::BeginTable("Widgets", NumColumns, Flags))
    {
        if (AddLeftColumn)
        {
            ImGui::TableSetupColumn("Stretch", ImGuiTableColumnFlags_WidthStretch);
        }
        
        for (int i = 0; i < NumVisibleWidgets; ++i)
        {
            ImGui::TableSetupColumn("Fixed", ImGuiTableColumnFlags_WidthFixed);
        }

        if (AddRightColumn)
        {
            ImGui::TableSetupColumn("Stretch", ImGuiTableColumnFlags_WidthStretch);
        }
        
        ImGui::TableNextRow();

        if (AddLeftColumn)
        {
            ImGui::TableNextColumn();
        }

        //---------------------------------------------------------------------
        // Widgets 
        //---------------------------------------------------------------------
        for (int column = 0; column < Widgets.Num(); ++column)
        {
            ImGui::PushID(column);
            
            FCogWindow* Window = Widgets[column];
            if (Window->GetIsWidgetVisible())
            {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
                Window->RenderMainMenuWidget();
            }

            ImGui::PopID();
        }

        if (AddRightColumn)
        {
            ImGui::TableNextColumn();
        }

        ImGui::EndTable();
    }
    //ImGui::PopStyleVar();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::RenderOptionMenu(FMenu& Menu)
{
    if (Menu.Window != nullptr)
    {
        RenderMenuItem(*Menu.Window, COG_TCHAR_TO_CHAR(*Menu.Name));
    }
    else
    {
        if (ImGui::BeginMenu(COG_TCHAR_TO_CHAR(*Menu.Name)))
        {
            for (FMenu& SubMenu : Menu.SubMenus)
            {
                RenderOptionMenu(SubMenu);
            }
            ImGui::EndMenu();
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::RenderMenuItem(FCogWindow& Window, const char* MenuItemName)
{
    if (Settings->bShowWindowsInMainMenu)
    {
        ImGui::SetNextWindowSizeConstraints(
            ImVec2(FCogWidgets::GetFontWidth() * 40, ImGui::GetTextLineHeightWithSpacing() * 5),
            ImVec2(FCogWidgets::GetFontWidth() * 50, ImGui::GetTextLineHeightWithSpacing() * 60));

        if (ImGui::BeginMenu(MenuItemName))
        {
            Window.RenderContent();
            ImGui::EndMenu();
        }

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        {
            Window.SetIsVisible(!Window.GetIsVisible());
        }
        
        RenderMenuItemHelp(Window);
    }
    else
    {
        bool bIsVisible = Window.GetIsVisible();
        if (ImGui::MenuItem(MenuItemName, nullptr, &bIsVisible))
        {
            Window.SetIsVisible(bIsVisible);
        }
     
        RenderMenuItemHelp(Window);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::RenderMenuItemHelp(FCogWindow& Window)
{
    if (Settings->bShowHelp)
    {
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - FCogWidgets::GetFontWidth() * 3.0f);
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(29, 42, 62, 240));
            const float HelpWidth = FCogWidgets::GetFontWidth() * 80;
            ImGui::SetNextWindowSizeConstraints(ImVec2(HelpWidth / 2.0f, 0.0f), ImVec2(HelpWidth, FLT_MAX));
            if (ImGui::BeginTooltip())
            {
                ImGui::PushTextWrapPos(HelpWidth - 1 * FCogWidgets::GetFontWidth());
                Window.RenderHelp();
                ImGui::Separator();
                ImGui::TextDisabled("Help can be hidden in Window/Settings.");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
            ImGui::PopStyleColor();
        }
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(FCogWidgets::GetFontWidth() * 1, 0));
    }
}



//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::SettingsHandler_ClearAll(ImGuiContext* Context, ImGuiSettingsHandler* Handler)
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::SettingsHandler_ApplyAll(ImGuiContext* Context, ImGuiSettingsHandler* Handler)
{
    UCogSubsystem* CogSubsystem = static_cast<UCogSubsystem*>(Handler->UserData);

    CogSubsystem->Widgets.Sort([](const FCogWindow& Window1, const FCogWindow& Window2)
    {
        return Window1.GetWidgetOrderIndex() < Window2.GetWidgetOrderIndex();
    });
}

//--------------------------------------------------------------------------------------------------------------------------
void* UCogSubsystem::SettingsHandler_ReadOpen(ImGuiContext* Context, ImGuiSettingsHandler* Handler, const char* Name)
{
    if (strcmp(Name, "Windows") == 0)
    {
        return reinterpret_cast<void*>(1);
    }

    if (strcmp(Name, "Widgets") == 0)
    {
        UCogSubsystem* CogSubsystem = static_cast<UCogSubsystem*>(Handler->UserData);
        CogSubsystem->WidgetsOrderIndex = 0;

        return reinterpret_cast<void*>(2);
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::SettingsHandler_ReadLine(ImGuiContext* Context, ImGuiSettingsHandler* Handler, void* Entry, const char* Line)
{
    //-----------------------------------------------------------------------------------
	// Load the visibility of windows. 
	//-----------------------------------------------------------------------------------
    if (Entry == reinterpret_cast<void*>(1))
    {
        ImGuiID Id;
        int32 ShowMenu = 0;
#if PLATFORM_WINDOWS || PLATFORM_MICROSOFT
        if (sscanf_s(Line, "0x%08X %d", &Id, &ShowMenu) == 2)
#else
        if (sscanf(Line, "0x%08X", &Id) == 1)
#endif
        {
            UCogSubsystem* CogSubsystem = static_cast<UCogSubsystem*>(Handler->UserData);
            if (FCogWindow* Window = CogSubsystem->FindWindowByID(Id))
            {
                Window->SetIsVisible(true);
                Window->bShowMenu = (ShowMenu > 0);
            }
        }
    }
    //-----------------------------------------------------------------------------------
    // Load which widgets are present in the main menu bar and with what order. 
    //-----------------------------------------------------------------------------------
    else if (Entry == reinterpret_cast<void*>(2))
    {
        ImGuiID Id;
        int32 Visible = false;
#if PLATFORM_WINDOWS || PLATFORM_MICROSOFT
        if (sscanf_s(Line, "0x%08X %d", &Id, &Visible) == 2)
#else
        if (sscanf(Line, "0x%08X %d", &Id, &Visible) == 2)
#endif
        {
            UCogSubsystem* CogSubsystem = static_cast<UCogSubsystem*>(Handler->UserData);
            if (FCogWindow* Window = CogSubsystem->FindWindowByID(Id))
            {
                Window->SetWidgetOrderIndex(CogSubsystem->WidgetsOrderIndex);
                Window->SetIsWidgetVisible(Visible > 0);
            }

            CogSubsystem->WidgetsOrderIndex++;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::SettingsHandler_WriteAll(ImGuiContext* Context, ImGuiSettingsHandler* Handler, ImGuiTextBuffer* Buffer)
{
    UCogSubsystem* CogSubsystem = static_cast<UCogSubsystem*>(Handler->UserData);

    CogSubsystem->SaveAllSettings();

    //-----------------------------------------------------------------------------------
	// Save the visibility of windows. Example:
	//      [Cog][Windows]
	//	    0xB5D96693
    //      0xBF3390B5
	//-----------------------------------------------------------------------------------
    Buffer->appendf("[%s][Windows]\n", Handler->TypeName);
    for (const FCogWindow* Window : CogSubsystem->Windows)
    {
        if (Window->GetIsVisible())
        {
            Buffer->appendf("0x%08X %d\n", Window->GetID(), static_cast<int32>(Window->bShowMenu));
        }
    }
    Buffer->append("\n");

    //-----------------------------------------------------------------------------------
	// Save which widgets are present in the main menu bar and with what order. Example:
    //      [Cog][Widgets]
    //      0x639F1181 1
	//      0x52BDE3E0 1
    //-----------------------------------------------------------------------------------
    Buffer->appendf("[%s][Widgets]\n", Handler->TypeName);
    for (const FCogWindow* Window : CogSubsystem->Widgets)
    {
        Buffer->appendf("0x%08X %d\n", Window->GetID(), Window->GetIsWidgetVisible());
    }
    Buffer->append("\n");
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::ResetAllWindowsConfig()
{
    for (FCogWindow* Window : Windows)
    {
        Window->ResetConfig();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::AddCommand(UPlayerInput* PlayerInput, const FString& Command, const FKey& Key)
{
    if (PlayerInput == nullptr)
    { return; }

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
void UCogSubsystem::SortCommands(UPlayerInput* PlayerInput)
{
    PlayerInput->DebugExecBindings.Sort([](const FKeyBind& Key1, const FKeyBind& Key2)
    {
        return Key1.Command.Compare(Key2.Command) < 0;
    });
}

//--------------------------------------------------------------------------------------------------------------------------
UCogCommonConfig* UCogSubsystem::GetConfig(const TSubclassOf<UCogCommonConfig>& ConfigClass)
{
    const UClass* Class = ConfigClass.Get();

    for (UCogCommonConfig* Config : Configs)
    {
        if (Config && Config->IsA(Class))
        { return Cast<UCogCommonConfig>(Config); }
    }

    UCogCommonConfig* Config = NewObject<UCogCommonConfig>(this, Class);
    Config->Reset();
    Config->ReloadConfig();
    
    Configs.Add(Config);
    return Config;
}

//--------------------------------------------------------------------------------------------------------------------------
const UObject* UCogSubsystem::GetAsset(const TSubclassOf<UObject>& AssetClass) const
{
    const UClass* Class = AssetClass.Get();

    for (const UObject* Asset : Assets)
    {
        if (Asset && Asset->IsA(Class))
        { return Asset; }
    }

    const UObject* Asset = FCogHelper::GetFirstAssetByClass(AssetClass);
    if (Asset == nullptr)
    { return nullptr; }

    Assets.Add(Asset);

    return Asset;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::ToggleInputMode()
{
    UE_LOG(LogCogImGui, Verbose, TEXT("UCogSubsystem::ToggleInputMode"));
    Context.SetEnableInput(!Context.GetEnableInput());
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::DisableInputMode()
{
    UE_LOG(LogCogImGui, Verbose, TEXT("UCogSubsystem::DisableInputMode"));
    Context.SetEnableInput(false);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::SetActivateSelectionMode(const bool Value)
{
    SelectionModeActiveCounter = FMath::Max(SelectionModeActiveCounter + (Value ? 1 : -1), 0);
    bIsSelectionModeActive = SelectionModeActiveCounter > 0;

    if (bIsSelectionModeActive)
    {
        bIsInputEnabledBeforeEnteringSelectionMode = GetContext().GetEnableInput();

        Context.SetEnableInput(true);
    }
    else
    {
        GetContext().SetEnableInput(bIsInputEnabledBeforeEnteringSelectionMode);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSubsystem::GetActivateSelectionMode() const
{
    return SelectionModeActiveCounter > 0;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::AddShortcut(const UObject& InInstance, const FProperty& InProperty, const FSimpleDelegate& Delegate)
{
    FCogShortcut& Shortcut = Shortcuts.AddDefaulted_GetRef();
    Shortcut.PropertyName = InProperty.GetFName();
    Shortcut.Config = &InInstance;
    Shortcut.Delegate = Delegate;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSubsystem::BindShortcut(FCogShortcut& InShortcut) const
{
    UObject* Config = const_cast<UObject*>(InShortcut.Config.Get());
    if (Config == nullptr)
    { return false; }
    
    const FStructProperty* StructProperty = CastField<FStructProperty>(Config->GetClass()->FindPropertyByName(InShortcut.PropertyName));
    if (StructProperty == nullptr)
    { return false; }

    const FCogInputChord* InputChord = static_cast<FCogInputChord*>(StructProperty->ContainerPtrToValuePtr<void>(Config));
    if (InputChord == nullptr)
    { return false; }
        
    FInputKeyBinding InputBinding(*InputChord, IE_Pressed);

    InputBinding.KeyDelegate.GetDelegateForManualSet() = FInputActionHandlerSignature::CreateLambda([this, &InShortcut]()
        {
            FCogImGuiContextScope ImGuiContextScope(Context);

            if (ImGui::GetIO().WantTextInput == false || InShortcut.InputChord.bTriggerWhenImguiWantTextInput)
            {
                InShortcut.Delegate.ExecuteIfBound();
            }
        });


    if (UInputComponent* InputComponentPtr = InputComponent.Get())
    {
        InputComponentPtr->KeyBindings.Add(InputBinding);
    }
    
    InShortcut.InputChord = *InputChord;

    FCogImguiInputHelper::GetPrioritizedShortcuts().AddUnique(*InputChord);
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::RebindShortcut(const UCogCommonConfig& InConfig, const FProperty& InProperty)
{
    // Ideally would unbind and rebind only the provided shortcut, but we currently rebind all shortcuts. 

    UInputComponent* InputComponentPtr = InputComponent.Get();
    if (InputComponentPtr ==  nullptr)
    { return; }

    for (auto& KeyBinding : InputComponentPtr->KeyBindings)
    {
        KeyBinding.KeyDelegate.Unbind();
    }
    InputComponentPtr->KeyBindings.Empty();

    for (FCogShortcut& Shortcut : Shortcuts)
    {
        BindShortcut(Shortcut);
    }

    RequestDisableCommandsConflictingWithShortcuts();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSubsystem::TryDisableCommandsConflictingWithShortcuts(UPlayerInput* PlayerInput)
{
    const int32 NewNumExecBindings = PlayerInput->DebugExecBindings.Num();
    if (NumExecBindingsChecked == NewNumExecBindings)
    { return; }

    NumExecBindingsChecked = NewNumExecBindings;;

    if (Settings->bDisableConflictingCommands == false)
    { return; }
    
    TArray<FCogInputChord>& PrioritizedShortcuts = FCogImguiInputHelper::GetPrioritizedShortcuts();
    for (const FCogShortcut& Shortcut : Shortcuts)
    {
        PrioritizedShortcuts.Add(Shortcut.InputChord);
    }
        
    FCogImguiInputHelper::DisableCommandsConflictingWithShortcuts(*PlayerInput);
}