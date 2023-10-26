#include "CogSampleAreaComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CogCommon.h"
#include "CogSampleDamageableInterface.h"
#include "CogSampleFunctionLibrary_Gameplay.h"
#include "CogSampleFunctionLibrary_Team.h"
#include "CogSampleLogCategories.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"

#if ENABLE_COG
#include "CogDebugDraw.h"
#include "CogDebugLog.h"
#endif //ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleAreaComponent::UCogSampleAreaComponent(const class FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsReplicatedByDefault(true);

    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;

    bWantsInitializeComponent = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::InitializeComponent()
{
    Super::InitializeComponent();

    RefreshOtherComponentsValues();
}

//--------------------------------------------------------------------------------------------------------------------------
//void UCogSampleAreaComponent::PreInitializeComponents()
//{
//    Super::PreInitializeComponents();
//
//    RefreshOtherComponentsValues();
//}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;

    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAreaComponent, HalfExtent, Params);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::BeginPlay()
{
    COG_LOG_OBJECT(LogCogArea, ELogVerbosity::Verbose, GetOwner()->GetInstigator(), TEXT("Area:%s"), *GetName());

    IsAtStart = true;
    IsAtEnd = false;

    if (GetOwner()->HasAuthority())
    {
        if (TickRate > 0.0f && DurationType != ECogSampleAreaDurationType::Instant)
        {
            GetWorld()->GetTimerManager().SetTimer(TickTimerHandle, this, &UCogSampleAreaComponent::OnTickEffect, TickRate, true, InitialTickDelay);
        }

        if (DurationType == ECogSampleAreaDurationType::HasDuration)
        {
            GetWorld()->GetTimerManager().SetTimer(DurationTimerHandle, this, &UCogSampleAreaComponent::OnDurationElapsed, Duration, false);
        }

        RegisterAllEffects();

        GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UCogSampleAreaComponent::OnActorEntered);
        GetOwner()->OnActorEndOverlap.AddDynamic(this, &UCogSampleAreaComponent::OnActorExited);


    }

    Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    COG_LOG_OBJECT(LogCogArea, ELogVerbosity::Verbose, GetOwner()->GetInstigator(), TEXT("Area:%s"), *GetName());

    IsAtEnd = true;

    if (CanPerformDetection() && DurationType != ECogSampleAreaDurationType::Instant)
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
void UCogSampleAreaComponent::OnDurationElapsed_Implementation()
{
    GetOwner()->Destroy();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

#if ENABLE_COG

    const AActor* AreaInstigator = UCogSampleFunctionLibrary_Gameplay::GetInstigator(GetOwner());
    if (FCogDebugLog::IsLogCategoryActive(LogCogArea) && FCogDebugSettings::IsDebugActiveForObject(AreaInstigator))
    {
        TArray<USceneComponent*, TInlineAllocator<8>> Components;
        GetOwner()->GetComponents<USceneComponent>(Components);
        for (USceneComponent* SceneComponent : Components)
        {
            if (USphereComponent* SphereComponent = Cast<USphereComponent>(SceneComponent))
            {
                FCogDebugDraw::Sphere(LogCogArea, this, SphereComponent->GetComponentLocation(), SphereComponent->GetScaledSphereRadius(), FColor::Yellow, false, 0);
            }
            else if (UBoxComponent* BoxComponent = Cast<UBoxComponent>(SceneComponent))
            {
                FCogDebugDraw::Box(LogCogArea, this, BoxComponent->GetComponentLocation(), BoxComponent->GetScaledBoxExtent(), BoxComponent->GetComponentQuat(), FColor::Yellow, false, 0);
            }
        }
    }
#endif //ENABLE_COG

}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::RegisterAllEffects()
{
    TArray<TSubclassOf<UGameplayEffect>> AllEffects;

    for (const FCogSampleAreaEffectConfig& EffectConfig : Effects)
    {
        for (TSubclassOf<UGameplayEffect> EffectClass : EffectConfig.Effects)
        {
            AllEffects.Add(EffectClass);
        }
    }

    UCogSampleFunctionLibrary_Gameplay::MakeOutgoingSpecs(GetOwner(), AllEffects, BakedEffects, EffectsMap);
}

