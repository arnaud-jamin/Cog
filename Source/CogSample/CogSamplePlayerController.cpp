#include "CogSamplePlayerController.h"

#include "CogDefines.h"
#include "CogSampleCharacter.h"
#include "CogSampleTargetAcquisition.h"
#include "Net/UnrealNetwork.h"

#if USE_COG
#include "CogAbilityReplicator.h"
#include "CogDebugDefines.h"
#include "CogDebugReplicator.h"
#include "CogEngineReplicator.h"
#endif //USE_COG


//--------------------------------------------------------------------------------------------------------------------------
ACogSamplePlayerController::ACogSamplePlayerController()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::BeginPlay()
{
    Super::BeginPlay();

#if USE_COG
    ACogDebugReplicator::Create(this);
    ACogAbilityReplicator::Create(this);
    ACogEngineReplicator::Create(this);
#endif //USE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::AcknowledgePossession(APawn* P)
{
    Super::AcknowledgePossession(P);

    if (ACogSampleCharacter* PossessedCharacter = Cast<ACogSampleCharacter>(P))
    {
        PossessedCharacter->OnAcknowledgePossession(this);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (TargetAcquisition != nullptr)
    {   
        TArray<AActor*> TagretToIgnore;
        FCogSampleTargetAcquisitionResult Result;
        TargetAcquisition->FindBestTarget(this, TagretToIgnore, nullptr, true, FVector2D::ZeroVector, false, Result);
        Target = Result.Target;
    }
}