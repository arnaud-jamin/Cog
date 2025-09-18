#include "CogSampleBasicActor.h"

#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

//--------------------------------------------------------------------------------------------------------------------------
ACogSampleBasicActor::ACogSampleBasicActor(const FObjectInitializer& ObjectInitializer)
{
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleBasicActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;

    DOREPLIFETIME_WITH_PARAMS_FAST(ACogSampleBasicActor, Team, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ACogSampleBasicActor, ProgressionLevel, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ACogSampleBasicActor, Creator, Params);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleBasicActor::BeginPlay()
{
    //-----------------------------------------------------
    // Set default creator before Super::BeginPlay()
    // so component can access it inside they BeginPlay()
    //-----------------------------------------------------
    if (Creator == nullptr)
    {
        Creator = GetWorld()->GetGameState();
    }

    Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleBasicActor::SetTeam(int32 Value)
{
    COMPARE_ASSIGN_AND_MARK_PROPERTY_DIRTY(ACogSampleBasicActor, Team, Value, this);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleBasicActor::SetProgressionLevel(int32 Value)
{
    COMPARE_ASSIGN_AND_MARK_PROPERTY_DIRTY(ACogSampleBasicActor, ProgressionLevel, Value, this);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleBasicActor::SetCreator(AActor* Value)
{
    COMPARE_ASSIGN_AND_MARK_PROPERTY_DIRTY(ACogSampleBasicActor, Creator, Value, this);
}