//--------------------------------------------------------------------------------------------------------------------------
// We iterate over effects and then over actors to make sure effects are applied in order on all actors.
// One case where this is useful is an area that applies first a corruption effect and damage effect.
// The corruption must be given to everyone in the area before any damage is applied, because the corruption 
// propagate the damage to other corrupted actors.
//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::ApplyEffectsOnActors(const TArray<AActor*>& HitActors, int32 EventTypeFilter)
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
                if (UCogSampleFunctionLibrary_Team::MatchAllegiance(GetOwner(), HitActor, EffectConfig.Allegiance) == false)
                {
                    continue;
                }

                if (UCogSampleFunctionLibrary_Gameplay::IsDead(HitActor) && EffectConfig.AffectDead == false)
                {
                    continue;
                }

                UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
                if (TargetAbilitySystem == nullptr)
                {
                    continue;
                }

                TArray<FActiveGameplayEffectHandle>* TargetInsideEffects = nullptr;
                if ((EventTypeFilter & (int32)ECogSampleAreaEventType::OnInside) != 0)
                {
                    TargetInsideEffects = &InsideEffects.FindOrAdd(HitActor);
                }

                const FVector HitNormal = (HitActor->GetActorLocation() - GetOwner()->GetActorLocation()) .GetSafeNormal(0.1f, FVector::UpVector);
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

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleAreaComponent::MakeAreaOutgoingEffectSpecs(TSubclassOf<UGameplayEffect> EffectClass, FGameplayEffectSpecHandle& EffectSpecHandle) const
{
    AActor* AreaInstigator = UCogSampleFunctionLibrary_Gameplay::GetInstigator(GetOwner());
    if (AreaInstigator == nullptr)
    {
        return false;
    }

    UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AreaInstigator);
    if (AbilitySystem == nullptr)
    {
        return false;
    }

    const int32 Level = UCogSampleFunctionLibrary_Gameplay::GetProgressionLevel(GetOwner());
    EffectSpecHandle = AbilitySystem->MakeOutgoingSpec(EffectClass, Level, AbilitySystem->MakeEffectContext());

    return true;
}


//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleAreaComponent::CanPerformDetection() const
{
    if (OnlyDetectOnAuthority == false)
    {
        return true;
    }

    if (GetOwner()->HasAuthority())
    {
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::OnTickEffect_Implementation()
{
    ApplyEffectsOnActors(InsideActors, (int32)ECogSampleAreaEventType::OnTick);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::OnActorEntered_Implementation(AActor* OverlappedActor, AActor* EnteringActor)
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
void UCogSampleAreaComponent::OnActorExited_Implementation(AActor* OverlappedActor, AActor* ExitingActor)
{
    if (DurationType == ECogSampleAreaDurationType::Instant)
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
bool UCogSampleAreaComponent::CanBeAffected_Implementation(AActor* OtherActor) const
{
    check(OtherActor);

    if (IsAlreadyAffected(OtherActor))
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleAreaComponent::IsAlreadyAffected(AActor* OtherActor) const
{
    check(OtherActor);

    if (InsideActors.Contains(OtherActor))
    {
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::AffectEnteringActor_Implementation(AActor* EnteringActor)
{
    check(EnteringActor);

    if (DurationType == ECogSampleAreaDurationType::Instant)
    {
        return;
    }    

    TArray<AActor*> Actors;
    Actors.Add(EnteringActor);
    ApplyEffectsOnActors(Actors, (int32)ECogSampleAreaEventType::OnEnter | (int32)ECogSampleAreaEventType::OnInside);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::AffectExitingActor_Implementation(AActor* ExitingActor)
{
    if (DurationType == ECogSampleAreaDurationType::Instant || ExitingActor == nullptr)
    {
        return;
    }

    TArray<AActor*> Actors;
    Actors.Add(ExitingActor);
    ApplyEffectsOnActors(Actors, (int32)ECogSampleAreaEventType::OnExit);

    RemoveInsideEffects(ExitingActor);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::RemoveInsideEffects_Implementation(AActor* HitActor)
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
void UCogSampleAreaComponent::ApplyInstantEffects_Implementation(AActor* HitActor)
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::ApplyInsideEffects_Implementation(AActor* HitActor)
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::ApplyTickEffects_Implementation(AActor* HitActor)
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::SetHalfExtent(const FVector& Value)
{
    COMPARE_ASSIGN_AND_MARK_PROPERTY_DIRTY(UCogSampleAreaComponent, HalfExtent, Value, this);
    RefreshOtherComponentsValues();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::OnRep_HalfExtent()
{
    RefreshOtherComponentsValues();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAreaComponent::RefreshOtherComponentsValues()
{
    TArray<USceneComponent*, TInlineAllocator<8>> Components;
    GetOwner()->GetComponents<USceneComponent>(Components);

    for (USceneComponent* SceneComponent : Components)
    {
        if (USphereComponent* SphereComponent = Cast<USphereComponent>(SceneComponent))
        {
            if (SphereComponent->GetUnscaledSphereRadius() != HalfExtent.X)
            {
                SphereComponent->SetSphereRadius(HalfExtent.X);
            }
        }
        else if (UBoxComponent* BoxComponent = Cast<UBoxComponent>(SceneComponent))
        {
            if (BoxComponent->GetUnscaledBoxExtent() != HalfExtent)
            {
                BoxComponent->SetBoxExtent(HalfExtent);
            }
        }
        else if (UNiagaraComponent* NiagaraComponent = Cast<UNiagaraComponent>(SceneComponent))
        {
            NiagaraComponent->SetNiagaraVariableVec3("HalfExtent", HalfExtent);
            NiagaraComponent->SetNiagaraVariableFloat("TickRate", TickRate);
            NiagaraComponent->SetNiagaraVariableFloat("Duration", Duration);
        }
    }
}

