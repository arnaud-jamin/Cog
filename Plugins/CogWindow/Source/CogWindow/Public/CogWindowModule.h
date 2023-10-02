#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COGWINDOW_API FCogWindowModule : public IModuleInterface
{
public:

    static inline FCogWindowModule& Get() { return FModuleManager::LoadModuleChecked<FCogWindowModule>("CogWindow"); }

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

private:
};
