#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

COGAI_API DECLARE_LOG_CATEGORY_EXTERN(LogCogAI, Warning, All);

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
