#include "CogImguiWidget.h"

#include "CogImguiInputHelper.h"
#include "CogImguiModule.h"
#include "CogImguiWidget.h"

#include "CogImGuiContext.h"
#include "imgui.h"
#include "SlateOptMacros.h"

//--------------------------------------------------------------------------------------------------------------------------
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SCogImguiWidget::Construct(const FArguments& InArgs)
{
    Context = InArgs._Context;

    //SetVisibility(EVisibility::SelfHitTestInvisible);
    SetVisibility(EVisibility::Visible);
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

//--------------------------------------------------------------------------------------------------------------------------
SCogImguiWidget::~SCogImguiWidget()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::SetDrawData(const ImDrawData* InDrawData)
{
    DrawData = FCogImguiDrawData(InDrawData);
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

    const FSlateRenderTransform Transform(FCogImguiHelper::RoundTranslation(AllottedGeometry.GetAccumulatedRenderTransform().GetTranslation() - FVector2d(DrawData.DisplayPos)));

    FSlateBrush TextureBrush;
    for (const FCogImguiDrawList& DrawList : DrawData.DrawLists)
    {
        TArray<FSlateVertex> Vertices;
        Vertices.SetNumUninitialized(DrawList.VtxBuffer.Size);
        for (int32 BufferIdx = 0; BufferIdx < Vertices.Num(); ++BufferIdx)
        {
            const ImDrawVert& Vtx = DrawList.VtxBuffer.Data[BufferIdx];
            Vertices[BufferIdx] = FSlateVertex::Make<ESlateVertexRounding::Disabled>(Transform, FCogImguiHelper::ToFVector2f(Vtx.pos), FCogImguiHelper::ToFVector2f(Vtx.uv), FVector2f::UnitVector, FCogImguiHelper::ToFColor(Vtx.col));
        }

        TArray<SlateIndex> Indices;
        Indices.SetNumUninitialized(DrawList.IdxBuffer.Size);
        for (int32 BufferIdx = 0; BufferIdx < Indices.Num(); ++BufferIdx)
        {
            Indices[BufferIdx] = DrawList.IdxBuffer.Data[BufferIdx];
        }

        for (const ImDrawCmd& DrawCmd : DrawList.CmdBuffer)
        {
            TArray VerticesSlice(Vertices.GetData() + DrawCmd.VtxOffset, Vertices.Num() - DrawCmd.VtxOffset);
            TArray IndicesSlice(Indices.GetData() + DrawCmd.IdxOffset, DrawCmd.ElemCount);

            UTexture2D* Texture = DrawCmd.GetTexID();
            if (TextureBrush.GetResourceObject() != Texture)
            {
                TextureBrush.SetResourceObject(Texture);
                if (IsValid(Texture))
                {
                    TextureBrush.ImageSize.X = Texture->GetSizeX();
                    TextureBrush.ImageSize.Y = Texture->GetSizeY();
                    TextureBrush.ImageType = ESlateBrushImageType::FullColor;
                    TextureBrush.DrawAs = ESlateBrushDrawType::Image;
                }
                else
                {
                    TextureBrush.ImageSize.X = 0;
                    TextureBrush.ImageSize.Y = 0;
                    TextureBrush.ImageType = ESlateBrushImageType::NoImage;
                    TextureBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
                }
            }

            FSlateRect ClipRect(DrawCmd.ClipRect.x, DrawCmd.ClipRect.y, DrawCmd.ClipRect.z, DrawCmd.ClipRect.w);
            ClipRect = TransformRect(Transform, ClipRect);

            OutDrawElements.PushClip(FSlateClippingZone(ClipRect));
            FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, TextureBrush.GetRenderingResource(), VerticesSlice, IndicesSlice, nullptr, 0, 0);
            OutDrawElements.PopClip();
        }
    }

    return LayerId;
}

//--------------------------------------------------------------------------------------------------------------------------
FVector2D SCogImguiWidget::ComputeDesiredSize(float Scale) const
{
    return FVector2D::ZeroVector;
}

