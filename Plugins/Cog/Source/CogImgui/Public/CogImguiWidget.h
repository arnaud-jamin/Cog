#pragma once

#include "CoreMinimal.h"
#include "CogImguiDrawList.h"
#include "Rendering/RenderingCommon.h"
#include "UObject/WeakObjectPtr.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"

class FCogImguiContext;
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

    ~SCogImguiWidget();

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& WidgetStyle, bool bParentEnabled) const override;

    virtual bool SupportsKeyboardFocus() const override { return true; }

    virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent) override;

    virtual FVector2D ComputeDesiredSize(float Scale) const override;

    void SetDrawData(const ImDrawData* InDrawData);

protected:

    TObjectPtr<FCogImguiContext> Context = nullptr;

    FSlateRenderTransform ImGuiRenderTransform;

    mutable TArray<FSlateVertex> VertexBuffer;

    mutable TArray<SlateIndex> IndexBuffer;

    FCogImguiDrawData DrawData;
};
