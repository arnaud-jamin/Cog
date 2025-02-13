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

struct COGIMGUI_API FCogImGuiViewportData
{
	TWeakPtr<SWindow> Window = nullptr;
	FCogImguiContext* Context = nullptr;
	TWeakPtr<SCogImguiWidget> Widget = nullptr;
};


struct COGIMGUI_API FCogImGuiContextScope
{
	UE_NODISCARD_CTOR explicit FCogImGuiContextScope(const FCogImguiContext& CogImguiContext);
	UE_NODISCARD_CTOR explicit FCogImGuiContextScope(ImGuiContext* GuiCtx, ImPlotContext* PlotCtx);
	~FCogImGuiContextScope();

private:
	ImGuiContext* PrevContext = nullptr;
	ImPlotContext* PrevPlotContext = nullptr;
};

class COGIMGUI_API FCogImguiContext : public TSharedFromThis<FCogImguiContext>
{
public:

	void Initialize(UGameViewportClient* InGameViewport);

	void Shutdown();

	bool GetEnableInput() const { return bEnableInput; }

	void SetEnableInput(bool InValue);

	bool GetWantCaptureMouse() const { return bWantCaptureMouse; }

	bool GetShareMouse() const { return bShareMouse; }

	void SetShareMouse(bool Value);

	bool GetShareMouseWithGameplay() const { return bShareMouseWithGameplay; }

	void SetShareMouseWithGameplay(bool Value);

	bool GetShareKeyboard() const { return bShareKeyboard; }

	void SetShareKeyboard(bool Value) { bShareKeyboard = Value; }

	bool BeginFrame(float InDeltaTime);

	void EndFrame();

	float GetDpiScale() const { return DpiScale; }

	void SetDPIScale(float Value);

	bool GetSkipRendering() const;

	void SetSkipRendering(bool Value);

	ImVec2 GetImguiMousePos() const;

	TObjectPtr<const UGameViewportClient> GetGameViewport() const { return GameViewport; }

	TSharedPtr<const SCogImguiWidget> GetMainWidget() const { return MainWidget; }

	void OnImGuiWidgetFocusLost();

	static bool GetIsNetImguiInitialized() { return bIsNetImGuiInitialized; }

private:

	friend struct FCogImGuiContextScope;

	void OnDisplayMetricsChanged(const FDisplayMetrics& DisplayMetrics) const;

	bool IsConsoleOpened() const;

	void DrawDebug() const;

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

	static const char* ImGui_GetClipboardTextFn(ImGuiContext* InImGuiContext);

	static void ImGui_SetClipboardTextFn(ImGuiContext* InImGuiContext, const char* Arg);

	static bool ImGui_OpenInShell(ImGuiContext* Context, const char* Path);

	UTexture2D* FontAtlasTexture = nullptr;

	TMap<TWeakPtr<SWindow>, ImGuiID> WindowToViewportMap;

	TSharedPtr<IInputProcessor> InputProcessor = nullptr;

	TStrongObjectPtr<UTexture2D> FontAtlasTexturePtr = nullptr;

	TSharedPtr<SCogImguiWidget> MainWidget = nullptr;

	TSharedPtr<SCogImguiInputCatcherWidget> InputCatcherWidget = nullptr;

	TWeakPtr<SWidget> PreviousMouseCaptor = nullptr;

	TObjectPtr<UGameViewportClient> GameViewport = nullptr;

	ImGuiContext* Context = nullptr;

	ImPlotContext* PlotContext = nullptr;

	char IniFilename[512] = {};

	TArray<char> ClipboardBuffer;
	
	bool bEnableInput = false;

	bool bShareMouse = false;

	bool bShareMouseWithGameplay = false;

	bool bHasSavedInitialCursorVisibility = false;

	bool bIsCursorInitiallyVisible = false;

	bool bShareKeyboard = false;

	bool bRefreshDPIScale = false;

	bool bIsThrottleDisabled = false;

	bool bIsFirstFrame = true;

	bool bWantCaptureMouse = false;

	float DpiScale = 1.f;

	bool bSkipRendering = false;

	bool bRetakeFocus = false;

	static bool bIsNetImGuiInitialized;

};
