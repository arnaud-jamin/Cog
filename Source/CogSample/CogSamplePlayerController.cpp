#include "CogSamplePlayerController.h"

#include "CogDebugLogMacros.h"
#include "CogSampleDefines.h"
#include "CogSampleCharacter.h"
#include "CogSampleLogCategories.h"
#include "CogSampleTargetAcquisition.h"
#include "Net/UnrealNetwork.h"

#if USE_COG
#include "CogAbilityReplicator.h"
#include "CogDebugDefines.h"
#include "CogDebugDraw.h"
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
void ACogSamplePlayerController::OnPossess(APawn* InPawn)
{
    COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Verbose, this, TEXT(""));

    ACogSampleCharacter* OldControlledCharacter = Cast<ACogSampleCharacter>(GetPawn());

    //----------------------------------------------------------------------------------------------------------
    // Case of pawn was possessed too early by the server before a join complete
    //----------------------------------------------------------------------------------------------------------
    if (InPawn != nullptr && InPawn->Controller != nullptr)
    {
        COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Warning, this, TEXT("Asking %s to possess pawn %s more than once. Pawn will be restarted! Should call Unpossess first if possible."), *GetNameSafe(this), *GetNameSafe(InPawn));
        InPawn->Controller->UnPossess();
    }

    Super::OnPossess(InPawn);

    if (InPawn != nullptr)
    {
        //on server, the pawn are not possessed on begin play callback, need to do it here.
        if (IsLocalController())
        {
            SetControlRotation(InPawn->GetActorRotation());
        }
    }

    ControlledCharacter = Cast<ACogSampleCharacter>(InPawn);

    if (InitialControlledCharacter == nullptr)
    {
        InitialControlledCharacter = ControlledCharacter;
    }

    OnControlledCharacterChanged.Broadcast(this, ControlledCharacter.Get(), OldControlledCharacter);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::AcknowledgePossession(APawn* NewPawn)
{
    COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Verbose, this, TEXT(""));

    ACogSampleCharacter* OldControlledCharacter = Cast<ACogSampleCharacter>(AcknowledgedPawn);

    Super::AcknowledgePossession(NewPawn);

    if (InitialControlledCharacter == nullptr)
    {
        InitialControlledCharacter = Cast<ACogSampleCharacter>(NewPawn);
    }

    if (ControlledCharacter != nullptr)
    {
        ControlledCharacter->AcknowledgeUnpossession();
    }

    ControlledCharacter = Cast<ACogSampleCharacter>(NewPawn);
    ControlledCharacter->AcknowledgePossession(this);

    OnControlledCharacterChanged.Broadcast(this, ControlledCharacter.Get(), OldControlledCharacter);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::ControlCharacter(ACogSampleCharacter* NewCharacter)
{
    if (NewCharacter == nullptr || GetPawn() == NewCharacter)
    {
        return;
    }

    //-------------------------------------------------------------------------------------------
    // Unplug the current controller so it doesn't conflict with the newly assigned controller
    //-------------------------------------------------------------------------------------------
    AController* OldController = NewCharacter->GetController();
    if (OldController != nullptr)
    {
        COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Verbose, this, TEXT("%s unpossess %s"), *GetNameSafe(OldController), *GetNameSafe(NewCharacter));
        OldController->UnPossess();
    }

    //-------------------------------------------------------------------------------------------
    // We will need to replug the initial controller of the character we currently control
    //-------------------------------------------------------------------------------------------
    ACogSampleCharacter* OldCharacter = Cast<ACogSampleCharacter>(GetPawn());

    //-------------------------------------------------------------------------------------------
    // Unpossess before possession to prevent a warning
    //-------------------------------------------------------------------------------------------
    COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Verbose, this, TEXT("%s unpossess %s"), *GetNameSafe(this), *GetNameSafe(GetPawn()));
    UnPossess();

    COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Verbose, this, TEXT("%s possess %s"), *GetNameSafe(this), *GetNameSafe(NewCharacter));
    Possess(NewCharacter);

    //-------------------------------------------------------------------------------------------
    // Replug the initial controller on the old character. For example, replug the initial
    // AI controller of an AI when the player finishes controlling it. This needs to be done
    // after the Possess call.
    //-------------------------------------------------------------------------------------------
    if (OldCharacter != nullptr && OldCharacter->InitialController != nullptr && OldCharacter->InitialController != this)
    {
        COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Verbose, this, TEXT("%s possess %s"), *GetNameSafe(OldCharacter->InitialController), *GetNameSafe(OldCharacter));
        OldCharacter->InitialController->Possess(OldCharacter);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::ResetControlledPawn()
{
    ControlCharacter(InitialControlledCharacter.Get());
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    TickTargeting(DeltaSeconds);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::TickTargeting(float DeltaSeconds)
{
    if (IsLocalController())
    {
        if (TargetAcquisition == nullptr)
        {
            SetTarget(nullptr);
            return;
        }
    
        TArray<AActor*> TagretToIgnore;
        FCogSampleTargetAcquisitionResult Result;
        TargetAcquisition->FindBestTarget(this, TagretToIgnore, nullptr, true, FVector2D::ZeroVector, false, Result);
        SetTarget(Result.Target);
    }

#if USE_COG
    if (Target != nullptr && ControlledCharacter != nullptr)
    {
        FCogDebugDraw::Segment(LogCogTargetAcquisition, ControlledCharacter.Get(), ControlledCharacter->GetActorLocation(), Target->GetActorLocation(), FColor::White, false);
    }
#endif //USE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::SetTarget(AActor* Value)
{
    if (Value == Target)
    {
        return;
    }

    AActor* OldTarget = Target.Get();

    Target = Value;

    if (GetLocalRole() == ROLE_AutonomousProxy)
    {
        Server_SetTarget(Value);
    }

    OnTargetChanged.Broadcast(this, Target.Get(), OldTarget);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::Server_SetTarget_Implementation(AActor* Value)
{
    Target = Value;
}
