#include "CogSampleProjectileComponent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "CogCommon.h"
#include "CogSampleFunctionLibrary_Gameplay.h"
#include "CogSampleFunctionLibrary_Team.h"
#include "CogSampleLogCategories.h"

#if ENABLE_COG
#include "CogDebugLog.h"
#include "CogDebugDraw.h"
#endif //ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::BeginPlay()
{
    Super::BeginPlay();

    Creator = UCogSampleFunctionLibrary_Gameplay::GetCreator(GetOwner());
    SpawnPrediction = GetOwner()->FindComponentByClass<UCogSampleSpawnPredictionComponent>();

    if (GetOwner()->HasAuthority())
    {
        RegisterAllEffects();
    }

    Collision = Cast<USphereComponent>(CollisionReference.GetComponent(GetOwner()));
    if (Collision != nullptr)
    {
        Collision->OnComponentBeginOverlap.AddDynamic(this, &UCogSampleProjectileComponent::OnCollisionOverlapBegin);
    }

    AssistanceOverlap = Cast<USphereComponent>(OverlapReference.GetComponent(GetOwner()));
    if (AssistanceOverlap != nullptr)
    {
        AssistanceOverlap->OnComponentBeginOverlap.AddDynamic(this, &UCogSampleProjectileComponent::OnAssistanceOverlapBegin);
    }

#if ENABLE_COG
    if (FCogDebugLog::IsLogCategoryActive(LogCogProjectile))
    {
        LastDebugLocation = GetOwner()->GetActorLocation();
    }
#endif //ENABLE_COG
}


//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    //if (CosmeticComponent != nullptr)
    //{
    //    CosmeticComponent->AddLocalRotation(CurrentCosmeticAngularVelocity * DeltaTime);
    //    CurrentCosmeticAngularVelocity *= FMath::Clamp(1.0f - CosmeticAngularDrag * DeltaTime, 0.0f, 1.0f);
    //}

    //PreviousVelocity = Velocity;

    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    //TravelingTime += DeltaTime;

#if ENABLE_COG

    if (FCogDebugLog::IsLogCategoryActive(LogCogProjectile))
    {
        const float CollisionRadius = Collision != nullptr ? Collision->GetScaledSphereRadius() : 0.0f;
        const float AssistanceRadius = AssistanceOverlap != nullptr ? AssistanceOverlap->GetScaledSphereRadius() : 0.0f;
        const float DebugRadius = FMath::Max(CollisionRadius, AssistanceRadius);
        const FColor Color = SpawnPrediction != nullptr ? SpawnPrediction->GetRoleColor() : FColor(128, 128, 128, 255);
        const bool Show = SpawnPrediction == nullptr || SpawnPrediction->GetRole() != ECogSampleSpawnPredictionRole::Replicated;

        if (Show && UpdatedComponent != nullptr)
        {
            const FVector Location = UpdatedComponent->GetComponentLocation();
            const FVector Delta = Location - LastDebugLocation;

            if (LogCogProjectile.GetVerbosity() == ELogVerbosity::VeryVerbose)
            {
                FCogDebugDraw::Sphere(LogCogProjectile, GetOwner(), Location, DebugRadius, Color, true, 0);
                FCogDebugDraw::Axis(LogCogProjectile, GetOwner(), Location, UpdatedComponent->GetComponentRotation(), 50.0f, true, 0);
            }
            else
            {
                FCogDebugDraw::Point(LogCogProjectile, GetOwner(), Location, 5.0f, Color, true, 0);
            }

            if (Delta.IsNearlyZero() == false)
            {
                FCogDebugDraw::Segment(LogCogProjectile, GetOwner(), LastDebugLocation, Location, Color, true, 0);
            }

            LastDebugLocation = Location;
        }

    }

