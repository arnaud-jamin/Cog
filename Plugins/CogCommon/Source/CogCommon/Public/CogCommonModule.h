#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COGCOMMON_API FCogCommonModule : public IModuleInterface
{
public:

    static inline FCogCommonModule& Get() { return FModuleManager::LoadModuleChecked<FCogCommonModule>("CogCommon"); }

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

private:
};
