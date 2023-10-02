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

//--------------------------------------------------------------------------------------------------------------------------
ACogDebugReplicator* FCogDebugModule::GetLocalReplicator()
{
    return LocalReplicator;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugModule::SetLocalReplicator(ACogDebugReplicator* Value)
{
    LocalReplicator = Value;
}

//--------------------------------------------------------------------------------------------------------------------------
ACogDebugReplicator* FCogDebugModule::GetRemoteReplicator(const APlayerController* PlayerController)
{
    for (ACogDebugReplicator* Replicator : RemoteReplicators)
    {
        if (Replicator->GetPlayerController() == PlayerController)
        {
            return Replicator;
        }
    }
    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugModule::AddRemoteReplicator(ACogDebugReplicator* Value)
{
    RemoteReplicators.Add(Value);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCogDebugModule, CogDebug)