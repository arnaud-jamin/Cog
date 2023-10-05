#include "CogDebugModule.h"

#include "CogDebugReplicator.h"
#include "EngineUtils.h"

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
ACogDebugReplicator* FCogDebugModule::GetLocalReplicator(UWorld& World)
{
    for (TActorIterator<ACogDebugReplicator> It(&World, ACogDebugReplicator::StaticClass()); It; ++It)
    {
        ACogDebugReplicator* Replicator = *It;
        return Replicator;
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugModule::GetRemoteReplicators(UWorld& World, TArray<ACogDebugReplicator*>& Replicators)
{
    for (TActorIterator<ACogDebugReplicator> It(&World, ACogDebugReplicator::StaticClass()); It; ++It)
    {
        ACogDebugReplicator* Replicator = Cast<ACogDebugReplicator>(*It);
        Replicators.Add(Replicator);
    }
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCogDebugModule, CogDebug)