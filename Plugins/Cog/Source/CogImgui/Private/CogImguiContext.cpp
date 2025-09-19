#include "CogImguiContext.h"

#include "Application/ThrottleManager.h"
#include "CogImguiHelper.h"
#include "CogImguiInputCatcherWidget.h"
#include "CogImguiInputHelper.h"
#include "CogImguiWidget.h"
#include "Engine/Console.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/SlateUser.h"
#include "GameFramework/PlayerController.h"
#include "HAL/PlatformApplicationMisc.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "Misc/EngineVersionComparison.h"
#include "NetImgui_Api.h"
#include "TextureResource.h"
#include "Widgets/SViewport.h"
#include "Widgets/SWindow.h"
#include "CogLocalizationConfig.h"

//--------------------------------------------------------------------------------------------------------------------------
FCogImGuiContextScope::FCogImGuiContextScope(const FCogImguiContext& CogImguiContext)
{
    PrevImGuiContext = ImGui::GetCurrentContext();
    PrevPlotContext = ImPlot::GetCurrentContext();

    ImGui::SetCurrentContext(CogImguiContext.Context);
    ImPlot::SetCurrentContext(CogImguiContext.PlotContext);
}

//--------------------------------------------------------------------------------------------------------------------------
FCogImGuiContextScope::FCogImGuiContextScope(ImGuiContext* GuiCtx, ImPlotContext* PlotCtx)
{
    PrevImGuiContext = ImGui::GetCurrentContext();
    PrevPlotContext = ImPlot::GetCurrentContext();

    ImGui::SetCurrentContext(GuiCtx);
    ImPlot::SetCurrentContext(PlotCtx);
}

