#pragma once

#include "CoreMinimal.h"
#include "CogImguiDrawList.h"
#include "Rendering/RenderingCommon.h"
#include "UObject/WeakObjectPtr.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"

class FCogImguiContext;
class SWindow;
class UGameViewportClient;

//--------------------------------------------------------------------------------------------------------------------------
class COGIMGUI_API SCogImguiWidget : public SLeafWidget
{
    typedef SLeafWidget Super;

public:

    SLATE_BEGIN_ARGS(SCogImguiWidget) {}
        SLATE_ARGUMENT(FCogImguiContext*, Context)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& WidgetStyle, bool bParentEnabled) const override;

    virtual bool SupportsKeyboardFocus() const override { return true; }

    virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent) override;

    virtual FVector2D ComputeDesiredSize(float Scale) const override;

    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;

    virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;

    virtual FReply OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& AnalogInputEvent) override;

    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    virtual FReply OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& FocusEvent) override;
    
    virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;

    void SetDrawData(const ImDrawData* InDrawData);

    TSharedPtr<const SWindow> GetWindow() const { return Window; }

    void SetWindow(const TSharedPtr<SWindow>& Value) { Window = Value; }
    
protected:

    FReply HandleKeyEvent(const FKeyEvent& KeyEvent, bool Down) const;

    FReply HandleMouseButtonEvent(const FPointerEvent& MouseEvent, bool Down) const;

    void RefreshVisibility();

    FCogImguiContext* Context = nullptr;

    TSharedPtr<SWindow> Window = nullptr;

    FCogImguiDrawData DrawData;
};
