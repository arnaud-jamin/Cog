#include "CogSamplePlayerController.h"

#include "CogCommon.h"
#include "CogSampleDefines.h"
#include "CogSampleCharacter.h"
#include "CogSampleLogCategories.h"
#include "CogSampleProjectileComponent.h"
#include "CogSampleTargetAcquisition.h"
#include "GameFramework/PlayerState.h"

#if ENABLE_COG
#include "CogDebugDraw.h"
#endif //ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
ACogSamplePlayerController::ACogSamplePlayerController()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::BeginPlay()
{
    Super::BeginPlay();
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

    PossessedCharacter = Cast<ACogSampleCharacter>(InPawn);

    if (InitialPossessedCharacter == nullptr)
    {
        InitialPossessedCharacter = PossessedCharacter;
    }

    OnControlledCharacterChanged.Broadcast(this, PossessedCharacter.Get(), OldControlledCharacter);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::AcknowledgePossession(APawn* NewPawn)
{
    COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Verbose, this, TEXT(""));

    ACogSampleCharacter* OldControlledCharacter = Cast<ACogSampleCharacter>(AcknowledgedPawn);

    Super::AcknowledgePossession(NewPawn);

    if (InitialPossessedCharacter == nullptr)
    {
        InitialPossessedCharacter = Cast<ACogSampleCharacter>(NewPawn);
    }

    if (PossessedCharacter != nullptr)
    {
        PossessedCharacter->AcknowledgeUnpossession();
    }

    PossessedCharacter = Cast<ACogSampleCharacter>(NewPawn);
    PossessedCharacter->AcknowledgePossession(this);

    OnControlledCharacterChanged.Broadcast(this, PossessedCharacter.Get(), OldControlledCharacter);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::SetPossession(APawn* NewPawn)
{
    if (NewPawn == nullptr || GetPawn() == NewPawn)
    {
        return;
    }

    //-------------------------------------------------------------------------------------------
    // Unplug the current controller so it doesn't conflict with the newly assigned controller
    //-------------------------------------------------------------------------------------------
    AController* OldController = NewPawn->GetController();
    if (OldController != nullptr)
    {
        COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Verbose, this, TEXT("%s unpossess %s"), *GetNameSafe(OldController), *GetNameSafe(NewPawn));
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

    COG_LOG_OBJECT(LogCogPossession, ELogVerbosity::Verbose, this, TEXT("%s possess %s"), *GetNameSafe(this), *GetNameSafe(NewPawn));
    Possess(NewPawn);

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
void ACogSamplePlayerController::ResetPossession()
{
    SetPossession(InitialPossessedCharacter.Get());
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
    
        TArray<AActor*> TargetToIgnore;
        FCogSampleTargetAcquisitionResult Result;
        TargetAcquisition->FindBestTarget(this, TargetToIgnore, nullptr, true, FVector2D::ZeroVector, false, Result);
        SetTarget(Result.Target);
    }

#if ENABLE_COG
    if (Target != nullptr && PossessedCharacter != nullptr)
    {
        FCogDebugDraw::Segment(LogCogTargetAcquisition, this, PossessedCharacter->GetActorLocation(), Target->GetActorLocation(), FColor::White, false);
    }
#endif //ENABLE_COG
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

//--------------------------------------------------------------------------------------------------------------------------
const ACogSamplePlayerController* ACogSamplePlayerController::GetFirstLocalPlayerControllerConst(const UObject* WorldContextObject)
{
    const ACogSamplePlayerController* PlayerController = GetFirstLocalPlayerController(WorldContextObject);
    return PlayerController;
}

//--------------------------------------------------------------------------------------------------------------------------
ACogSamplePlayerController* ACogSamplePlayerController::GetFirstLocalPlayerController(const UObject* WorldContextObject)
{
    UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
    if (World == nullptr)
    {
        return nullptr;
    }

    UGameInstance* const GameInstance = World->GetGameInstance();
    if (GameInstance == nullptr)
    {
        return nullptr;
    }

    ACogSamplePlayerController* PlayerController = Cast<ACogSamplePlayerController>(GameInstance->GetFirstLocalPlayerController(World));
    return PlayerController;
}

//--------------------------------------------------------------------------------------------------------------------------
float ACogSamplePlayerController::GetClientLag() const
{
    const float Ping = (PlayerState != nullptr && GetNetMode() != NM_Standalone) ? PlayerState->ExactPing : 0.0f; 
    const float HalfPingInSeconds = Ping * 0.0001f * 0.5f;
    return HalfPingInSeconds;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSamplePlayerController::Server_ProjectileHit_Implementation(UCogSampleProjectileComponent* Projectile, const FHitResult& HitResult)
{
    if (Projectile != nullptr)
    {
        Projectile->Server_Hit(HitResult);
    }
}