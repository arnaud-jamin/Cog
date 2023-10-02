#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COGINPUT_API FCogInputModule : public IModuleInterface
{
public:

    static inline FCogInputModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FCogInputModule>("CogInput");
    }

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
};
