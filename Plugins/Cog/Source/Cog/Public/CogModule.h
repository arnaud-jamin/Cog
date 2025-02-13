#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COG_API FCogModule : public IModuleInterface
{
public:

    static inline FCogModule& Get() { return FModuleManager::LoadModuleChecked<FCogModule>("Cog"); }

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

};
