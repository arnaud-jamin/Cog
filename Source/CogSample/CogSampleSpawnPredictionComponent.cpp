#include "CogSampleSpawnPredictionComponent.h"

#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "CogSampleLogCategories.h"
#include "CogSampleFunctionLibrary_Gameplay.h"
#include "CogSamplePlayerController.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

#if ENABLE_COG
#include "CogDebugDraw.h"
#include "CogDebugLog.h"
#endif //ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
// FCogSampleSpawnPredictionKey
//--------------------------------------------------------------------------------------------------------------------------
FString FCogSampleSpawnPredictionKey::ToString() const
{
    return FString::Printf(TEXT("[Creator=%s Ability=%s PredictionKey=%s Index=%d]"), *Creator.ToString(), *Ability.ToString(), *PredictionKey.ToString(), InstanceIndex);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogSampleSpawnPredictionKey::operator==(const FCogSampleSpawnPredictionKey& other) const
{
    return Creator == other.Creator
        && Ability == other.Ability
        && PredictionKey == other.PredictionKey
        && InstanceIndex == other.InstanceIndex;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogSampleSpawnPredictionKey::operator!=(const FCogSampleSpawnPredictionKey& other) const
{
    return ((*this == other) == false);
}

//--------------------------------------------------------------------------------------------------------------------------
FCogSampleSpawnPredictionKey FCogSampleSpawnPredictionKey::MakeFromAbility(const UGameplayAbility& InAbility, int32 InInstanceIndex)
{
    FGameplayAbilitySpec* Spec = InAbility.GetCurrentAbilitySpec();
    check(Spec);

    FCogSampleSpawnPredictionKey Key;
    Key.Creator = InAbility.GetAvatarActorFromActorInfo() != nullptr ? InAbility.GetAvatarActorFromActorInfo()->GetFName() : FName();
    Key.Ability = InAbility.GetFName();
    Key.PredictionKey = Spec->ActivationInfo.GetActivationPredictionKey();
    Key.InstanceIndex = InInstanceIndex;
    Key.GameTime = InAbility.GetWorld()->GetTimeSeconds();
    return Key;
}

//--------------------------------------------------------------------------------------------------------------------------
// UCogSampleSpawnPredictionComponent
//--------------------------------------------------------------------------------------------------------------------------
UCogSampleSpawnPredictionComponent::UCogSampleSpawnPredictionComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsReplicatedByDefault(true);

    PredictedSpawn = nullptr;
    ReplicatedActor = nullptr;

#if ENABLE_COG
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleSpawnPredictionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //-------------------------------------------------------------------------
    // TODO: Push Model
    //-------------------------------------------------------------------------
    DOREPLIFETIME_CONDITION(UCogSampleSpawnPredictionComponent, PredictedActorKey, COND_OwnerOnly);
    DOREPLIFETIME(UCogSampleSpawnPredictionComponent, Creator);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleSpawnPredictionComponent::BeginPlay()
{
    Super::BeginPlay();

    APawn* PawnCreator = Cast<APawn>(Creator);
    PlayerController = PawnCreator ? Cast<ACogSamplePlayerController>(PawnCreator->Controller) : nullptr;
    //IsReplicatedActor = GetOwner()->HasAuthority() == false && PlayerController != nullptr;

    if (PlayerController != nullptr)
    {
        if (GetOwner()->HasAuthority() == false)
        {
            Role = ECogSampleSpawnPredictionRole::Replicated;
        }
    }
    else
    {
        Role = ECogSampleSpawnPredictionRole::Remote;
    }

    if (PlayerController != nullptr)
    {
        COG_LOG_OBJECT(LogCogPredictedActor, ELogVerbosity::Verbose, UCogSampleFunctionLibrary_Gameplay::GetInstigator(GetOwner()), TEXT("Actor:%s - Role:%s"), *GetName(), *GetRoleName());
        ReconcileReplicatedWithPredicted();
    }

#if ENABLE_COG
    if (FCogDebugLog::IsLogCategoryActive(LogCogPredictedActor))
    {
        LastDebugLocation = GetOwner()->GetActorLocation();
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleSpawnPredictionComponent::ReconcileReplicatedWithPredicted()
{
    //-------------------------------------------------------------------------
    // If we are a replicated actor on a local client, find the corresponding 
    // predicted actor and sync them together. 
    //-------------------------------------------------------------------------
    if (Role != ECogSampleSpawnPredictionRole::Replicated)
    {
        return;
    }

    CleanInvalidPredictedActors();

    int32 PredictedActorIndex = FindSpawnPredictionIndex();
    if (PredictedActorIndex == INDEX_NONE)
    {
        COG_LOG_OBJECT(LogCogPredictedActor, ELogVerbosity::Warning, Creator.Get(), TEXT("Failed to find predicted actor. Key:%s"), *PredictedActorKey.ToString());

#if ENABLE_COG
        for (const UCogSampleSpawnPredictionComponent* Candidate : PlayerController->SpawnPredictions)
        {
            if (Candidate != nullptr)
            {
                COG_LOG_OBJECT(LogCogPredictedActor, ELogVerbosity::Warning, Creator.Get(), TEXT("  Candidate:%s"), *Candidate->PredictedActorKey.ToString());
            }
            else
            {
                COG_LOG_OBJECT(LogCogPredictedActor, ELogVerbosity::Warning, Creator.Get(), TEXT("  Candidate:NULL"));
            }
        }
#endif //ENABLE_COG

        return;
    }

    UCogSampleSpawnPredictionComponent* SpawnPrediction = PlayerController->SpawnPredictions[PredictedActorIndex];
    PlayerController->SpawnPredictions.RemoveAt(PredictedActorIndex, 1);
    SyncReplicatedWithPredicted(SpawnPrediction);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleSpawnPredictionComponent::OnRep_PredictedActorKey()
{
    COG_LOG_OBJECT(LogCogPredictedActor, ELogVerbosity::Verbose, Creator.Get(), TEXT("Actor:%s - Role:%s - PredictedActorKey:%s"), *GetName(), *GetRoleName(), *PredictedActorKey.ToString());
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleSpawnPredictionComponent::CleanInvalidPredictedActors()
{
    if (PlayerController == nullptr)
    {
        return;
    }

    TArray<UCogSampleSpawnPredictionComponent*>& SpawnPredictions = PlayerController->SpawnPredictions;
    for (int32 i = 0; i < SpawnPredictions.Num(); )
    {
        UCogSampleSpawnPredictionComponent* Candidate = SpawnPredictions[i];
        if (Candidate == nullptr || Candidate->GetOwner()->IsPendingKillPending())
        {
            SpawnPredictions.RemoveAt(i, 1);
        }
        else
        {
            i++;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
int32 UCogSampleSpawnPredictionComponent::FindSpawnPredictionIndex()
{
    if (PlayerController == nullptr)
    {
        return INDEX_NONE;
    }

    int32 Index = PlayerController->SpawnPredictions.IndexOfByPredicate([this](const UCogSampleSpawnPredictionComponent* Candidate)
        {
            if (Candidate == nullptr)
            {
                return false;
            }

            if (Candidate->GetClass() != GetClass())
            {
                return false;
            }

            if (Candidate->PredictedActorKey != PredictedActorKey)
            {
                return false;
            }

            return true;
        });

    return Index;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleSpawnPredictionComponent::SyncReplicatedWithPredicted(UCogSampleSpawnPredictionComponent* InPredictedActor)
{
    COG_LOG_OBJECT(LogCogPredictedActor, ELogVerbosity::Verbose, Creator.Get(), TEXT("Actor:%s - Role:%s - PredictedActor:%s"), *GetName(), *GetRoleName(), InPredictedActor != nullptr ? *InPredictedActor->GetName() : TEXT("NULL"));

    PredictedSpawn = InPredictedActor;

    if (PredictedSpawn != nullptr)
    {
        PredictedSpawn->ReplicatedActor = this;

        const float PredictedLifeSpawn = PredictedSpawn->GetOwner()->GetLifeSpan();
        GetOwner()->SetLifeSpan(PredictedLifeSpawn);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Destroying the replicated actor doesn't currently work because the server will keep it alive on all clients
    // no matter what. We should investigate a way to make it work, but currently we hide it.
    //------------------------------------------------------------------------------------------------------------------
    if (DestroyReplicatedActor)
    {
        SetVisibility(false);
        GetOwner()->Destroy();
    }
    else if (HideReplicatedActor)
    {
        SetVisibility(false);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleSpawnPredictionComponent::SetVisibility(bool Value)
{
    COG_LOG_OBJECT(LogCogPredictedActor, ELogVerbosity::Verbose, Creator.Get(), TEXT("Actor:%s - Role:%s - Visibility:%d"), *GetName(), *GetRoleName(), Value);

    GetOwner()->SetActorHiddenInGame(!Value);

    TArray<USceneComponent*> OtherComponents;
    GetOwner()->GetComponents<USceneComponent>(OtherComponents);

    for (int32 i = 0; i < OtherComponents.Num(); i++)
    {
        OtherComponents[i]->SetVisibility(Value);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleSpawnPredictionComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    COG_LOG_OBJECT(LogCogPredictedActor, ELogVerbosity::Verbose, Creator.Get(), TEXT("Actor:%s - Role:%s"), *GetName(), *GetRoleName());

    if (ReplicatedActor != nullptr)
    {
        ReplicatedActor->GetOwner()->Destroy();
    }

    Super::OnComponentDestroyed(bDestroyingHierarchy);
}

//--------------------------------------------------------------------------------------------------------------------------
FString UCogSampleSpawnPredictionComponent::GetRoleName() const
{
    switch (Role)
    {
        case ECogSampleSpawnPredictionRole::Server:     return "Server";
        case ECogSampleSpawnPredictionRole::Predicted:  return "Predicted";
        case ECogSampleSpawnPredictionRole::Replicated: return "Replicated";
        case ECogSampleSpawnPredictionRole::Remote:     return "Remote";
    }

    return "Unknown";
}

//--------------------------------------------------------------------------------------------------------------------------
FColor UCogSampleSpawnPredictionComponent::GetRoleColor() const
{
    switch (Role)
    {
        case ECogSampleSpawnPredictionRole::Server:     return FColor(255,   0,   0, 255);
        case ECogSampleSpawnPredictionRole::Predicted:  return FColor(255, 255,   0, 255);
        case ECogSampleSpawnPredictionRole::Replicated: return FColor(128, 128,   0, 255);
        case ECogSampleSpawnPredictionRole::Remote:     return FColor(255,   0, 255, 255);
    }

    return FColor(128, 128, 128, 255);
}

//--------------------------------------------------------------------------------------------------------------------------
#if ENABLE_COG

void UCogSampleSpawnPredictionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    const FVector Location = GetOwner()->GetActorLocation();

    if (FCogDebugLog::IsLogCategoryActive(LogCogPredictedActor))
    {
        const FColor Color = GetRoleColor();
        const FVector Delta = Location - LastDebugLocation;

        FCogDebugDraw::Axis(LogCogPredictedActor, this, Location, GetOwner()->GetActorRotation(), 50.0f, true, 0);
        FCogDebugDraw::Point(LogCogPredictedActor, this, Location, 8.0f, Color, true, 0);

        if (Delta.IsNearlyZero() == false)
        {
            FCogDebugDraw::Segment(LogCogPredictedActor, this, LastDebugLocation, Location, Color, true, 0);
        }
    }

    LastDebugLocation = Location;
}

#endif //ENABLE_COG
