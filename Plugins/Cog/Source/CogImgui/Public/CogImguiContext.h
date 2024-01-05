#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h" 
#include "imgui.h"
#include "Templates/SharedPointer.h"
#include "UObject/StrongObjectPtr.h"

class FCogImguiContext;
class IInputProcessor;
class SCogImguiWidget;
class SCogImguiWidget;
class SCogImguiInputCatcherWidget;
class SWidget;
class SWindow;
class UGameViewportClient;
class ULocalPlayer;
struct FDisplayMetrics;
struct ImPlotContext;

struct COGIMGUI_API FImGuiViewportData
{
	TWeakPtr<SWindow> Window = nullptr;
	FCogImguiContext* Context = nullptr;
	TWeakPtr<SCogImguiWidget> Widget = nullptr;
};

class COGIMGUI_API FCogImguiContext : public TSharedFromThis<FCogImguiContext>
{
public:

	void Initialize();

	void Shutdown();

	bool GetEnableInput() const { return bEnableInput; }

	void SetEnableInput(bool Value);

	bool GetWantCaptureMouse() const { return bWantCaptureMouse; }

	bool GetShareMouse() const { return bShareMouse; }

	void SetShareMouse(bool Value);

	bool GetShowCursorWhenSharingMouse() const { return bShowCursorWhenSharingMouse; }

	void SetShowCursorWhenSharingMouse(bool Value);

	bool GetShareKeyboard() const { return bShareKeyboard; }

	void SetShareKeyboard(bool Value) { bShareKeyboard = Value; }

	bool BeginFrame(float InDeltaTime);

	void EndFrame();

	float GetDpiScale() const { return DpiScale; }

	void SetDPIScale(float Value);

	void PushCaptureMouse();

	void PopCaptureMouse();

	TObjectPtr<const UGameViewportClient> GetGameViewport() const { return GameViewport; }

	TSharedPtr<const SCogImguiWidget> GetMainWidget() const { return MainWidget; }

private:

	void OnDisplayMetricsChanged(const FDisplayMetrics& DisplayMetrics) const;

	bool IsConsoleOpened() const;

	void DrawDebug();

	void BuildFont();

	void RefreshMouseCursor();

	ULocalPlayer* GetLocalPlayer() const;

	static void ImGui_CreateWindow(ImGuiViewport* Viewport);

	static void ImGui_DestroyWindow(ImGuiViewport* Viewport);

	static void ImGui_ShowWindow(ImGuiViewport* Viewport);

	static void ImGui_SetWindowPos(ImGuiViewport* Viewport, ImVec2 Pos);

	static ImVec2 ImGui_GetWindowPos(ImGuiViewport* Viewport);

	static void ImGui_SetWindowSize(ImGuiViewport* Viewport, ImVec2 Size);

	static ImVec2 ImGui_GetWindowSize(ImGuiViewport* Viewport);

	static void ImGui_SetWindowFocus(ImGuiViewport* Viewport);

	static bool ImGui_GetWindowFocus(ImGuiViewport* Viewport);

	static bool ImGui_GetWindowMinimized(ImGuiViewport* Viewport);

	static void ImGui_SetWindowTitle(ImGuiViewport* Viewport, const char* TitleAnsi);

	static void ImGui_SetWindowAlpha(ImGuiViewport* Viewport, float Alpha);

	static void ImGui_RenderWindow(ImGuiViewport* Viewport, void* Data);

	UPROPERTY()
	UTexture2D* FontAtlasTexture = nullptr;

	TMap<TWeakPtr<SWindow>, ImGuiID> WindowToViewportMap;

	TSharedPtr<IInputProcessor> InputProcessor = nullptr;

	TStrongObjectPtr<UTexture2D> FontAtlasTexturePtr = nullptr;

	TSharedPtr<SCogImguiWidget> MainWidget = nullptr;

	TSharedPtr<SCogImguiInputCatcherWidget> InputCatcherWidget = nullptr;

	TWeakPtr<SWidget> PreviousMouseCaptor = nullptr;

	TObjectPtr<UGameViewportClient> GameViewport = nullptr;

	ImGuiContext* ImGuiContext = nullptr;

	ImPlotContext* PlotContext = nullptr;

	char IniFilename[512] = {};

	bool bEnableInput = false;

	bool bShareMouse = false;

	bool bShowCursorWhenSharingMouse = false;

	bool bPlayerControllerShowMouse = false;

	bool bShareKeyboard = false;

	bool bRefreshDPIScale = false;

	bool bIsThrottleDisabled = false;

	bool bIsFirstFrame = true;

	bool bWantCaptureMouse = false;

	int32 CaptureMouseCount = 0;

	float DpiScale = 1.f;
};