#endif //ENABLE_COG
}
//--------------------------------------------------------------------------------------------------------------------------
//void UCogSampleProjectileComponent::CatchupReplicatedActor(float CatchupDuration)
//{
//    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Projectile:%s - Role:%s - CatchupDuration:%0.2f"), *GetName(), *GetRoleName(), CatchupDuration);
//
//    const FVector OldPosition = GetOwner()->GetActorLocation();
//    const float OldSpeed = Velocity.Length();
//
//    TickComponent(CatchupDuration, LEVELTICK_All, nullptr);
//
//    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator, TEXT("Distance:%0.2f - OldSpeed:%0.2f - NewSpeed:%0.2f"),
//        (GetOwner()->GetActorLocation() - OldPosition).Length(),
//        OldSpeed,
//        Velocity.Length());
//
//    if (SpawnPrediction != nullptr)
//    {
//        if (UCogSampleSpawnPrediction* SpawnPrediction2 = SpawnPrediction->GetSpawnPrediction())
//        {
//            SpawnPrediction2->ProjectileMovement->Velocity = ProjectileMovement->Velocity;
//        }
//    }
//}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::RegisterAllEffects()
{
    TArray<TSubclassOf<UGameplayEffect>> AllEffects;

    for (const FCogSampleProjectileEffectConfig& EffectConfig : Effects)
    {
        for (TSubclassOf<UGameplayEffect> EffectClass : EffectConfig.Effects)
        {
            AllEffects.Add(EffectClass);
        }
    }

    UCogSampleFunctionLibrary_Gameplay::MakeOutgoingSpecs(GetOwner(), AllEffects, BakedEffects, EffectsMap);
}


