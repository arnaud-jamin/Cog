#pragma once

namespace Cog::Window
{
	DECLARE_MULTICAST_DELEGATE(FCogWindowModuleCommandDelegate);
	DECLARE_MULTICAST_DELEGATE_OneParam(FCogWindowModuleCommandLayoutDelegate, int32 /*LayoutIndex*/)
}

/**
 * 
 */
struct COGWINDOW_API FCogWindowModuleCommands
{
public:
	FCogWindowModuleCommands();

	static const TCHAR* const ToggleInputCommand;
	static const TCHAR* const LoadLayoutCommand;
	static const TCHAR* const SaveLayoutCommand;
	static const TCHAR* const ResetLayoutCommand;

	Cog::Window::FCogWindowModuleCommandDelegate OnToggleInputCommandExecuted;
	Cog::Window::FCogWindowModuleCommandLayoutDelegate OnLoadLayoutCommandExecuted;
	Cog::Window::FCogWindowModuleCommandLayoutDelegate OnSaveLayoutCommandExecuted;
	Cog::Window::FCogWindowModuleCommandDelegate OnResetLayoutCommandExecuted;

private:
	void ToggleInputImpl() const;
	void LoadLayoutImpl(const TArray<FString>& Args) const;
	void LoadLayoutImpl(const int32 LayoutIndex) const;
	void SaveLayoutImpl(const TArray<FString>& Args) const;
	void SaveLayoutImpl(const int32 LayoutIndex) const;
	void ResetLayoutImpl() const;

	TArray<FAutoConsoleCommand> Commands;
};
