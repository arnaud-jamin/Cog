#include "CogEngineModule.h"

#include "CogCommon.h"
#include "CogDebugReplicator.h"
#include "CogEngineReplicator.h"
#include "GameFramework/GameModeBase.h"

#define LOCTEXT_NAMESPACE "FCogEngineModule"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineModule::StartupModule()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineModule::ShutdownModule()
{
}

void FCogEngineModule::RegisterPlayerControllerSpawnedCallback()
{
	// TODO: Ensure this is not called/handled on client
#if ENABLE_COG
	FGameModeEvents::OnGameModePostLoginEvent().AddStatic(&FCogEngineModule::OnPlayerControllerSpawned);
#endif
}

void FCogEngineModule::OnPlayerControllerSpawned(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
#if ENABLE_COG
	ACogDebugReplicator::Spawn(NewPlayer);
	ACogEngineReplicator::Spawn(NewPlayer);
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCogEngineModule, CogEngine)