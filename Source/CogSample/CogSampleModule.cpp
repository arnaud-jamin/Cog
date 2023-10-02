#include "CogSampleModule.h"

#include "CogSampleLogCategories.h"
#include "Modules/ModuleManager.h"

//--------------------------------------------------------------------------------------------------------------------------
class FCogSampleModule : public IModuleInterface
{
public:

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
};

#define LOCTEXT_NAMESPACE "FCogInputModule"

//--------------------------------------------------------------------------------------------------------------------------
void FCogSampleModule::StartupModule()
{
    CogSampleLog::RegiterAllLogCategories();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogSampleModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_PRIMARY_GAME_MODULE(FCogSampleModule, CogSample, "CogSample");
