#include "CogImguiWidget.h"

#include "CogImguiInputHelper.h"
#include "CogImguiInputHelper.h"
#include "CogImguiModule.h"
#include "CogImguiModule.h"
#include "CogImguiTextureManager.h"
#include "CogImguiWidget.h"
#include "Engine/Console.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/PlayerController.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "SlateOptMacros.h"
#include "UnrealClient.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SViewport.h"

//--------------------------------------------------------------------------------------------------------------------------
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SCogImguiWidget::Construct(const FArguments& InArgs)
{
    checkf(InArgs._GameViewport, TEXT("Null Game Viewport argument"));

    GameViewport = InArgs._GameViewport;
    FontAtlas = InArgs._FontAtlas;
    Render = InArgs._Render;

    ImGuiContext = ImGui::CreateContext(FontAtlas);
    ImPlotContext = ImPlot::CreateContext();
    ImPlot::SetImGuiContext(ImGuiContext);

    const char* InitFilenameTemp = TCHAR_TO_ANSI(*FCogImguiHelper::GetIniFilePath("imgui"));
    ImStrncpy(IniFilename, InitFilenameTemp, IM_ARRAYSIZE(IniFilename));

    ImGuiIO& IO = ImGui::GetIO();
    IO.IniFilename = IniFilename;
    IO.DisplaySize = ImVec2(100, 100);
    IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

//--------------------------------------------------------------------------------------------------------------------------
SCogImguiWidget::~SCogImguiWidget()
{
    ImPlot::DestroyContext(ImPlotContext);
    ImGui::DestroyContext(ImGuiContext);
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    Super::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    TickKeyModifiers();
    TickFocus();
    TickImGui(InDeltaTime);
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::TickKeyModifiers()
{
    //-------------------------------------------------------------------------------------------------------
    // Refresh modifiers otherwise, when pressing ALT-TAB, the Alt modifier is always true
    //-------------------------------------------------------------------------------------------------------
    FModifierKeysState ModifierKeys = FSlateApplication::Get().GetModifierKeys();
    
    ImGuiIO& IO = ImGui::GetIO();
    if (ModifierKeys.IsControlDown() != IO.KeyCtrl)     { IO.AddKeyEvent(ImGuiMod_Ctrl, ModifierKeys.IsControlDown()); }
    if (ModifierKeys.IsShiftDown() != IO.KeyShift)      { IO.AddKeyEvent(ImGuiMod_Shift, ModifierKeys.IsShiftDown()); }
    if (ModifierKeys.IsAltDown() != IO.KeyAlt)          { IO.AddKeyEvent(ImGuiMod_Alt, ModifierKeys.IsAltDown()); }
    if (ModifierKeys.IsCommandDown() != IO.KeySuper)    { IO.AddKeyEvent(ImGuiMod_Super, ModifierKeys.IsCommandDown()); }
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::TickImGui(float InDeltaTime)
{
    if (ImGuiContext == nullptr)
    {
        return;
    }

    ImGui::SetCurrentContext(ImGuiContext);
    ImGuiIO& IO = ImGui::GetIO();
    IO.DeltaTime = InDeltaTime;

    ImPlot::SetImGuiContext(ImGuiContext);
    ImPlot::SetCurrentContext(ImPlotContext);

    FVector2D DisplaySize;
    GameViewport->GetViewportSize(DisplaySize);
    IO.DisplaySize = FCogImguiHelper::ToImVec2(DisplaySize);

    ImGui::NewFrame();
    Render(InDeltaTime);
    ImGui::Render();

    SetCursor(FCogImguiInputHelper::ToSlateMouseCursor(ImGui::GetMouseCursor()));

    ImDrawData* DrawData = ImGui::GetDrawData();
    if (DrawData && DrawData->CmdListsCount > 0)
    {
        DrawLists.SetNum(DrawData->CmdListsCount, false);
        for (int i = 0; i < DrawData->CmdListsCount; i++)
        {
            DrawLists[i].TransferDrawData(*DrawData->CmdLists[i]);
        }
    }
    else
    {
        DrawLists.Empty();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::TickFocus()
{
    FCogImguiModule& Module = FCogImguiModule::Get();

    if (UWorld* World = GameViewport->GetWorld())
    {
        if (APlayerController* Controller = FCogImguiInputHelper::GetFirstLocalPlayerController(*World))
        {
            if (FCogImguiInputHelper::WasKeyInfoJustPressed(*Controller, Module.GetToggleInputKey()))
            {
                Module.ToggleEnableInput();
            }
        }
    }

    const bool bShouldEnableInput = Module.GetEnableInput();
    if (bEnableInput != bShouldEnableInput)
    {
        bEnableInput = bShouldEnableInput;

        if (bEnableInput)
        {
            TakeFocus();
        }
        else
        {
            ReturnFocus();
        }
    }
    else if (bEnableInput)
    {
        const auto& ViewportWidget = GameViewport->GetGameViewportWidget();
        if (!HasKeyboardFocus() && !IsConsoleOpened() && (ViewportWidget->HasKeyboardFocus() || ViewportWidget->HasFocusedDescendants()))
        {
            TakeFocus();
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::TakeFocus()
{
    FSlateApplication& SlateApplication = FSlateApplication::Get();

    PreviousUserFocusedWidget = SlateApplication.GetUserFocusedWidget(SlateApplication.GetUserIndexForKeyboard());

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        TSharedRef<SWidget> FocusWidget = SharedThis(this);
        LocalPlayer->GetSlateOperations().CaptureMouse(FocusWidget);
        LocalPlayer->GetSlateOperations().SetUserFocus(FocusWidget);
    }
    else
    {
        SlateApplication.SetKeyboardFocus(SharedThis(this));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::ReturnFocus()
{
    if (HasKeyboardFocus())
    {
        auto FocusWidgetPtr = PreviousUserFocusedWidget.IsValid()
            ? PreviousUserFocusedWidget.Pin()
            : GameViewport->GetGameViewportWidget();

        if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
        {
            auto FocusWidgetRef = FocusWidgetPtr.ToSharedRef();

            if (FocusWidgetPtr == GameViewport->GetGameViewportWidget())
            {
                LocalPlayer->GetSlateOperations().CaptureMouse(FocusWidgetRef);
            }

            LocalPlayer->GetSlateOperations().SetUserFocus(FocusWidgetRef);
        }
        else
        {
            FSlateApplication& SlateApplication = FSlateApplication::Get();
            SlateApplication.ResetToDefaultPointerInputSettings();
            SlateApplication.SetUserFocus(SlateApplication.GetUserIndexForKeyboard(), FocusWidgetPtr);
        }
    }

    PreviousUserFocusedWidget.Reset();
}


//--------------------------------------------------------------------------------------------------------------------------
int32 SCogImguiWidget::OnPaint(
    const FPaintArgs& Args, 
    const FGeometry& AllottedGeometry, 
    const FSlateRect& MyClippingRect,
    FSlateWindowElementList& OutDrawElements, 
    int32 LayerId, 
    const FWidgetStyle& WidgetStyle, 
    bool bParentEnabled) const
{

    const FSlateRenderTransform& WidgetToScreen = AllottedGeometry.GetAccumulatedRenderTransform();
    const FSlateRenderTransform ImGuiToScreen = FCogImguiHelper::RoundTranslation(ImGuiRenderTransform.Concatenate(WidgetToScreen));

    FCogImguiTextureManager& TextureManager = FCogImguiModule::Get().GetTextureManager();

    for (const auto& DrawList : DrawLists)
    {
        DrawList.CopyVertexData(VertexBuffer, ImGuiToScreen);

        int IndexBufferOffset = 0;
        for (int i = 0; i < DrawList.NumCommands(); i++)
        {
            const auto& DrawCommand = DrawList.GetCommand(i, ImGuiToScreen);

            DrawList.CopyIndexData(IndexBuffer, IndexBufferOffset, DrawCommand.NumElements);

            // Advance offset by number of copied elements to position it for the next command.
            IndexBufferOffset += DrawCommand.NumElements;

            // Get texture resource handle for this draw command (null index will be also mapped to a valid texture).
            const FSlateResourceHandle& Handle = TextureManager.GetTextureHandle(DrawCommand.TextureId);

            // Transform clipping rectangle to screen space and apply to elements that we draw.
            const FSlateRect ClippingRect = DrawCommand.ClippingRect.IntersectionWith(MyClippingRect);

            OutDrawElements.PushClip(FSlateClippingZone{ ClippingRect });

            // Add elements to the list.
            FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, Handle, VertexBuffer, IndexBuffer, nullptr, 0, 0);

            OutDrawElements.PopClip();
        }
    }

    return Super::OnPaint(Args, AllottedGeometry, MyClippingRect, OutDrawElements, LayerId, WidgetStyle, bParentEnabled);
}

//--------------------------------------------------------------------------------------------------------------------------
FVector2D SCogImguiWidget::ComputeDesiredSize(float Scale) const
{
    return Super::ComputeDesiredSize(Scale);
}

//--------------------------------------------------------------------------------------------------------------------------
ULocalPlayer* SCogImguiWidget::GetLocalPlayer() const
{
    if (GameViewport.IsValid())
    {
        if (UWorld* World = GameViewport->GetWorld())
        {
            if (ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController())
            {
                return World->GetFirstLocalPlayerFromController();
            }
        }
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
FVector2D SCogImguiWidget::TransformScreenPointToImGui(const FGeometry& MyGeometry, const FVector2D& Point) const
{
    const FSlateRenderTransform ImGuiToScreen = MyGeometry.GetAccumulatedRenderTransform();
    return ImGuiToScreen.Inverse().TransformPoint(Point);
}


//--------------------------------------------------------------------------------------------------------------------------
bool SCogImguiWidget::IsConsoleOpened() const
{
    return GameViewport->ViewportConsole && GameViewport->ViewportConsole->ConsoleState != NAME_None;
}

//--------------------------------------------------------------------------------------------------------------------------
bool SCogImguiWidget::IsCurrentContext() const
{
    return ImGui::GetCurrentContext() == ImGuiContext;
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::SetAsCurrentContext()
{
    ImGui::SetCurrentContext(ImGuiContext);
}


//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::SetDPIScale(float Scale)
{
    if (DpiScale == Scale)
    {
        return;
    }

    DpiScale = Scale;
    OnDpiChanged();
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::OnDpiChanged()
{
    if (FontAtlas == nullptr)
    {
        return;
    }

    FontAtlas->Clear();

    ImFontConfig FontConfig = {};
    FontConfig.SizePixels = FMath::RoundFromZero(13.f * DpiScale);
    FontAtlas->AddFontDefault(&FontConfig);

    unsigned char* Pixels;
    int Width, Height, Bpp;
    FontAtlas->GetTexDataAsRGBA32(&Pixels, &Width, &Height, &Bpp);
    const CogTextureIndex FontsTexureIndex = FCogImguiModule::Get().GetTextureManager().CreateTexture("xx", Width, Height, Bpp, Pixels);
    FontAtlas->TexID = FCogImguiHelper::ToImTextureID(FontsTexureIndex);

    ImGuiStyle NewStyle = ImGuiStyle();
    ImGui::GetStyle() = MoveTemp(NewStyle);
    NewStyle.ScaleAllSizes(DpiScale);
}


//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent)
{
    if (FCogImguiModule::Get().GetEnableInput())
    {
        ImGui::GetIO().AddInputCharacter(FCogImguiInputHelper::CastInputChar(CharacterEvent.GetCharacter()));
        return FReply::Handled();
    }

    return FReply::Unhandled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
    if (FCogImguiInputHelper::IsKeyEventHandled(KeyEvent) == false)
    {
        return FReply::Unhandled();
    }

    ImGuiIO& IO = ImGui::GetIO();
    IO.AddKeyEvent(FCogImguiInputHelper::KeyEventToImGuiKey(KeyEvent), true);
    IO.AddKeyEvent(ImGuiMod_Ctrl, KeyEvent.IsControlDown());
    IO.AddKeyEvent(ImGuiMod_Shift, KeyEvent.IsShiftDown());
    IO.AddKeyEvent(ImGuiMod_Alt, KeyEvent.IsAltDown());
    IO.AddKeyEvent(ImGuiMod_Super, KeyEvent.IsCommandDown());

    return FReply::Handled();

}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
    if (FCogImguiInputHelper::IsKeyEventHandled(KeyEvent) == false)
    {
        return FReply::Unhandled();
    }

    ImGuiIO& IO = ImGui::GetIO();
    IO.AddKeyEvent(FCogImguiInputHelper::KeyEventToImGuiKey(KeyEvent), false);
    IO.AddKeyEvent(ImGuiMod_Ctrl, KeyEvent.IsControlDown());
    IO.AddKeyEvent(ImGuiMod_Shift, KeyEvent.IsShiftDown());
    IO.AddKeyEvent(ImGuiMod_Alt, KeyEvent.IsAltDown());
    IO.AddKeyEvent(ImGuiMod_Super, KeyEvent.IsCommandDown());

    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& AnalogInputEvent)
{
    return FReply::Unhandled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (FCogImguiModule::Get().GetEnableInput() == false)
    {
        return FReply::Unhandled();
    }

    ImGui::GetIO().AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    ImGui::GetIO().AddMouseButtonEvent(FCogImguiInputHelper::MouseButtonToImGuiMouseButton(MouseEvent.GetEffectingButton()), true);
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (FCogImguiModule::Get().GetEnableInput() == false)
    {
        return FReply::Unhandled();
    }

    ImGui::GetIO().AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    ImGui::GetIO().AddMouseButtonEvent(FCogImguiInputHelper::MouseButtonToImGuiMouseButton(MouseEvent.GetEffectingButton()), false);
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (FCogImguiModule::Get().GetEnableInput() == false)
    {
        return FReply::Unhandled();
    }

    ImGui::GetIO().AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    ImGui::GetIO().AddMouseWheelEvent(0, MouseEvent.GetWheelDelta());
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (FCogImguiModule::Get().GetEnableInput() == false)
    {
        return FReply::Unhandled();
    }

    ImGui::GetIO().AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    const FVector2D Pos = TransformScreenPointToImGui(MyGeometry, MouseEvent.GetScreenSpacePosition());
    ImGui::GetIO().AddMousePosEvent(Pos.X, Pos.Y);
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& FocusEvent)
{
    Super::OnFocusReceived(MyGeometry, FocusEvent);

    FSlateApplication::Get().ResetToDefaultPointerInputSettings();

    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::OnFocusLost(const FFocusEvent& FocusEvent)
{
    return Super::OnFocusLost(FocusEvent);
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    Super::OnMouseEnter(MyGeometry, MouseEvent);
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::OnMouseLeave(const FPointerEvent& MouseEvent)
{
    Super::OnMouseLeave(MouseEvent);
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent)
{
    return Super::OnTouchStarted(MyGeometry, TouchEvent);
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent)
{
    return Super::OnTouchMoved(MyGeometry, TouchEvent);
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent)
{
    return Super::OnTouchEnded(MyGeometry, TouchEvent);
}