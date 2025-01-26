#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COGCOMMONUI_API FCogCommonUIModule : public IModuleInterface
{
public:

    static inline FCogCommonUIModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FCogCommonUIModule>("CogCommonUI");
    }

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
};
