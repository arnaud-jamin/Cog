#include "CogImguiWidget.h"

#include "CogImguiContext.h"
#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "Engine/GameViewportClient.h"
#include "imgui.h"
#include "SlateOptMacros.h"
#include "Widgets/SWindow.h"

//--------------------------------------------------------------------------------------------------------------------------
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SCogImguiWidget::Construct(const FArguments& InArgs)
{
    Context = InArgs._Context;

    RefreshVisibility();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::SetDrawData(const ImDrawData* InDrawData)
{
    DrawData = FCogImguiDrawData(InDrawData);
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    Super::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    RefreshVisibility();
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
    if (Context == nullptr || Context->GetSkipRendering())
    {
        return LayerId;
    }

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

            UTexture2D* Texture = Cast<UTexture2D>(DrawCmd.GetTexID());
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

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent)
{
    FCogImGuiContextScope ImGuiContextScope(*Context);

    if (Context->GetEnableInput() == false)
    {
        return FReply::Unhandled();
    }
    
    ImGuiIO& IO = ImGui::GetIO();
    IO.AddInputCharacter(FCogImguiInputHelper::CastInputChar(CharacterEvent.GetCharacter()));

    const FReply Result = IO.WantCaptureKeyboard ? FReply::Handled() : FReply::Unhandled();
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
    return HandleKeyEvent(KeyEvent, true);
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
    return HandleKeyEvent(KeyEvent, false);
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::HandleKeyEvent(const FKeyEvent& KeyEvent, bool Down) const
{
    FCogImGuiContextScope ImGuiContextScope(*Context);

    if (Context->GetEnableInput() == false)
    {
        return FReply::Unhandled();
    }

    if (KeyEvent.GetKey().IsGamepadKey())
    {
        return FReply::Unhandled();
    }

    if (const UWorld* World = Context->GetGameViewport()->GetWorld())
    {
        if (const UPlayerInput* PlayerInput = FCogImguiInputHelper::GetPlayerInput(*World))
        {
            if (FCogImguiInputHelper::IsTopPriorityKeyEvent(*PlayerInput, KeyEvent))
            {
                return FReply::Unhandled();
            }
        }
    }

    ImGuiIO& IO = ImGui::GetIO();
    IO.AddKeyEvent(FCogImguiInputHelper::ToImKey(KeyEvent.GetKey()), Down);
    IO.AddKeyEvent(ImGuiMod_Ctrl, KeyEvent.IsControlDown());
    IO.AddKeyEvent(ImGuiMod_Shift, KeyEvent.IsShiftDown());
    IO.AddKeyEvent(ImGuiMod_Alt, KeyEvent.IsAltDown());
    IO.AddKeyEvent(ImGuiMod_Super, KeyEvent.IsCommandDown());

    if (IO.WantCaptureKeyboard == false && Context->GetShareKeyboard())
    {
        return FReply::Unhandled();
    }

    // if (IO.WantTextInput == false)
    // {
    //     return FReply::Unhandled();
    // }

    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& AnalogInputEvent)
{
    if (AnalogInputEvent.GetKey().IsGamepadKey())
    {
        return FReply::Unhandled();
    }

    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return HandleMouseButtonEvent(MouseEvent, true);
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return HandleMouseButtonEvent(MouseEvent, true);
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return HandleMouseButtonEvent(MouseEvent, false);
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::HandleMouseButtonEvent(const FPointerEvent& MouseEvent, bool Down) const
{
    FCogImGuiContextScope ImGuiContextScope(*Context);

    if (Context->GetEnableInput() == false)
    {
        UE_LOG(LogCogImGui, VeryVerbose, TEXT("SCogImguiWidget::HandleMouseButtonEvent | %s | Unhandled | EnableInput == false | Down:%d"), Window.IsValid() ? *Window->GetTitle().ToString() : *FString("None"), Down);
        return FReply::Unhandled();
    }
    const uint32 MouseButton = FCogImguiInputHelper::ToImGuiMouseButton(MouseEvent.GetEffectingButton());
    ImGui::GetIO().AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    ImGui::GetIO().AddMouseButtonEvent(MouseButton, Down);

    UE_LOG(LogCogImGui, VeryVerbose, TEXT("SCogImguiWidget::HandleMouseButtonEvent | Window:%s | Handled | Down:%d"), Window.IsValid() ? *Window->GetTitle().ToString() : *FString("None"), Down);
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------------------------------------------
FReply SCogImguiWidget::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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
FReply SCogImguiWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    FCogImGuiContextScope ImGuiContextScope(*Context);

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
void SCogImguiWidget::OnFocusLost(const FFocusEvent& InFocusEvent)
{
    SLeafWidget::OnFocusLost(InFocusEvent);

    if (Context != nullptr)
    {
        Context->OnImGuiWidgetFocusLost();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogImguiWidget::RefreshVisibility()
{
    EVisibility DesiredVisibility;

    if (Context->GetEnableInput())
    {
        if (Context->GetShareMouse() && Context->GetWantCaptureMouse() == false)
        {
            DesiredVisibility = EVisibility::SelfHitTestInvisible;
        }
        else
        {
            DesiredVisibility = EVisibility::Visible;
        }
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