//--------------------------------------------------------------------------------------------------------------------------
FCogImGuiContextScope::~FCogImGuiContextScope()
{
    ImGui::SetCurrentContext(PrevImGuiContext);
    ImPlot::SetCurrentContext(PrevPlotContext);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImGuiContextScope::ClearPreviousContexts()
{
    PrevImGuiContext = nullptr;
    PrevPlotContext = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiContext::bIsNetImGuiInitialized = false;

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::Initialize(UGameViewportClient* InGameViewport)
{
    IMGUI_CHECKVERSION();

    GameViewport = InGameViewport;

    // ImGui Context must be created before creating widgets as widgets can receive events that uses the ImGui context right away.
    Context = ImGui::CreateContext();
    PlotContext = ImPlot::CreateContext();
    ImGui::SetCurrentContext(Context);
    ImPlot::SetImGuiContext(Context);
    ImPlot::SetCurrentContext(PlotContext);

    if (GameViewport != nullptr)
    {
        SAssignNew(MainWidget, SCogImguiWidget).Context(this);
        GameViewport->AddViewportWidgetContent(MainWidget.ToSharedRef(), TNumericLimits<int32>::Max());

        SAssignNew(InputCatcherWidget, SCogImguiInputCatcherWidget).Context(this);
        GameViewport->AddViewportWidgetContent(InputCatcherWidget.ToSharedRef(), -TNumericLimits<int32>::Max());
    }

    ImGuiIO& IO = ImGui::GetIO();
    IO.UserData = this;


    IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    IO.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    IO.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    IO.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
    IO.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;
    IO.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
    IO.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    //--------------------------------------------------------------------
    // 
    //--------------------------------------------------------------------
    ImGuiViewport* MainViewport = ImGui::GetMainViewport();
    FCogImGuiViewportData* ViewportData = new FCogImGuiViewportData();
    MainViewport->PlatformUserData = ViewportData;
    ViewportData->Window = FSlateApplication::IsInitialized() ? FSlateApplication::Get().GetActiveTopLevelWindow() : nullptr;
    ViewportData->Context = this;
    ViewportData->Widget = MainWidget;

    const auto InitFilenameTemp = StringCast<ANSICHAR>(*FCogImguiHelper::GetIniFilePath("imgui"));
    ImStrncpy(IniFilename, InitFilenameTemp.Get(), IM_ARRAYSIZE(IniFilename));
    IO.IniFilename = IniFilename;

    ImGuiPlatformIO& PlatformIO = ImGui::GetPlatformIO();
    PlatformIO.Platform_CreateWindow = ImGui_CreateWindow;
    PlatformIO.Platform_DestroyWindow = ImGui_DestroyWindow;
    PlatformIO.Platform_ShowWindow = ImGui_ShowWindow;
    PlatformIO.Platform_SetWindowPos = ImGui_SetWindowPos;
    PlatformIO.Platform_GetWindowPos = ImGui_GetWindowPos;
    PlatformIO.Platform_SetWindowSize = ImGui_SetWindowSize;
    PlatformIO.Platform_GetWindowSize = ImGui_GetWindowSize;
    PlatformIO.Platform_SetWindowFocus = ImGui_SetWindowFocus;
    PlatformIO.Platform_GetWindowFocus = ImGui_GetWindowFocus;
    PlatformIO.Platform_GetWindowMinimized = ImGui_GetWindowMinimized;
    PlatformIO.Platform_SetWindowTitle = ImGui_SetWindowTitle;
    PlatformIO.Platform_SetWindowAlpha = ImGui_SetWindowAlpha;
    PlatformIO.Platform_RenderWindow = ImGui_RenderWindow;
    
    PlatformIO.Platform_ClipboardUserData = &ClipboardBuffer;
    PlatformIO.Platform_GetClipboardTextFn = ImGui_GetClipboardTextFn;
    PlatformIO.Platform_SetClipboardTextFn = ImGui_SetClipboardTextFn;
    PlatformIO.Platform_OpenInShellFn = ImGui_OpenInShell;

    if (FSlateApplication::IsInitialized())
    {
        if (const TSharedPtr<GenericApplication> PlatformApplication = FSlateApplication::Get().GetPlatformApplication())
        {
            FDisplayMetrics DisplayMetrics;
            PlatformApplication->GetInitialDisplayMetrics(DisplayMetrics);
            PlatformApplication->OnDisplayMetricsChanged().AddRaw(this, &FCogImguiContext::OnDisplayMetricsChanged);
            OnDisplayMetricsChanged(DisplayMetrics);
        }
    }
    else
    {
        FMonitorInfo monitorInfo;
        monitorInfo.bIsPrimary = true;
        monitorInfo.DisplayRect = FPlatformRect(0, 0, 1080, 720);
        FDisplayMetrics DisplayMetrics;
        DisplayMetrics.MonitorInfo.Add(monitorInfo);
        OnDisplayMetricsChanged(DisplayMetrics);
    }

#if NETIMGUI_ENABLED
    if (bIsNetImGuiInitialized == false)
    {
        NetImgui::Startup();
        bIsNetImGuiInitialized = true;
    }
#endif
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::Shutdown()
{
    FCogImGuiContextScope ImGuiContextScope(Context, PlotContext);

    //------------------------------------------------------------------
    // NetImgui must be shutdown before imgui as it uses context hooks
    //------------------------------------------------------------------
#if NETIMGUI_ENABLED
    if (bIsNetImGuiInitialized)
    {
        NetImgui::Shutdown();
        bIsNetImGuiInitialized = false;
    }
#endif

    if (ImGuiViewport* MainViewport = ImGui::GetMainViewport())
    {
        if (const FCogImGuiViewportData* ViewportData = static_cast<FCogImGuiViewportData*>(MainViewport->PlatformUserData))
        {
            delete ViewportData;
            MainViewport->PlatformUserData = nullptr;
        }
    }

    if (FSlateApplication::IsInitialized())
    {
        if (const TSharedPtr<GenericApplication> PlatformApplication = FSlateApplication::Get().GetPlatformApplication())
        {
            PlatformApplication->OnDisplayMetricsChanged().RemoveAll(this);
        }
    }

    if (GameViewport != nullptr)
    {
        GameViewport->RemoveViewportWidgetContent(MainWidget.ToSharedRef());
        GameViewport->RemoveViewportWidgetContent(InputCatcherWidget.ToSharedRef());
    }

    //Make sure ImGuiContextScope does not contain dangling pointer as we will destroy the contexts.
    ImGuiContextScope.ClearPreviousContexts();

    if (PlotContext != nullptr)
    {
        ImPlot::DestroyContext(PlotContext);
        PlotContext = nullptr;
    }

    if (Context != nullptr)
    {
        // Prevent triggering the saving of ImGui config. This is done manually by calling SaveSettings.
        Context->IO.IniFilename = nullptr;
        
        ImGui::DestroyContext(Context);
        Context = nullptr;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::SaveSettings() const
{
    if (Context && Context->SettingsLoaded && Context->IO.IniFilename != nullptr)
    {
        ImGui::SaveIniSettingsToDisk(Context->IO.IniFilename);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::OnImGuiWidgetFocusLost()
{
    if (bEnableInput == false)
    { return; }
    
    if (GameViewport == nullptr)
    { return; }

    const SViewport* ViewportWidget = GameViewport->GetGameViewportWidget().Get();
    if (ViewportWidget == nullptr)
    { return; }
    
    if (!ViewportWidget->HasUserFocus(0))
    { return; }

    bRetakeFocus = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::OnDisplayMetricsChanged(const FDisplayMetrics& DisplayMetrics) const
{
    FCogImGuiContextScope ImGuiContextScope(Context, PlotContext);

    ImGuiPlatformIO& PlatformIO = ImGui::GetPlatformIO();
    PlatformIO.Monitors.resize(0);

    for (const FMonitorInfo& Monitor : DisplayMetrics.MonitorInfo)
    {
        ImGuiPlatformMonitor ImGuiMonitor;
        ImGuiMonitor.MainPos = ImVec2(Monitor.DisplayRect.Left, Monitor.DisplayRect.Top);
        ImGuiMonitor.MainSize = ImVec2(Monitor.DisplayRect.Right - Monitor.DisplayRect.Left, Monitor.DisplayRect.Bottom - Monitor.DisplayRect.Top);
        ImGuiMonitor.WorkPos = ImVec2(Monitor.WorkArea.Left, Monitor.WorkArea.Top);
        ImGuiMonitor.WorkSize = ImVec2(Monitor.WorkArea.Right - Monitor.WorkArea.Left, Monitor.WorkArea.Bottom - Monitor.WorkArea.Top);
        ImGuiMonitor.DpiScale = 1.0f;

        if (Monitor.bIsPrimary)
        {
            PlatformIO.Monitors.push_front(ImGuiMonitor);
        }
        else
        {
            PlatformIO.Monitors.push_back(ImGuiMonitor);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiContext::BeginFrame(float InDeltaTime)
{
    FCogImGuiContextScope ImGuiContextScope(Context, PlotContext);
    
    //-------------------------------------------------------------------------------------------------------
    // Skip the first frame, to let the main widget update its TickSpaceGeometry which is returned by the 
    // platform callback ImGui_GetWindowPos. When using viewports Imgui needs to know the main viewport 
    // absolute position to correctly place the initial imgui windows. 
    //-------------------------------------------------------------------------------------------------------
    if (bIsFirstFrame)
    {
        bIsFirstFrame = false;
        return false;
    }

    //-------------------------------------------------------------------------------------------------------
    // Sometime the game can retake unaware that ImGui want to keep the focus and mouse unlock.
    // This typically happens when switching level.  
    //-------------------------------------------------------------------------------------------------------
    if (bRetakeFocus && IsConsoleOpened() == false)
    {
        SetEnableInput(true);
        bRetakeFocus = false;
    }

    ImGuiIO& IO = ImGui::GetIO();
    IO.DeltaTime = InDeltaTime;

    if (MainWidget != nullptr)
    {
        IO.DisplaySize = FCogImguiHelper::ToImVec2(MainWidget->GetTickSpaceGeometry().GetAbsoluteSize());
    }
    else
    {
        IO.DisplaySize = ImVec2(1080, 720);
    }

    //-------------------------------------------------------------------------------------------------------
    // Build font
    //-------------------------------------------------------------------------------------------------------
    if (IO.Fonts->IsBuilt() == false || FontAtlasTexturePtr.IsValid() == false)
    {
        BuildFont();
    }

    //-------------------------------------------------------------------------------------------------------
    // Update which viewport is under the mouse
    //-------------------------------------------------------------------------------------------------------
    if (FSlateApplication::IsInitialized())
    {
        ImGuiID MouseViewportId = 0;

        FWidgetPath WidgetsUnderCursor = FSlateApplication::Get().LocateWindowUnderMouse(
            FSlateApplication::Get().GetCursorPos(), 
            FSlateApplication::Get().GetInteractiveTopLevelWindows());

        if (WidgetsUnderCursor.IsValid())
        {
            TSharedRef<SWindow> Window = WidgetsUnderCursor.GetWindow();
            ImGuiID* ViewportId = WindowToViewportMap.Find(Window);

            if (ViewportId != nullptr)
            {
                MouseViewportId = *ViewportId;
            }
            else
            {
                MouseViewportId = ImGui::GetMainViewport()->ID;
            }
        }

        IO.AddMouseViewportEvent(MouseViewportId);

        //-------------------------------------------------------------------------------------------------------
        // Refresh modifiers otherwise, when pressing ALT-TAB, the Alt modifier is always true
        //-------------------------------------------------------------------------------------------------------
        if (bEnableInput)
        {
            FModifierKeysState ModifierKeys = FSlateApplication::Get().GetModifierKeys();
            if (ModifierKeys.IsControlDown() != IO.KeyCtrl) { IO.AddKeyEvent(ImGuiMod_Ctrl, ModifierKeys.IsControlDown()); }
            if (ModifierKeys.IsShiftDown() != IO.KeyShift) { IO.AddKeyEvent(ImGuiMod_Shift, ModifierKeys.IsShiftDown()); }
            if (ModifierKeys.IsAltDown() != IO.KeyAlt) { IO.AddKeyEvent(ImGuiMod_Alt, ModifierKeys.IsAltDown()); }
            if (ModifierKeys.IsCommandDown() != IO.KeySuper) { IO.AddKeyEvent(ImGuiMod_Super, ModifierKeys.IsCommandDown()); }
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // 
    //-------------------------------------------------------------------------------------------------------
    if (bEnableInput || NetImgui::IsConnected())
    {
        IO.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    }
    else
    {
        IO.ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }

    if (MainWidget != nullptr && FSlateApplication::IsInitialized())
    {
        const bool bHasMouse = (IO.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0;
        const bool bUpdateMouseMouseCursor = (IO.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0;
        if (bHasMouse && bUpdateMouseMouseCursor)
        {
            MainWidget->SetCursor(FCogImguiInputHelper::ToSlateMouseCursor(ImGui::GetMouseCursor()));
        }

        if (bEnableInput)
        {
            const ImVec2 mousePos = GetImguiMousePos();
            IO.AddMousePosEvent(mousePos.x, mousePos.y);
        }
    }

    bWantCaptureMouse = ImGui::GetIO().WantCaptureMouse;

    if (bRefreshDPIScale)
    {
        bRefreshDPIScale = false;

        BuildFont();

        ImGuiStyle NewStyle = ImGuiStyle();
        ImGui::GetStyle() = MoveTemp(NewStyle);
        NewStyle.ScaleAllSizes(DpiScale);
    }

    ImGui::NewFrame();
    //if (NetImgui::NewFrame(true) == false)
    //{
    //    return false;
    //}

    //DrawDebug();

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec2 FCogImguiContext::GetImguiMousePos() const
{
    const FVector2D& MousePosition = FSlateApplication::Get().GetCursorPos();
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        return ImVec2(MousePosition.X, MousePosition.Y);
    }

    const FVector2D TransformedMousePosition = MousePosition - MainWidget->GetTickSpaceGeometry().GetAbsolutePosition();
    return ImVec2(TransformedMousePosition.X, TransformedMousePosition.Y);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::EndFrame()
{
    FCogImGuiContextScope ImGuiContextScope(Context, PlotContext);

    ImGui::Render();
    //NetImgui::EndFrame();

    ImGui_RenderWindow(ImGui::GetMainViewport(), nullptr);

    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_CreateWindow(ImGuiViewport* Viewport)
{
    if (FSlateApplication::IsInitialized() == false)
    {
        return;
    }

    if (Viewport->ParentViewportId == 0)
    {
        return;
    }

    ImGuiViewport* ParentViewport = ImGui::FindViewportByID(Viewport->ParentViewportId);
    if (ParentViewport == nullptr)
    {
        return;
    }

    const FCogImGuiViewportData* ParentViewportData = static_cast<FCogImGuiViewportData*>(ParentViewport->PlatformUserData);
    if (ParentViewportData == nullptr)
    {
        return;
    }

    FCogImguiContext* Context = ParentViewportData->Context;

    const bool bTooltipWindow = (Viewport->Flags & ImGuiViewportFlags_TopMost);
    const bool bPopupWindow = (Viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon);

    static FWindowStyle WindowStyle = FWindowStyle()
        .SetActiveTitleBrush(FSlateNoResource())
        .SetInactiveTitleBrush(FSlateNoResource())
        .SetFlashTitleBrush(FSlateNoResource())
        .SetOutlineBrush(FSlateNoResource())
        .SetBorderBrush(FSlateNoResource())
        .SetBackgroundBrush(FSlateNoResource())
        .SetChildBackgroundBrush(FSlateNoResource());

    TSharedPtr<SCogImguiWidget> Widget;

    const TSharedRef<SWindow> Window =
        SNew(SWindow)
        //.Type(bTooltipWindow ? EWindowType::ToolTip : EWindowType::Normal)
        .Type(EWindowType::ToolTip)
        .Style(&WindowStyle)
        .ScreenPosition(FCogImguiHelper::ToFVector2D(Viewport->Pos))
        .ClientSize(FCogImguiHelper::ToFVector2D(Viewport->Size))
        .SupportsTransparency(EWindowTransparency::PerWindow)
        .SizingRule(ESizingRule::UserSized)
        .IsPopupWindow(bTooltipWindow || bPopupWindow)
        .IsTopmostWindow(bTooltipWindow)
        .FocusWhenFirstShown(!(Viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing))
        .HasCloseButton(false)
        .SupportsMaximize(false)
        .SupportsMinimize(false)
        .CreateTitleBar(false)
        .LayoutBorder(0)
        .UserResizeBorder(0)
        .UseOSWindowBorder(false)
        [
            SAssignNew(Widget, SCogImguiWidget)
                      .Context(Context)
        ];

    if (ParentViewportData->Window.IsValid())
    {
        FSlateApplication::Get().AddWindowAsNativeChild(Window, ParentViewportData->Window.Pin().ToSharedRef());
    }
    else
    {
        FSlateApplication::Get().AddWindow(Window);
    }

    Widget->SetWindow(Window);

    FCogImGuiViewportData* ViewportData = new FCogImGuiViewportData();
    Viewport->PlatformUserData = ViewportData;
    ViewportData->Context = ParentViewportData->Context;
    ViewportData->Widget = Widget;
    ViewportData->Window = Window;

    ParentViewportData->Context->WindowToViewportMap.Add(Window, Viewport->ID);

    Viewport->PlatformRequestResize = false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_DestroyWindow(ImGuiViewport* Viewport)
{
    FCogImGuiViewportData* ViewportData = static_cast<FCogImGuiViewportData*>(Viewport->PlatformUserData);
    if (ViewportData == nullptr)
    {
        return;
    }

    if ((Viewport->Flags & ImGuiViewportFlags_OwnedByApp))
    {
        return;
    }

    ViewportData->Context->WindowToViewportMap.Remove(ViewportData->Window);

    if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
    {
        Window->RequestDestroyWindow();
    }

    delete ViewportData;
    Viewport->PlatformUserData = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_ShowWindow(ImGuiViewport* Viewport)
{
    if (const FCogImGuiViewportData* ViewportData = static_cast<FCogImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            Window->ShowWindow();
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_SetWindowPos(ImGuiViewport* Viewport, ImVec2 Pos)
{
    if (const FCogImGuiViewportData* ViewportData = static_cast<FCogImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            Window->MoveWindowTo(FCogImguiHelper::ToFVector2D(Pos));
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec2 FCogImguiContext::ImGui_GetWindowPos(ImGuiViewport* Viewport)
{
    if (const FCogImGuiViewportData* ViewportData = static_cast<FCogImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SCogImguiWidget> Widget = ViewportData->Widget.Pin())
        {
            return FCogImguiHelper::ToImVec2(Widget->GetTickSpaceGeometry().GetAbsolutePosition());
        }
    }

    return ImVec2(0, 0);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_SetWindowSize(ImGuiViewport* Viewport, ImVec2 Size)
{
    if (const FCogImGuiViewportData* ViewportData = static_cast<FCogImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            Window->Resize(FCogImguiHelper::ToFVector2D(Size));
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec2 FCogImguiContext::ImGui_GetWindowSize(ImGuiViewport* Viewport)
{
    if (const FCogImGuiViewportData* ViewportData = static_cast<FCogImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SCogImguiWidget> Widget = ViewportData->Widget.Pin())
        {
            return FCogImguiHelper::ToImVec2(Widget->GetTickSpaceGeometry().GetAbsoluteSize());
        }
    }

    return ImVec2(0, 0);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_SetWindowFocus(ImGuiViewport* Viewport)
{
    if (const FCogImGuiViewportData* ViewportData = static_cast<FCogImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            if (const TSharedPtr<FGenericWindow> NativeWindow = Window->GetNativeWindow())
            {
                NativeWindow->BringToFront();
                NativeWindow->SetWindowFocus();
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiContext::ImGui_GetWindowFocus(ImGuiViewport* Viewport)
{
    if (const FCogImGuiViewportData* ViewportData = static_cast<FCogImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            if (const TSharedPtr<FGenericWindow> NativeWindow = Window->GetNativeWindow())
            {
                return NativeWindow->IsForegroundWindow();
            }
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiContext::ImGui_GetWindowMinimized(ImGuiViewport* Viewport)
{
    if (const FCogImGuiViewportData* ViewportData = static_cast<FCogImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            return Window->IsWindowMinimized();
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_SetWindowTitle(ImGuiViewport* Viewport, const char* TitleAnsi)
{
    if (const FCogImGuiViewportData* ViewportData = static_cast<FCogImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            Window->SetTitle(FText::FromString(ANSI_TO_TCHAR(TitleAnsi)));
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_SetWindowAlpha(ImGuiViewport* Viewport, float Alpha)
{
    if (const FCogImGuiViewportData* ViewportData = static_cast<FCogImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            Window->SetOpacity(Alpha);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_RenderWindow(ImGuiViewport* Viewport, void* Data)
{
    if (const FCogImGuiViewportData* ViewportData = static_cast<FCogImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SCogImguiWidget> Widget = ViewportData->Widget.Pin())
        {
            Widget->SetDrawData(Viewport->DrawData);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
const char* FCogImguiContext::ImGui_GetClipboardTextFn(ImGuiContext* InImGuiContext)
{
    TArray<char>* ClipboardBuffer = static_cast<TArray<char>*>(InImGuiContext->PlatformIO.Platform_ClipboardUserData);
    if (ClipboardBuffer)
    {
        FString ClipboardText;
        FPlatformApplicationMisc::ClipboardPaste(ClipboardText);

        ClipboardBuffer->SetNumUninitialized(FPlatformString::ConvertedLength<UTF8CHAR>(*ClipboardText));
        FPlatformString::Convert(reinterpret_cast<UTF8CHAR*>(ClipboardBuffer->GetData()), ClipboardBuffer->Num(), *ClipboardText, ClipboardText.Len() + 1);

        return ClipboardBuffer->GetData();
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_SetClipboardTextFn(ImGuiContext* InImGuiContext, const char* ClipboardText)
{
    FPlatformApplicationMisc::ClipboardCopy(UTF8_TO_TCHAR(ClipboardText));
}

//--------------------------------------------------------------------------------------------------------------------------
 bool FCogImguiContext::ImGui_OpenInShell(ImGuiContext* Context, const char* Path)
{
    return FPlatformProcess::LaunchFileInDefaultExternalApplication(UTF8_TO_TCHAR(Path));
}

//--------------------------------------------------------------------------------------------------------------------------
static APlayerController* GetLocalPlayerController(const UWorld* World)
{
    if (World == nullptr)
    { return nullptr; }

    for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* ItPlayerController = Iterator->Get();
        if (ItPlayerController->IsLocalController())
        { return ItPlayerController; }
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::SetEnableInput(const bool InValue)
{
    FCogImGuiContextScope ImGuiContextScope(Context, PlotContext);

    bEnableInput = InValue; 

    if (FSlateApplication::IsInitialized() == false)
    { return; }

    if (bEnableInput)
    {
        FSlateThrottleManager::Get().DisableThrottle(true);
        bIsThrottleDisabled = true;

        if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
        {
            LocalPlayer->GetSlateOperations()
                .ReleaseMouseLock()
                .ReleaseMouseCapture();
        }
    }
    else
    {
        if (bIsThrottleDisabled)
        {
            FSlateThrottleManager::Get().DisableThrottle(false);
            bIsThrottleDisabled = false;
        }
        
        if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
        {
            LocalPlayer->GetSlateOperations().CaptureMouse(GameViewport->GetGameViewportWidget().ToSharedRef());
        }

        //---------------------------------------------------------------------------------
        // Make sure no keys stay down after disabling inputs
        //---------------------------------------------------------------------------------
        ImGuiIO& IO = ImGui::GetIO();
        IO.ClearInputMouse();
        IO.ClearEventsQueue();
        IO.ClearInputKeys();
    }

    RefreshMouseCursor();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::SetShareMouse(bool Value)
{
    bShareMouse = Value;
    RefreshMouseCursor();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::SetShareMouseWithGameplay(bool Value)
{
    bShareMouseWithGameplay = Value;
    RefreshMouseCursor();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::RefreshMouseCursor()
{
    if (FSlateApplication::IsInitialized() == false)
    {
        return;
    }

    //-------------------------------------------------------------------------------------------
    // Focus the main widget when enabling input otherwise the mouse can still be hidden because
    // the gameplay might have the focus and might hide the cursor.
    //-------------------------------------------------------------------------------------------
    if (bEnableInput)
    {
        FSlateApplication::Get().SetKeyboardFocus(MainWidget);
        FSlateApplication::Get().SetUserFocus(0, MainWidget);
    }

    //-------------------------------------------------------------------------------------------
	// Force to show the cursor when sharing mouse with gameplay for games that hide the cursor
    //-------------------------------------------------------------------------------------------
    if (GameViewport != nullptr)
    {
        if (APlayerController* PlayerController = GetLocalPlayerController(GameViewport->GetWorld()))
        {
            if (bHasSavedInitialCursorVisibility == false)
            {
                bIsCursorInitiallyVisible = PlayerController->ShouldShowMouseCursor();
                bHasSavedInitialCursorVisibility = true;
            }

            if (bEnableInput && bShareMouse && bShareMouseWithGameplay)
            {
                PlayerController->SetShowMouseCursor(true);
            }
            else
            {
                PlayerController->SetShowMouseCursor(bIsCursorInitiallyVisible);
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::SetDPIScale(float Value)
{
    if (DpiScale == Value)
    {
        return;
    }

    DpiScale = Value;
    bRefreshDPIScale = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::SetFont(FString Value)
{
    Font = Value;
    bRefreshDPIScale = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::BuildFont()
{
    FCogImGuiContextScope ImGuiContextScope(Context, PlotContext);

    if (FontAtlasTexture != nullptr)
    {
        FontAtlasTexture->RemoveFromRoot();
        FontAtlasTexture->ConditionalBeginDestroy();
    }

    ImGuiIO& IO = ImGui::GetIO();
    IO.Fonts->Clear();

    ImFontConfig FontConfig;
    FontConfig.SizePixels = FMath::RoundFromZero(13.f * DpiScale);
	
    // if engine's font file exist, use it as truetype font, else use fallback one
    if (FPaths::FileExists(Font))
    {
        // support chinese if needed
#if COG_SUPPORT_CHINESE
        const ImWchar* glyphRange = IO.Fonts->GetGlyphRangesChineseFull();
#else
        const ImWchar* glyphRange = IO.Fonts->GetGlyphRangesDefault();
#endif
        IO.Fonts->AddFontFromFileTTF(TCHAR_TO_UTF8(*Font), 16.f * DpiScale, nullptr, glyphRange);
    }
    else
    {
        IO.Fonts->AddFontDefault(&FontConfig);
    }
	
    uint8* TextureDataRaw;
    int32 TextureWidth, TextureHeight, BytesPerPixel;
    IO.Fonts->GetTexDataAsRGBA32(&TextureDataRaw, &TextureWidth, &TextureHeight, &BytesPerPixel);

#if UE_VERSION_OLDER_THAN(5, 5, 0)
    const int32 PieSessionId = GPlayInEditorID;
#else
    const int32 PieSessionId = UE::GetPlayInEditorID();
#endif

    FString TextureName = FString::Format(TEXT("ImGuiFontAtlas{0}"), { PieSessionId });

    FontAtlasTexture = UTexture2D::CreateTransient(TextureWidth, TextureHeight, PF_R8G8B8A8, *TextureName);
    FontAtlasTexture->Filter = TF_Bilinear;
    FontAtlasTexture->AddressX = TA_Wrap;
    FontAtlasTexture->AddressY = TA_Wrap;

    FontAtlasTexture->UpdateResource();

    FUpdateTextureRegion2D* TextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, TextureWidth, TextureHeight);
    auto DataCleanup = [](uint8* Data, const FUpdateTextureRegion2D* UpdateRegion)
    {
        delete UpdateRegion;
    };
    FontAtlasTexture->UpdateTextureRegions(0, 1u, TextureRegion, BytesPerPixel * TextureWidth, BytesPerPixel, TextureDataRaw, DataCleanup);
    FlushRenderingCommands();

    IO.Fonts->SetTexID(FontAtlasTexture);
    FontAtlasTexturePtr.Reset(FontAtlasTexture);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiContext::IsConsoleOpened() const
{
    return GameViewport != nullptr 
        && GameViewport->ViewportConsole 
        && GameViewport->ViewportConsole->ConsoleState != NAME_None;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::DrawDebug() const
{
    if (ImGui::Begin("ImGui Integration Debug"))
    {
        ImGui::BeginDisabled();

        ImVec2 AbsPos = FCogImguiHelper::ToImVec2(MainWidget->GetTickSpaceGeometry().GetAbsolutePosition());
        ImGui::InputFloat2("Widget Abs Pos", &AbsPos.x, "%0.1f");

        ImVec2 AbsSize = FCogImguiHelper::ToImVec2(MainWidget->GetTickSpaceGeometry().GetAbsoluteSize());
        ImGui::InputFloat2("Widget Abs Size", &AbsSize.x, "%0.1f");

        ImVec2 LocalSize = FCogImguiHelper::ToImVec2(MainWidget->GetTickSpaceGeometry().GetLocalSize());
        ImGui::InputFloat2("Widget Local Size", &LocalSize.x, "%0.1f");

        ImVec2 MousePosition = FCogImguiHelper::ToImVec2(FSlateApplication::Get().GetCursorPos());
        ImGui::InputFloat2("Mouse", &MousePosition.x, "%0.1f");

        ImGuiIO& IO = ImGui::GetIO();
        ImGui::InputFloat2("ImGui Mouse", &IO.MousePos.x, "%0.1f");

        FString Focus = "None";
        if (TSharedPtr<SWidget> KeyboardFocusedWidget = FSlateApplication::Get().GetKeyboardFocusedWidget())
        {
            Focus = KeyboardFocusedWidget->ToString();
        }
        static char Buffer[256] = "";
        ImStrncpy(Buffer, COG_TCHAR_TO_CHAR(*Focus), IM_ARRAYSIZE(Buffer));
        ImGui::InputText("Keyboard Focus", Buffer, IM_ARRAYSIZE(Buffer));

        ImGui::EndDisabled();
    }
    ImGui::End();
}

//--------------------------------------------------------------------------------------------------------------------------
ULocalPlayer* FCogImguiContext::GetLocalPlayer() const
{
    if (GameViewport == nullptr)
    {
        return nullptr;
    }

    UWorld* World = GameViewport->GetWorld();
    if (World == nullptr)
    {
        return nullptr;
    }

    ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
    return LocalPlayer;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiContext::GetSkipRendering() const
{
    return bSkipRendering;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::SetSkipRendering(bool Value)
{
    bSkipRendering = Value;
}

