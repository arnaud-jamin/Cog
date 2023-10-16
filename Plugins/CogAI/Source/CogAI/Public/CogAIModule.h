#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COGAI_API FCogAIModule : public IModuleInterface
{
public:

    static inline FCogAIModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FCogAIModule>("CogAI");
    }

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
};