////--------------------------------------------------------------------------------------------------------------------------
//FReply SCogImguiWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& FocusEvent)
//{
//    if (bEnableInput == false)
//    {
//        return FReply::Unhandled();
//    }
//
//    Super::OnFocusReceived(MyGeometry, FocusEvent);
//
//    FSlateApplication::Get().ResetToDefaultPointerInputSettings();
//
//    return FReply::Handled();
//}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent)
{
    ImGuiIO& IO = ImGui::GetIO();
    IO.AddInputCharacter(FCogImguiInputHelper::CastInputChar(CharacterEvent.GetCharacter()));

    const FReply Result = IO.WantCaptureKeyboard ? FReply::Handled() : FReply::Unhandled();
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
    return HandleKeyEvent(MyGeometry, KeyEvent);
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
    return HandleKeyEvent(MyGeometry, KeyEvent);
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::HandleKeyEvent(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
    if (Context->GetEnableInput() == false)
    {
        return FReply::Unhandled();
    }
    if (KeyEvent.GetKey().IsGamepadKey())
    {
        //if (bShareGamepad)
        //{
        //    // TODO: handle imgui gamepad
        //    return FReply::Unhandled();
        //}
    }
    else
    {
        if (FCogImguiInputHelper::IsKeyEventHandled(Context->GetGameViewport()->GetWorld(), KeyEvent) == false)
        {
            return FReply::Unhandled();
        }
        ImGuiIO& IO = ImGui::GetIO();
        IO.AddKeyEvent(FCogImguiInputHelper::ToImKey(KeyEvent.GetKey()), false);
        IO.AddKeyEvent(ImGuiMod_Ctrl, KeyEvent.IsControlDown());
        IO.AddKeyEvent(ImGuiMod_Shift, KeyEvent.IsShiftDown());
        IO.AddKeyEvent(ImGuiMod_Alt, KeyEvent.IsAltDown());
        IO.AddKeyEvent(ImGuiMod_Super, KeyEvent.IsCommandDown());
        //if (bShareKeyboard)
        //{
        //    return FReply::Unhandled();
        //}
    }
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& AnalogInputEvent)
{
    if (AnalogInputEvent.GetKey().IsGamepadKey())
    {
        //if (bShareGamepad)
        //{
        //    // TODO: handle imgui gamepad
        //    return FReply::Unhandled();
        //}

    	return FReply::Unhandled();
    }
    else
    {
        //if (bShareKeyboard)
        //{
        //    return FReply::Unhandled();
        //}
    }
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (Context->GetEnableInput() == false)
    {
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("SCogImguiWidget::OnMouseButtonDown | %s | Unhandled | EnableInput == false"), Window.IsValid() ? *Window->GetTitle().ToString() : *FString("None"));
        return FReply::Unhandled();
    }
    const uint32 MouseButton = FCogImguiInputHelper::ToImGuiMouseButton(MouseEvent.GetEffectingButton());
    ImGui::GetIO().AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    ImGui::GetIO().AddMouseButtonEvent(MouseButton, true);

    UE_LOG(LogCogImGui, VeryVerbose, TEXT("SCogImguiWidget::OnMouseButtonDown | Window:%s | Handled"), Window.IsValid() ? *Window->GetTitle().ToString() : *FString("None"));
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (Context->GetEnableInput() == false)
    {
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("SCogImguiWidget::OnMouseButtonUp | Window:%s | Unhandled | EnableInput == false"), Window.IsValid() ? *Window->GetTitle().ToString() : *FString("None"));
        return FReply::Unhandled();
    }
    const uint32 MouseButton = FCogImguiInputHelper::ToImGuiMouseButton(MouseEvent.GetEffectingButton());
    ImGui::GetIO().AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    ImGui::GetIO().AddMouseButtonEvent(MouseButton, false);
    UE_LOG(LogCogImGui, VeryVerbose, TEXT("SCogImguiWidget::OnMouseButtonUp | Window:%s | Handled"), Window.IsValid() ? *Window->GetTitle().ToString() : *FString("None"));
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (Context->GetEnableInput() == false)
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
    if (Context->GetEnableInput() == false)
    {
        //UE_LOG(LogCogImGui, VeryVerbose, TEXT("SCogImguiWidget::OnMouseMove | Window:%s | Unhandled | EnableInput == false"), Window.IsValid() ? *Window->GetTitle().ToString() : *FString("None"));
        return FReply::Unhandled();
    }

    ImGuiIO& IO = ImGui::GetIO();
    if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        IO.AddMousePosEvent(MouseEvent.GetScreenSpacePosition().X, MouseEvent.GetScreenSpacePosition().Y);
    }
    else
    {
        const FVector2D TransformedMousePosition = MouseEvent.GetScreenSpacePosition() - Context->GetMainWidget()->GetTickSpaceGeometry().GetAbsolutePosition();
        IO.AddMousePosEvent(TransformedMousePosition.X, TransformedMousePosition.Y);
    }

    //UE_LOG(LogCogImGui, VeryVerbose, TEXT("SCogImguiWidget::OnMouseMove | Window:%s | Handled"), Window.IsValid() ? *Window->GetTitle().ToString() : *FString("None"));
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& FocusEvent)
{
    return Super::OnFocusReceived(MyGeometry, FocusEvent);
}


