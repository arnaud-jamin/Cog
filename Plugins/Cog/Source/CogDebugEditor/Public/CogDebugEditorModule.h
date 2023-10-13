
#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class ICogDebugEditorModule : public IModuleInterface
{

public:

    static inline ICogDebugEditorModule& Get() { return FModuleManager::LoadModuleChecked<ICogDebugEditorModule>("CogDebugEditor"); }

    static inline bool IsAvailable() { return FModuleManager::Get().IsModuleLoaded("CogDebugEditor"); }
};
