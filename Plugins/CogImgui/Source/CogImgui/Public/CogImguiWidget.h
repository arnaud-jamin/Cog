#pragma once

#include "CoreMinimal.h"
#include "CogImguiDrawList.h"
#include "Rendering/RenderingCommon.h"
#include "UObject/WeakObjectPtr.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class UGameViewportClient;
class ULocalPlayer;
struct ImFontAtlas;
struct ImGuiContext;
struct ImPlotContext;

using FCogImguiRender = TFunction<void(float DeltaTime)>;

//--------------------------------------------------------------------------------------------------------------------------
class COGIMGUI_API SCogImguiWidget : public SCompoundWidget
{
    typedef SCompoundWidget Super;

public:

    SLATE_BEGIN_ARGS(SCogImguiWidget) {}
    SLATE_ARGUMENT(UGameViewportClient*, GameViewport)
    SLATE_ARGUMENT(ImFontAtlas*, FontAtlas)
    SLATE_ARGUMENT(FCogImguiRender, Render)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    ~SCogImguiWidget();

    //----------------------------------------------------------------------------------------------------
    // SWidget overrides
    //----------------------------------------------------------------------------------------------------
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
    virtual bool SupportsKeyboardFocus() const override { return true; }
    virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent) override;
    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;
    virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;
    virtual FReply OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& AnalogInputEvent) override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& FocusEvent) override;
    virtual void OnFocusLost(const FFocusEvent& FocusEvent) override;
    virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
    virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;
    virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;
    virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& WidgetStyle, bool bParentEnabled) const override;
    virtual FVector2D ComputeDesiredSize(float Scale) const override;

    ULocalPlayer* SCogImguiWidget::GetLocalPlayer() const;
    
    float GetDpiScale() const { return DpiScale; }

    void SetDPIScale(float Scale);

    bool IsCurrentContext() const;

    void SetAsCurrentContext();

protected:

    FVector2D TransformScreenPointToImGui(const FGeometry& MyGeometry, const FVector2D& Point) const;
    
    virtual void TickKeyModifiers();

    virtual void TickImGui(float InDeltaTime);
    
    virtual void TickFocus();
    
    virtual void TakeFocus();
    
    virtual void ReturnFocus();
    
    virtual void OnDpiChanged();

    bool IsConsoleOpened() const;

    TWeakObjectPtr<UGameViewportClient> GameViewport;

    ImFontAtlas* FontAtlas;

    TWeakPtr<SWidget> PreviousUserFocusedWidget;

    bool bEnableInput = false;

    FSlateRenderTransform ImGuiRenderTransform;

    mutable TArray<FSlateVertex> VertexBuffer;

    mutable TArray<SlateIndex> IndexBuffer;

    TArray<FCogImguiDrawList> DrawLists;

    ImGuiContext* ImGuiContext = nullptr;

    ImPlotContext* ImPlotContext = nullptr;

    FCogImguiRender Render;

    float DpiScale = 1.f;

    char IniFilename[512];
};
