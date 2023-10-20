#include "CogAIModule.h"

#define LOCTEXT_NAMESPACE "FCogAIModule"

DEFINE_LOG_CATEGORY(LogCogAI);

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIModule::StartupModule()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCogAIModule, CogAI)