//--------------------------------------------------------------------------------------------------------------------------
//void SCogImguiWidget::TickFocus()
//{
//    FCogImguiModule& Module = FCogImguiModule::Get();
//
//    const bool bShouldEnableInput = Module.GetEnableInput();
//    if (bEnableInput != bShouldEnableInput)
//    {
//        bEnableInput = bShouldEnableInput;
//
//        if (bEnableInput)
//        {
//            TakeFocus();
//        }
//        else
//        {
//            ReturnFocus();
//        }
//    }
//    else if (bEnableInput)
//    {
//        const auto& ViewportWidget = GameViewport->GetGameViewportWidget();
//        if (!HasKeyboardFocus() && !IsConsoleOpened() && (ViewportWidget->HasKeyboardFocus() || ViewportWidget->HasFocusedDescendants()))
//        {
//            TakeFocus();
//        }
//    }
//}
//
//
////--------------------------------------------------------------------------------------------------------------------------
//void SCogImguiWidget::TakeFocus()
//{
//    FSlateApplication& SlateApplication = FSlateApplication::Get();
//
//    PreviousUserFocusedWidget = SlateApplication.GetUserFocusedWidget(SlateApplication.GetUserIndexForKeyboard());
//
//    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
//    {
//        TSharedRef<SWidget> FocusWidget = SharedThis(this);
//        LocalPlayer->GetSlateOperations().CaptureMouse(FocusWidget);
//        LocalPlayer->GetSlateOperations().SetUserFocus(FocusWidget);
//    }
//    else
//    {
//        SlateApplication.SetKeyboardFocus(SharedThis(this));
//    }
//}
//
////--------------------------------------------------------------------------------------------------------------------------
//void SCogImguiWidget::ReturnFocus()
//{
//    if (HasKeyboardFocus())
//    {
//        auto FocusWidgetPtr = PreviousUserFocusedWidget.IsValid()
//            ? PreviousUserFocusedWidget.Pin()
//            : GameViewport->GetGameViewportWidget();
//
//        if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
//        {
//            auto FocusWidgetRef = FocusWidgetPtr.ToSharedRef();
//
//            if (FocusWidgetPtr == GameViewport->GetGameViewportWidget())
//            {
//                LocalPlayer->GetSlateOperations().CaptureMouse(FocusWidgetRef);
//            }
//
//            LocalPlayer->GetSlateOperations().SetUserFocus(FocusWidgetRef);
//        }
//        else
//        {
//            FSlateApplication& SlateApplication = FSlateApplication::Get();
//            SlateApplication.ResetToDefaultPointerInputSettings();
//            SlateApplication.SetUserFocus(SlateApplication.GetUserIndexForKeyboard(), FocusWidgetPtr);
//        }
//    }
//
//    PreviousUserFocusedWidget.Reset();
//}
