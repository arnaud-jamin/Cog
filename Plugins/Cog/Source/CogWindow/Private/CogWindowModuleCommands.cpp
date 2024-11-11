#include "CogWindowModuleCommands.h"

const TCHAR* const FCogWindowModuleCommands::ToggleInputCommand = TEXT("Cog.ToggleInput");
const TCHAR* const FCogWindowModuleCommands::LoadLayoutCommand = TEXT("Cog.LoadLayout");
const TCHAR* const FCogWindowModuleCommands::SaveLayoutCommand = TEXT("Cog.SaveLayout");
const TCHAR* const FCogWindowModuleCommands::ResetLayoutCommand = TEXT("Cog.ResetLayout");

FCogWindowModuleCommands::FCogWindowModuleCommands()
{
	Commands.Emplace(ToggleInputCommand, TEXT("Toggle the input focus between the Game and ImGui"),
		FConsoleCommandDelegate::CreateRaw(this, &FCogWindowModuleCommands::ToggleInputImpl));

	Commands.Emplace(LoadLayoutCommand, TEXT("Load the layout. Cog.LoadLayout <Index>"),
		FConsoleCommandWithArgsDelegate::CreateRaw(this, &FCogWindowModuleCommands::LoadLayoutImpl));

	Commands.Emplace(SaveLayoutCommand, TEXT("Save the layout. Cog.SaveLayout <Index>"),
		FConsoleCommandWithArgsDelegate::CreateRaw(this, &FCogWindowModuleCommands::SaveLayoutImpl));

	Commands.Emplace(ResetLayoutCommand, TEXT("Reset the layout."),
		FConsoleCommandDelegate::CreateRaw(this, &FCogWindowModuleCommands::ResetLayoutImpl));
}

void FCogWindowModuleCommands::ToggleInputImpl() const
{
	OnToggleInputCommandExecuted.Broadcast();
}

void FCogWindowModuleCommands::LoadLayoutImpl(const TArray<FString>& Args) const
{
	if (Args.Num() > 0)
	{
		LoadLayoutImpl(FCString::Atoi(*Args[0]));
	}
}

void FCogWindowModuleCommands::LoadLayoutImpl(const int32 LayoutIndex) const
{
	OnLoadLayoutCommandExecuted.Broadcast(LayoutIndex);
}

void FCogWindowModuleCommands::SaveLayoutImpl(const TArray<FString>& Args) const
{
	if (Args.Num() > 0)
	{
		SaveLayoutImpl(FCString::Atoi(*Args[0]));
	}
}

void FCogWindowModuleCommands::SaveLayoutImpl(const int32 LayoutIndex) const
{
	OnSaveLayoutCommandExecuted.Broadcast(LayoutIndex);
}

void FCogWindowModuleCommands::ResetLayoutImpl() const
{
	OnResetLayoutCommandExecuted.Broadcast();
}

