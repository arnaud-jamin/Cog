#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COGALL_API FCogAllModule : public IModuleInterface 
{

public:
  
   static FCogAllModule &Get() { return FModuleManager::LoadModuleChecked<FCogAllModule>("CogAll"); }

  /** IModuleInterface implementation */
  virtual void StartupModule() override;
  virtual void ShutdownModule() override;

private:
};
