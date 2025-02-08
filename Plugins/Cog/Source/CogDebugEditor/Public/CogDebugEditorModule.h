#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class ICogDebugEditorModule : public IModuleInterface
{

public:

    static ICogDebugEditorModule& Get() { return FModuleManager::LoadModuleChecked<ICogDebugEditorModule>("CogDebugEditor"); }

    static  bool IsAvailable() { return FModuleManager::Get().IsModuleLoaded("CogDebugEditor"); }
};
