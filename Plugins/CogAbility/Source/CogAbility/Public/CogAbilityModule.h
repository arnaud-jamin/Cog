#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COGABILITY_API FCogAbilityModule : public IModuleInterface
{
public:

    static inline FCogAbilityModule& Get() { return FModuleManager::LoadModuleChecked<FCogAbilityModule>("CogAbility"); }

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
