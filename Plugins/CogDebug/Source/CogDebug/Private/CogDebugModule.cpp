#include "CogDebugModule.h"

#define LOCTEXT_NAMESPACE "FCogDebugModule"

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugModule::StartupModule()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCogDebugModule, CogDebug)