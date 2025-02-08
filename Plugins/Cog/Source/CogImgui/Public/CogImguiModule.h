#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COGIMGUI_API FCogImguiModule : public IModuleInterface
{
public:

    static FCogImguiModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FCogImguiModule>("CogImgui");
    }

    //----------------------------------------------------------------------------------------------------------------------
    // IModuleInterface implementation
    //----------------------------------------------------------------------------------------------------------------------
    virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
