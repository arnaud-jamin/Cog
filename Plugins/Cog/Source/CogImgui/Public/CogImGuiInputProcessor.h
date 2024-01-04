#pragma once

#include "Framework/Application/IInputProcessor.h"
#include "CogImguiContext.h"

class FCogImguiContext;
class SCogImguiWidget;
class UPlayerInput;
enum ImGuiKey : int;
struct FKeyBind;

class FCogImGuiInputProcessor : public IInputProcessor
{
public:

	FCogImGuiInputProcessor(UPlayerInput* InPlayerInput, FCogImguiContext* InContext, SCogImguiWidget* InWidget);

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> SlateCursor) override;

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& Event) override;

	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& Event) override;

	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& Event) override;

	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& Event) override;

	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& Event) override;

	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& Event) override;

	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& Event) override;

	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& Event, const FPointerEvent* GestureEvent) override;

protected:

	bool HandleKeyEvent(FSlateApplication& SlateApp, const FKeyEvent& Event, bool IsKeyDown);

	bool HandleMouseButtonEvent(FSlateApplication& SlateApp, const FPointerEvent& Event, bool IsButtonDown);

	void AddMousePosEvent(const FVector2D& MousePosition) const;

	FCogImguiContext* Context = nullptr;

	TObjectPtr<UPlayerInput> PlayerInput;

	TObjectPtr<SCogImguiWidget> MainWidget;
};