//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleProjectileComponent::ShouldProcessOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, bool RequireValidActor)
{
    if (RequireValidActor && OtherActor == nullptr)
    {
        COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Skipped:InvalidActor"));
        return false;
    }

    if (GetOwner()->GetLocalRole() != ROLE_Authority)
    {
        COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Skipped:NotAuthority"));
        return false;
    }

    //if (IsStopped)
    //{
    //    //----------------------------------------------------------------------------------------
    //    // We can receive overlap events the same frame Stop is called. It shouldn't happen after.
    //    //----------------------------------------------------------------------------------------
    //    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Skipped:Stopped"));
    //    return false;
    //}

    if (OtherComp == nullptr)
    {
        COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Warning, Creator.Get(), TEXT("Skipped:InvalidCollider"));
        return false;
    }

    if (OtherActor == GetOwner())
    {
        COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Skipped:HittingSelf"));
        return false;
    }

    if (CanHitCreator == false && OtherActor == Creator)
    {
        COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Skipped:HittingCreator"));
        return false;
    }

    ////-----------------------------------------------------------------------------------------
    //// Don't overlap if OtherActor is the simulated proxy replicated one time from server to
    //// client for synch-up
    ////-----------------------------------------------------------------------------------------
    //if (OtherActor->IsA(AGPCoreProjectile::StaticClass()) && OtherActor->GetLocalRole() == ROLE_SimulatedProxy)
    //{
    //    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Skipped:SimulatedProxyProjectile"));
    //    return false;
    //}

    if (IsAlreadyProcessingAnOverlap)
    {
        COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Skipped:AlreadyProcessingAnOverlap"));
        return false;
    }

    //-----------------------------------------------------------------------------------------
    // Ignore multiple hits on the same actor. This can happen if this the has multiple
    // collisions, such as the character hit volumes (head, arm, chest, ...)
    //-----------------------------------------------------------------------------------------
    if (HitActors.Contains(OtherActor))
    {
        COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Skipped:AlreadyHit"));
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool IsFromSweep, const FHitResult& SweepHit)
{
    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Projectile:%s - Role:%s - Other:%s - Comp:%s"),
        *GetName(),
        *GetRoleName(),
        OtherActor != nullptr ? *OtherActor->GetName() : TEXT("NULL"),
        OtherComp != nullptr ? *OtherComp->GetName() : TEXT("NULL"));

    if (ShouldProcessOverlap(OtherActor, OtherComp, false) == false)
    {
        return;
    }

    TGuardValue<bool> OverlapGuard(IsAlreadyProcessingAnOverlap, true);
    FHitResult PreciseHit;

    if (IsFromSweep)
    {
        //-----------------------------------------------------------------------------
        // When the projectile moves, it moves with sweep activated.
        //-----------------------------------------------------------------------------
        COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Method:IsFromSweep"));
        PreciseHit = SweepHit;
    }
    else if (Collision != nullptr)
    {
        //-----------------------------------------------------------------------------
        // Trace to find the accurate collision location. Use the largest collider
        // which should be the assistance sphere, to make sure we find a result.
        // If we were using the collision sphere after an assistance sphere overlap,
        // we could miss the sweep.
        //-----------------------------------------------------------------------------
        COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Method:SweepComponent"));

        USphereComponent* TestComp = (AssistanceOverlap != nullptr && AssistanceOverlap->GetUnscaledSphereRadius() > Collision->GetUnscaledSphereRadius()) ? AssistanceOverlap : Collision;
        OtherComp->SweepComponent(PreciseHit, GetOwner()->GetActorLocation() - Velocity * 10.f, GetOwner()->GetActorLocation() + Velocity, FQuat::Identity, TestComp->GetCollisionShape(), TestComp->bTraceComplexOnMove);

        // SweepComponent specifically does not return us the Physical Material of the hit surface, so we look it up manually
        if (PreciseHit.GetComponent() != nullptr)
        {
            PreciseHit.PhysMaterial = PreciseHit.GetComponent()->GetBodyInstance()->GetSimplePhysicalMaterial();
        }
    }
    else
    {
        //-----------------------------------------------------------------------------
        // Fallback that uses a raycast if we have no CollisionComp to find a more
        // accurate collision location. It should never happen.
        //-----------------------------------------------------------------------------
        COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Method:LineTrace"));
        static const FName TraceTag(TEXT("UCogSampleProjectileComponent::OnCollisionOverlapBegin"));
        FCollisionQueryParams QueryParams(TraceTag, false, GetOwner());
        QueryParams.bReturnPhysicalMaterial = true;
        OtherComp->LineTraceComponent(PreciseHit, GetOwner()->GetActorLocation() - Velocity * 10.f, GetOwner()->GetActorLocation() + Velocity, QueryParams);
    }

    TryHit(PreciseHit);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::OnAssistanceOverlapBegin(UPrimitiveComponent* overlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool IsFromSweep, const FHitResult& Hit)
{
    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Projectile:%s - Role:%s - Other:%s - Comp:%s"),
        *GetName(),
        *GetRoleName(),
        OtherActor != nullptr ? *OtherActor->GetName() : TEXT("NULL"),
        OtherComp != nullptr ? *OtherComp->GetName() : TEXT("NULL"));

    //-------------------------------------------------------------------------------------
    // Call ShouldProcessOverlap with a requirement of a valid actor because the
    // assistance overlap is made to overlap only against actors, not static objects which
    // would be null actor
    //-------------------------------------------------------------------------------------
    if (ShouldProcessOverlap(OtherActor, OtherComp, true) == false)
    {
        return;
    }

    //-------------------------------------------------------------------------------------
    // Since PawnOverlapSphere doesn't hit blocking objects, it is possible it is touching
    // a target through a wall. Make sure that the hit is valid before proceeding.
    //
    // TODO: This is an approximation that does not always work if the assistance sphere
    // is big. The raycast can pass even if there is a collision in the trajectory of the
    // projectile, because we don't trace along the projectiles trajectory, but against
    // both actors positions.
    // 
    // This test is skipped if the projectile collision has been disabled. When the 
    // collision is disabled it means we want the projectile to pass through walls. 
    //-------------------------------------------------------------------------------------
    bool IsValidOverlap = true;
    if (Collision->GetCollisionEnabled())
    {
        static const FName TraceTag(TEXT("UCogSampleProjectileComponent::OnAssistanceOverlapBegin"));
        FCollisionQueryParams QueryParams(TraceTag, true, GetOwner());
        QueryParams.AddIgnoredActor(OtherActor);
        const FVector OtherLocation = IsFromSweep ? (FVector)Hit.Location : OtherActor->GetActorLocation();
        if (GetWorld()->LineTraceTestByProfile(OtherLocation, GetOwner()->GetActorLocation(), Collision->GetCollisionProfileName(), QueryParams) == false)
        {
            IsValidOverlap = true;
        }
    }

    //-------------------------------------------------------------------------------------
    // Call OnCollisionOverlapBegin since its doing the sweep test.
    //-------------------------------------------------------------------------------------
    if (IsValidOverlap)
    {
        OnCollisionOverlapBegin(Collision, OtherActor, OtherComp, OtherBodyIndex, IsFromSweep, Hit);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::TryHit(const FHitResult& HitResult)
{
    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Projectile:%s - Role:%s - Other:%s - Comp:%s - Bone:%s"),
        *GetName(),
        *GetRoleName(),
        HitResult.GetActor() != nullptr ? *HitResult.GetActor()->GetName() : TEXT("NULL"),
        HitResult.GetComponent() != nullptr ? *HitResult.GetComponent()->GetName() : TEXT("NULL"),
        *HitResult.BoneName.ToString());

    //-----------------------------------------------------------------------------------------
    // User defined callback for gameplay logic
    //-----------------------------------------------------------------------------------------
    if (ShouldHit(HitResult) == false)
    {
        COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Skipped by ShouldHit callback"));
        return;
    }

    HitActors.Add(HitResult.GetActor());

    //-----------------------------------------------------------------------------------------
    // User defined callback to decide what should happen to projectile (Stop, Attach, ...)
    //-----------------------------------------------------------------------------------------
    FCogSampleHitConsequence HitConsequence;
    Hit(HitResult, HitConsequence);

    if (HitConsequence.Stop)
    {
        //InternalStop(HitResult, HitConsequence);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::ClearHitActors()
{
    HitActors.Empty();
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleProjectileComponent::ShouldHit_Implementation(const FHitResult& Hit)
{
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::Hit_Implementation(const FHitResult& HitResult, FCogSampleHitConsequence& hitConsequence)
{
    AActor* HitActor = HitResult.GetActor();

#if ENABLE_COG
    FCogDebugDraw::Arrow(LogCogProjectile, GetOwner(), HitResult.Location, HitResult.Location + HitResult.Normal * 50.0f, FColor::Red, true, 0);
    FCogDebugDraw::Box(LogCogProjectile, GetOwner(), HitResult.Location, FVector(0.0f, 5.0f, 5.0f), FRotationMatrix::MakeFromX(HitResult.Normal).ToQuat(), FColor::Red, true, 0);
#endif //ENABLE_COG

    for (const FCogSampleProjectileEffectConfig& EffectConfig : Effects)
    {
        for (TSubclassOf<UGameplayEffect> EffectClass : EffectConfig.Effects)
        {
            if (UCogSampleFunctionLibrary_Team::MatchAllegiance(GetOwner(), HitActor, EffectConfig.Allegiance) == false)
            {
                continue;
            }

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

            if (UCogSampleFunctionLibrary_Gameplay::IsDead(HitActor) && EffectConfig.AffectDead == false)
            {
                continue;
            }

            UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
            if (TargetAbilitySystem == nullptr)
            {
                continue;
            }

            Spec->GetContext().AddHitResult(HitResult, true);
            TargetAbilitySystem->ApplyGameplayEffectSpecToSelf(*Spec);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FString UCogSampleProjectileComponent::GetRoleName() const
{
    if (SpawnPrediction == nullptr)
    {
        return TEXT("No Prediction");
    }

    return SpawnPrediction->GetRoleName();
}
