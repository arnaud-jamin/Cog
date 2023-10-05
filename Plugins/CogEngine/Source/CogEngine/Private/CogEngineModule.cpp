#include "CogEngineModule.h"

#define LOCTEXT_NAMESPACE "FCogEngineModule"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineModule::StartupModule()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineModule::ShutdownModule()
{
}

//--------------------------------------------------------------------------------------------------------------------------
ACogEngineReplicator* FCogEngineModule::GetLocalReplicator()
{
    return LocalReplicator;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineModule::SetLocalReplicator(ACogEngineReplicator* Value)
{
    LocalReplicator = Value;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineModule::AddRemoteReplicator(ACogEngineReplicator* Value)
{
    RemoteReplicators.Add(Value);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCogEngineModule, CogEngine)