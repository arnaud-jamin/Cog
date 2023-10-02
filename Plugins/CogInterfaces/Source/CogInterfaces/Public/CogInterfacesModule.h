#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COGINTERFACES_API FCogInterfacesModule : public IModuleInterface
{
public:

    static inline FCogInterfacesModule& Get() { return FModuleManager::LoadModuleChecked<FCogInterfacesModule>("CogInterfaces"); }

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

private:
};
