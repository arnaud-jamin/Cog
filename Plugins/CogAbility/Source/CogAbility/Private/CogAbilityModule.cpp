#include "CogAbilityModule.h"

#include "CogAbilityReplicator.h"

#define LOCTEXT_NAMESPACE "FCogAbilityModule"

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityModule::StartupModule()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityModule::ShutdownModule()
{
}

//--------------------------------------------------------------------------------------------------------------------------
ACogAbilityReplicator* FCogAbilityModule::GetLocalReplicator()
{
    return LocalReplicator;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityModule::SetLocalReplicator(ACogAbilityReplicator* Value)
{
    LocalReplicator = Value;
}

//--------------------------------------------------------------------------------------------------------------------------
ACogAbilityReplicator* FCogAbilityModule::GetRemoteReplicator(const APlayerController* PlayerController) 
{
    for (ACogAbilityReplicator* Replicator : RemoteReplicators)
    {
        if (Replicator->GetPlayerController() == PlayerController)
        {
            return Replicator; 
        }
    }
    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityModule::AddRemoteReplicator(ACogAbilityReplicator* Value)
{
    RemoteReplicators.Add(Value);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCogAbilityModule, CogAbility)
