#include "CogSampleArea.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CogCommon.h"
#include "CogSampleDamageableInterface.h"
#include "CogSampleFunctionLibrary_Team.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

//--------------------------------------------------------------------------------------------------------------------------
ACogSampleArea::ACogSampleArea(const class FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bReplicates = true;

    PrimaryActorTick.bCanEverTick = true;
    SetActorTickEnabled(true);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACogSampleArea, HalfExtent);
    DOREPLIFETIME(ACogSampleArea, Level);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::BeginPlay()
{
    COG_LOG_OBJECT(LogCogArea, ELogVerbosity::Verbose, GetInstigator(), TEXT("Area:%s"), *GetName());

    IsAtStart = true;
    IsAtEnd = false;

    if (HasAuthority())
    {
        if (TickRate > 0.0f && IsInstant == false)
        {
            GetWorld()->GetTimerManager().SetTimer(TickTimerHandle, this, &ACogSampleArea::OnTickEffect, TickRate, true, InitialTickDelay);
        }

        RegisterAllEffects();

        OnActorBeginOverlap.AddDynamic(this, &ACogSampleArea::OnActorEntered);
        OnActorEndOverlap.AddDynamic(this, &ACogSampleArea::OnActorExited);
    }

    Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    COG_LOG_OBJECT(LogCogArea, ELogVerbosity::Verbose, GetInstigator(), TEXT("Area:%s"), *GetName());

    IsAtEnd = true;

    if (CanPerformDetection() && IsInstant == false)
    {
        int32 EventTypeFilter = (int32)ECogSampleAreaEventType::OnEnd;

        if (ApplyTickEffectOnExit)
        {
            EventTypeFilter |= (int32)ECogSampleAreaEventType::OnTick;
        }

        ApplyEffectsOnActors(InsideActors, EventTypeFilter);

        for (AActor* Actor : InsideActors)
        {
            AffectExitingActor(Actor);
        }
    }

    InsideActors.Empty();

    Super::EndPlay(EndPlayReason);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    //------------------------------------------------------------------------------
    // We cannot execute OnStart event inside the OnActorEntered callback because 
    // we need to know all the actors that are inside to execute the OnStart event.
    // This is because we must iterate over all actors for each effects we need 
    // to apply. See ApplyEffectsOnActors as to why.
    // So instead we execute it on Tick
    //------------------------------------------------------------------------------
    if (IsAtStart)
    {
        int32 EventTypeFilter = (int32)ECogSampleAreaEventType::OnStart;
        
        if (ApplyTickEffectOnEnter)
        {
            EventTypeFilter |= (int32)ECogSampleAreaEventType::OnTick;
        }

        ApplyEffectsOnActors(InsideActors, EventTypeFilter);

        IsAtStart = false;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
AActor* ACogSampleArea::GetInstigatorActor() const
{
    AActor* AreaInstigator = GetInstigator();
    if (AreaInstigator != nullptr)
    {
        return AreaInstigator;
    }

    if (ParentActor != nullptr)
    {
        return ParentActor;
    }

    //-------------------------------------------------
    // The game state is the default Instigator
    //-------------------------------------------------
    UWorld* World = GetWorld();
    if (World != nullptr)
    {
        return World->GetGameState();
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::RegisterAllEffects()
{
    AActor* AreaInstigator = GetInstigatorActor();
    if (AreaInstigator == nullptr)
    {
        return;
    }

    UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AreaInstigator);
    if (AbilitySystem == nullptr)
    {
        return;
    }

    FGameplayEffectContextHandle EffectContextHandle = AbilitySystem->MakeEffectContext();

    for (const FCogSampleAreaEffectConfig& EffectConfig : Effects)
    {
        for (TSubclassOf<UGameplayEffect> EffectClass : EffectConfig.Effects)
        {
            if (EffectsMap.Contains(EffectClass))
            {
                continue;
            }

            FGameplayEffectSpecHandle* BakedEffectSpecHandle = BakedEffects.FindByPredicate([EffectClass](const FGameplayEffectSpecHandle& Handle) 
                { 
                    return Handle.Data->Def.GetClass() == EffectClass; 
                });

            if (BakedEffectSpecHandle != nullptr)
            {
                EffectsMap.Add(EffectClass, *BakedEffectSpecHandle);
            }
            else
            {
                FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystem->MakeOutgoingSpec(EffectClass, Level, EffectContextHandle);
                EffectsMap.Add(EffectClass, EffectSpecHandle);
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// We iterate over effects and then over actors to make sure effects are applied in order on all actors.
// One case where this is useful is an area that applies first a corruption effect and damage effect.
// The corruption must be given to everyone in the area before any damage is applied, because the corruption 
// propagate the damage to other corrupted actors.
//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::ApplyEffectsOnActors(const TArray<AActor*>& HitActors, int32 EventTypeFilter)
{
    for (const FCogSampleAreaEffectConfig& EffectConfig : Effects)
    {
        if (((int32)EffectConfig.Event & (int32)EventTypeFilter) == 0)
        {
            continue;
        }

        for (TSubclassOf<UGameplayEffect> EffectClass : EffectConfig.Effects)
        {
            FGameplayEffectSpecHandle* Handle = EffectsMap.Find(EffectClass);
            if (Handle == nullptr)
            {
                continue;
            }

            FGameplayEffectSpec* Spec = Handle->Data.Get();
            if (Spec == nullptr)
            {
                continue;
            }

            for (AActor* HitActor : HitActors)
            {                
                if (UCogSampleFunctionLibrary_Team::MatchAllegianceBetweenTeamAndActor(Team, HitActor, EffectConfig.Allegiance) == false)
                {
                    continue;
                }

                if (IsAliveOfAffectDead(HitActor) == false)
                {
                    continue;
                }

                UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
                if (TargetAbilitySystem != nullptr)
                {
                    TArray<FActiveGameplayEffectHandle>* TargetInsideEffects = nullptr;
                    if ((EventTypeFilter & (int32)ECogSampleAreaEventType::OnInside) != 0)
                    {
                        TargetInsideEffects = &InsideEffects.FindOrAdd(HitActor);
                    }

                    const FVector HitNormal = (HitActor->GetActorLocation() - GetActorLocation()) .GetSafeNormal(0.1f, FVector::UpVector);
                    const FHitResult HitResult(HitActor, nullptr, HitActor->GetActorLocation(), HitNormal);
                    Spec->GetContext().AddHitResult(HitResult, true);

                    FActiveGameplayEffectHandle ActiveEffectHandle = TargetAbilitySystem->ApplyGameplayEffectSpecToSelf(*Spec);

                    if (EffectConfig.Event == ECogSampleAreaEventType::OnInside && TargetInsideEffects != nullptr)
                    {
                        TargetInsideEffects->Add(ActiveEffectHandle);
                    }
                }                
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool ACogSampleArea::MakeAreaOutgoingEffectSpecs(TSubclassOf<UGameplayEffect> EffectClass, FGameplayEffectSpecHandle& EffectSpecHandle) const
{
    AActor* AreaInstigator = GetInstigatorActor();
    if (AreaInstigator == nullptr)
    {
        return false;
    }

    UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AreaInstigator);
    if (AbilitySystem == nullptr)
    {
        return false;
    }

    EffectSpecHandle = AbilitySystem->MakeOutgoingSpec(EffectClass, Level, AbilitySystem->MakeEffectContext());
    return true;
}


//--------------------------------------------------------------------------------------------------------------------------
bool ACogSampleArea::CanPerformDetection() const
{
    if (OnlyDetectOnAuthority == false)
    {
        return true;
    }

    if (HasAuthority())
    {
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::OnTickEffect_Implementation()
{
    ApplyEffectsOnActors(InsideActors, (int32)ECogSampleAreaEventType::OnTick);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::OnActorEntered_Implementation(AActor* OverlappedActor, AActor* EnteringActor)
{
    check(EnteringActor);
    if (CanBeAffected(EnteringActor) == false)
    {
        return;
    }

    AffectEnteringActor(EnteringActor);
    InsideActors.Add(EnteringActor);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::OnActorExited_Implementation(AActor* OverlappedActor, AActor* ExitingActor)
{
    if (IsInstant)
    {
        return;
    }

    int32 Index = InsideActors.Find(ExitingActor);
    if (Index == INDEX_NONE)
    {
        return;
    }

    AffectExitingActor(ExitingActor);

    InsideActors.RemoveAt(Index);
}

//--------------------------------------------------------------------------------------------------------------------------
bool ACogSampleArea::CanBeAffected_Implementation(AActor* OtherActor) const
{
    check(OtherActor);

    if (IsAlreadyAffected(OtherActor))
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
bool ACogSampleArea::IsAliveOfAffectDead(AActor* OtherActor) const
{
    check(OtherActor);

    if (AffectDead)
    {
        return true;
    }

    const ICogSampleDamageableInterface* Damageable = Cast<ICogSampleDamageableInterface>(OtherActor);
    if (Damageable == nullptr)
    {
        return true;
    }

    if (Damageable->IsDead() == false)
    {
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool ACogSampleArea::IsAlreadyAffected(AActor* OtherActor) const
{
    check(OtherActor);

    if (InsideActors.Contains(OtherActor))
    {
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::AffectEnteringActor_Implementation(AActor* EnteringActor)
{
    check(EnteringActor);

    if (IsInstant == false)
    {
        TArray<AActor*> Actors;
        Actors.Add(EnteringActor);
        ApplyEffectsOnActors(Actors, (int32)ECogSampleAreaEventType::OnEnter | (int32)ECogSampleAreaEventType::OnInside);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::AffectExitingActor_Implementation(AActor* ExitingActor)
{
    if (IsInstant || ExitingActor == nullptr)
    {
        return;
    }

    TArray<AActor*> Actors;
    Actors.Add(ExitingActor);
    ApplyEffectsOnActors(Actors, (int32)ECogSampleAreaEventType::OnExit);

    RemoveInsideEffects(ExitingActor);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::RemoveInsideEffects_Implementation(AActor* HitActor)
{
    UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
    if (TargetAbilitySystem == nullptr)
    {
        return;
    }

    TArray<FActiveGameplayEffectHandle>* TargetInsideEffects = InsideEffects.Find(HitActor);
    if (TargetInsideEffects == nullptr)
    {
        return;
    }

    for (FActiveGameplayEffectHandle Handle : *TargetInsideEffects)
    {
        TargetAbilitySystem->RemoveActiveGameplayEffect(Handle, 1);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::ApplyInstantEffects_Implementation(AActor* HitActor)
{
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::ApplyInsideEffects_Implementation(AActor* HitActor)
{
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleArea::ApplyTickEffects_Implementation(AActor* HitActor)
{
}
