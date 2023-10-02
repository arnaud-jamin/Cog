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
ACogEngineReplicator* FCogEngineModule::GetRemoteReplicator(const APlayerController* PlayerController)
{
    for (ACogEngineReplicator* Replicator : RemoteReplicators)
    {
        if (Replicator->GetPlayerController() == PlayerController)
        {
            return Replicator;
        }
    }
    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineModule::AddRemoteReplicator(ACogEngineReplicator* Value)
{
    RemoteReplicators.Add(Value);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCogEngineModule, CogEngine)