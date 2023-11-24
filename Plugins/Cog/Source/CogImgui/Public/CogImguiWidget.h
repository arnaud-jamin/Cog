#pragma once

#include "CoreMinimal.h"
#include "CogImguiDrawList.h"
#include "Rendering/RenderingCommon.h"
#include "UObject/WeakObjectPtr.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"

class IInputProcessor;
class UGameViewportClient;
class ULocalPlayer;
struct ImFontAtlas;
struct ImGuiContext;
struct ImPlotContext;

using FCogImguiRenderFunction = TFunction<void(float DeltaTime)>;

//--------------------------------------------------------------------------------------------------------------------------
class COGIMGUI_API SCogImguiWidget : public SLeafWidget
{
    typedef SLeafWidget Super;

public:

    SLATE_BEGIN_ARGS(SCogImguiWidget) {}
    SLATE_ARGUMENT(UGameViewportClient*, GameViewport)
    SLATE_ARGUMENT(ImFontAtlas*, FontAtlas)
    SLATE_ARGUMENT(FCogImguiRenderFunction, Render)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    ~SCogImguiWidget();

    //----------------------------------------------------------------------------------------------------
    // SWidget overrides
    //----------------------------------------------------------------------------------------------------
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& WidgetStyle, bool bParentEnabled) const override;

    virtual bool SupportsKeyboardFocus() const override { return true; }

    virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent) override;

    virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& FocusEvent) override;
    
    virtual FVector2D ComputeDesiredSize(float Scale) const override;

    ULocalPlayer* GetLocalPlayer() const;
    
    bool GetEnableInput() const { return bEnableInput; }

    void SetEnableInput(bool Value);

    bool GetShareMouse() const { return bShareMouse; }

    void SetShareMouse(bool Value) { bShareMouse = Value; }

    float GetDpiScale() const { return DpiScale; }

    void SetDPIScale(float Value);

    bool IsCurrentContext() const;

    void SetAsCurrentContext();

    TWeakObjectPtr<UGameViewportClient> GetGameViewport() const { return GameViewport; }

    void DestroyImGuiContext();

protected:

    FVector2D TransformScreenPointToImGui(const FGeometry& MyGeometry, const FVector2D& Point) const;
    
    virtual void TickKeyModifiers();

    virtual void TickImGui(float InDeltaTime);
    
    virtual void TickFocus();
    
    virtual void TakeFocus();
    
    virtual void ReturnFocus();
    
    virtual void OnDpiChanged();

    virtual bool IsConsoleOpened() const;

    virtual void DrawDebug();

    TWeakObjectPtr<UGameViewportClient> GameViewport;

    ImFontAtlas* FontAtlas = nullptr;

    TWeakPtr<SWidget> PreviousUserFocusedWidget;

    bool bEnableInput = false;

    bool bShareMouse = true;

    FSlateRenderTransform ImGuiRenderTransform;

    mutable TArray<FSlateVertex> VertexBuffer;

    mutable TArray<SlateIndex> IndexBuffer;

    TArray<FCogImguiDrawList> DrawLists;

    ImGuiContext* ImGuiContext = nullptr;

    ImPlotContext* ImPlotContext = nullptr;

    FCogImguiRenderFunction Render;

    float DpiScale = 1.f;

    char IniFilename[512];

    TSharedPtr<IInputProcessor> InputProcessor = nullptr;
};
