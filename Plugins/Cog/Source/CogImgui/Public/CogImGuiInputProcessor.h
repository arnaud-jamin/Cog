#pragma once

#include "Framework/Application/IInputProcessor.h"

class FCogImguiContext;
class SCogImguiWidget;
class UPlayerInput;
enum ImGuiKey : int;
struct FKeyBind;

class FImGuiInputProcessor : public IInputProcessor
{
public:

	FImGuiInputProcessor(UPlayerInput* InPlayerInput, FCogImguiContext* InContext, SCogImguiWidget* InWidget);

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

	bool IsMouseInsideMainViewport();

	void AddMousePosEvent(const FVector2D& MousePosition) const;

	bool IsKeyBoundToCommand(const FKeyEvent& KeyEvent);

	static ImGuiKey ToImKey(const FKey& Key);

	static bool IsKeyEventMatchingKeyBind(const FKeyEvent& KeyEvent, const FKeyBind& KeyBind);

	static bool IsConsoleEvent(const FKeyEvent& KeyEvent);

	static bool IsStopPlaySessionEvent(const FKeyEvent& KeyEvent);

	static uint32 ToImGuiMouseButton(const FKey& MouseButton);

	TObjectPtr<FCogImguiContext> Context = nullptr;

	TObjectPtr<UPlayerInput> PlayerInput = nullptr;

	TObjectPtr<SCogImguiWidget> MainWidget = nullptr;
};
