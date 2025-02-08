#include "CogImguiInputCatcherWidget.h"

#include "CogImguiContext.h"
#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "Engine/GameViewportClient.h"
#include "imgui.h"
#include "SlateOptMacros.h"
#include "Widgets/SWindow.h"

//--------------------------------------------------------------------------------------------------------------------------
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SCogImguiInputCatcherWidget::Construct(const FArguments& InArgs)
{
    Context = InArgs._Context;

    RefreshVisibility();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiInputCatcherWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    Super::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    RefreshVisibility();
}

//--------------------------------------------------------------------------------------------------------------------------
int32 SCogImguiInputCatcherWidget::OnPaint(
    const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyClippingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& WidgetStyle,
    bool bParentEnabled) const
{
    return LayerId;
}

//--------------------------------------------------------------------------------------------------------------------------
FVector2D SCogImguiInputCatcherWidget::ComputeDesiredSize(float Scale) const
{
    return FVector2D::ZeroVector;
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiInputCatcherWidget::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent)
{
    return FReply::Unhandled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiInputCatcherWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
    return FReply::Unhandled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiInputCatcherWidget::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
    return FReply::Unhandled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiInputCatcherWidget::OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& AnalogInputEvent)
{
    return FReply::Unhandled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiInputCatcherWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return HandleMouseButtonEvent(MouseEvent, true);
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiInputCatcherWidget::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return HandleMouseButtonEvent(MouseEvent, true);
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiInputCatcherWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return HandleMouseButtonEvent(MouseEvent, false);
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiInputCatcherWidget::HandleMouseButtonEvent(const FPointerEvent& MouseEvent, bool Down) const
{
    FCogImGuiContextScope ImGuiContextScope(*Context);

    if (Context->GetEnableInput() == false)
    {
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("SCogImguiInputCatcherWidget::HandleMouseButtonEvent | Window:%s | Unhandled | EnableInput == false | Down:%d"), Window.IsValid() ? *Window->GetTitle().ToString() : *FString("None"), Down);
        return FReply::Unhandled();
    }
    
    const uint32 MouseButton = FCogImguiInputHelper::ToImGuiMouseButton(MouseEvent.GetEffectingButton());
    ImGui::GetIO().AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    ImGui::GetIO().AddMouseButtonEvent(MouseButton, Down);

    UE_LOG(LogCogImGui, VeryVerbose, TEXT("SCogImguiInputCatcherWidget::HandleMouseButtonEvent | Window:%s | Handled | Down:%d"), Window.IsValid() ? *Window->GetTitle().ToString() : *FString("None"), Down);
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiInputCatcherWidget::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    FCogImGuiContextScope ImGuiContextScope(*Context);

    if (Context->GetEnableInput() == false)
    {
        return FReply::Unhandled();
    }
    ImGui::GetIO().AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    ImGui::GetIO().AddMouseWheelEvent(0, MouseEvent.GetWheelDelta());
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiInputCatcherWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return FReply::Unhandled();
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiInputCatcherWidget::RefreshVisibility()
{
    EVisibility DesiredVisibility;

    if (Context->GetEnableInput() && Context->GetShareMouseWithGameplay() == false)
    {
        DesiredVisibility = EVisibility::Visible;
    }
    else
    {
        DesiredVisibility = EVisibility::SelfHitTestInvisible;
    }

    if (DesiredVisibility != GetVisibility())
    {
        SetVisibility(DesiredVisibility);
    }
}
