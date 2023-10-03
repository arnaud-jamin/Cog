#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COGINTERFACE_API FCogInterfaceModule : public IModuleInterface
{
public:

    static inline FCogInterfaceModule& Get() { return FModuleManager::LoadModuleChecked<FCogInterfaceModule>("CogInterface"); }

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

private:
};
