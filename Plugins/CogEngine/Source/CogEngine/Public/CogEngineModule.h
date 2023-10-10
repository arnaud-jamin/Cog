#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COGENGINE_API FCogEngineModule : public IModuleInterface
{
public:

    static inline FCogEngineModule& Get() { return FModuleManager::LoadModuleChecked<FCogEngineModule>("CogEngine"); }

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

private:
};
