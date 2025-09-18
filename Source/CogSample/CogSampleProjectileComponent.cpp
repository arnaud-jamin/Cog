#include "CogSampleProjectileComponent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "CogCommon.h"
#include "CogSampleFunctionLibrary_Gameplay.h"
#include "CogSampleFunctionLibrary_Team.h"
#include "CogSampleLogCategories.h"
#include "CogSamplePlayerController.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

#if ENABLE_COG
#include "CogDebugLog.h"
#include "CogDebugDraw.h"
#endif //ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
// Postulates:
//--------------------------------------------------------------------------------------------------------------------------
//
//  Projectiles can be launched by a Player or a NPC.
//  Projectiles can hit the world, or another player, or a NPC.
//  When hitting characters, projectiles do not hit the characters' capsules, but their animated hit boxes.
//  The server do not update animations and therefore do not update animated hit boxes(like Fortnite)
//  The server applies the gameplay effects related to projectile hit.
//
//--------------------------------------------------------------------------------------------------------------------------
// Questions/Answers:
//--------------------------------------------------------------------------------------------------------------------------
//
//  Where should projectile collision happen ? On the server, or on the client ?
//
//      On the clients.If projectiles should hit animated hit boxes, and if hit boxes are not updated on the server, 
//      then we need to rely on the clients to perform the collision detection.Clients should send the hit result to the 
//      server, the server should verify if the result is valid(anti cheat), and then apply gameplay effects.
//
//  If the projectile is launched by a NPC and hit a player, which client should send the hit result to the server ? 
//  The client that got hit, or another client that also detected the hit ? 
// 
//
//
//  If the projectile is launched by a player and hit another player, which client should detect and send the hit result 
//  to the server ? The player that launched the projectile or/ and the player that got hit ?
//
//      If the server only receives the hit from the client that got hit, then that client can prevent this message 
//      from being sent, or he can alter the message to gain advantages. The server needs another information. 
//      This information can come from the launcher of the projectile.
//
//  If the projectile is launched by a NPC and hit another NPC, which client should send the hit result to the server ?
//
//      One client could be made responsible to detect hits received by a specific NPC.
// 
//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleProjectileComponent::UCogSampleProjectileComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsReplicatedByDefault(true);
    bAutoActivate = false;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;

    Params.Condition = COND_None;
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleProjectileComponent, ServerSpawnLocation, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleProjectileComponent, ServerSpawnRotation, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleProjectileComponent, ServerSpawnVelocity, Params);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::BeginPlay()
{
    Super::BeginPlay();

    Creator = UCogSampleFunctionLibrary_Gameplay::GetCreator(GetOwner());
    SpawnPrediction = GetOwner()->FindComponentByClass<UCogSampleSpawnPredictionComponent>();
    LocalPlayerController = ACogSamplePlayerController::GetFirstLocalPlayerController(this);

    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Projectile:%s | Role:%s | IsActive:%d"), *GetNameSafe(GetOwner()), *GetRoleName(), IsActive());

    RegisterAllEffects();

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

    if (GetOwner()->GetLocalRole() != ROLE_Authority)
    {
        //-----------------------------------------------------------------------------------------------
        // Must force the activation because the server might have already replicated the IsActive flag.
        //-----------------------------------------------------------------------------------------------
        Activate(true);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::Activate(bool bReset)
{
    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Projectile:%s | Role:%s | bReset:%d"), *GetNameSafe(GetOwner()), *GetRoleName(), bReset);

    //------------------------------------------------------------------------------------------------
    // Save the spawn location and rotation and get them replicated because, we want remote clients 
    // to tick projectiles on their own, and be synced with the server. To do so they need to 
    // recompute (to catchup) the projectile movement from its initial location, rotation and velocity.
    // If we don't reposition the remote client projectile at its initial values, we get some offset
    // because the server doesn't necessarly replicates the location and rotation of the projectile
    // at its spawn frame.
    //------------------------------------------------------------------------------------------------
    if (GetOwner()->GetLocalRole() == ROLE_Authority)
    {
        COMPARE_ASSIGN_AND_MARK_PROPERTY_DIRTY(UCogSampleProjectileComponent, ServerSpawnLocation, GetOwner()->GetActorLocation(), this);
        COMPARE_ASSIGN_AND_MARK_PROPERTY_DIRTY(UCogSampleProjectileComponent, ServerSpawnRotation, GetOwner()->GetActorRotation(), this);
        COMPARE_ASSIGN_AND_MARK_PROPERTY_DIRTY(UCogSampleProjectileComponent, ServerSpawnVelocity, Velocity, this);
    }
    else
    {
        GetOwner()->SetActorLocationAndRotation(ServerSpawnLocation, ServerSpawnRotation);
        Velocity = ServerSpawnVelocity;
    }

#if ENABLE_COG
    DrawDebugCurrentState(FColor::Green);
    if (FCogDebugLog::IsLogCategoryActive(LogCogProjectile))
    {
        LastDebugLocation = GetOwner()->GetActorLocation();
    }
#endif //ENABLE_COG

    //--------------------------------------------------------------------------
    // Catchup after settings LastDebugLocation because Tick will be triggered 
    // by Catchup and LastDebugLocation is used in Tick debug draw.
    //--------------------------------------------------------------------------
    if (GetOwner()->GetLocalRole() != ROLE_Authority)
    {
        if (LocalPlayerController != nullptr)
        {
            Catchup(LocalPlayerController->GetClientLag());
        }
    }

    Super::Activate(bReset);
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
        const bool Show = SpawnPrediction == nullptr || SpawnPrediction->GetRole() != ECogSampleSpawnPredictionRole::Replicated;
        const FColor Color = (SpawnPrediction != nullptr ? SpawnPrediction->GetRoleColor() : FColor(128, 128, 128, 255)).WithAlpha(IsCatchingUp ? 100 : 255);

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
void UCogSampleProjectileComponent::Catchup(float CatchupDuration)
{
    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Projectile:%s | Role:%s | CatchupDuration:%dms"), *GetNameSafe(GetOwner()), *GetRoleName(), (int32)(CatchupDuration * 1000));

    IsCatchingUp = true;
    TickComponent(CatchupDuration, LEVELTICK_All, nullptr);
    IsCatchingUp = false;

#if ENABLE_COG
    DrawDebugCurrentState(FColor::Red);
#endif //ENABLE_COG
}


//--------------------------------------------------------------------------------------------------------------------------
#if ENABLE_COG

void UCogSampleProjectileComponent::DrawDebugCurrentState(const FColor& Color, bool DrawVelocity)
{
    if (FCogDebugLog::IsLogCategoryActive(LogCogProjectile))
    {
        float CollisionRadius = Collision != nullptr ? Collision->GetScaledSphereRadius() : 0.0f;
        float AssistanceRadius = AssistanceOverlap != nullptr ? AssistanceOverlap->GetScaledSphereRadius() : 0.0f;
        float DebugRadius = FMath::Max(CollisionRadius, AssistanceRadius);
        FCogDebugDraw::Sphere(LogCogProjectile, GetOwner(), GetOwner()->GetActorLocation(), DebugRadius, Color, true, 0);

        if (DrawVelocity)
        {
            FCogDebugDraw::Arrow(LogCogProjectile, GetOwner(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + Velocity * 0.1f, Color, true, 0);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::DrawDebugHitResult(const FHitResult& HitResult)
{
    FCogDebugDraw::Arrow(LogCogProjectile, GetOwner(), HitResult.Location, HitResult.Location + HitResult.Normal * 50.0f, FColor::Red, true, 0);
    FCogDebugDraw::Box(LogCogProjectile, GetOwner(), HitResult.Location, FVector(0.0f, 5.0f, 5.0f), FRotationMatrix::MakeFromX(HitResult.Normal).ToQuat(), FColor::Red, true, 0);
    FCogDebugDraw::Arrow(LogCogProjectile, GetOwner(), HitResult.ImpactPoint, HitResult.ImpactPoint + HitResult.ImpactNormal * 50.0f, FColor::Yellow, true, 0);
    FCogDebugDraw::Box(LogCogProjectile, GetOwner(), HitResult.ImpactPoint, FVector(0.0f, 5.0f, 5.0f), FRotationMatrix::MakeFromX(HitResult.ImpactNormal).ToQuat(), FColor::Yellow, true, 0);
}


#endif //ENABLE_COG

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

    //if (GetOwner()->GetLocalRole() == ROLE_Authority)
    //{
    //    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Skipped:NotAuthority"));
    //    return false;
    //}

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
    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Projectile:%s | Role:%s | Other:%s | Comp:%s"),
        *GetNameSafe(GetOwner()),
        *GetRoleName(),
        *GetNameSafe(OtherActor),
        *GetNameSafe(OtherComp));

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
    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Projectile:%s | Role:%s | Other:%s | Comp:%s"),
        *GetNameSafe(GetOwner()),
        *GetRoleName(),
        *GetNameSafe(OtherActor),
        *GetNameSafe(OtherComp));

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
    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Projectile:%s | Role:%s | Other:%s | Comp:%s | Bone:%s"),
        *GetNameSafe(GetOwner()),
        *GetRoleName(),
        *GetNameSafe(HitResult.GetActor()),
        *GetNameSafe(HitResult.GetComponent()),
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
void UCogSampleProjectileComponent::StopSimulating(const FHitResult& HitResult)
{
    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Projectile:%s | Role:%s | Other:%s | Comp:%s | Bone:%s"),
        *GetNameSafe(GetOwner()),
        *GetRoleName(),
        *GetNameSafe(HitResult.GetActor()),
        *GetNameSafe(HitResult.GetComponent()),
        *HitResult.BoneName.ToString());

    Super::StopSimulating(HitResult);
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
void UCogSampleProjectileComponent::Hit_Implementation(const FHitResult& HitResult, FCogSampleHitConsequence& HitConsequence)
{
#if ENABLE_COG
    DrawDebugCurrentState(FColor::White, false);
    DrawDebugHitResult(HitResult);
#endif //ENABLE_COG

    if (LocalPlayerController == nullptr)
    {
        return;
    }

    LocalPlayerController->Server_ProjectileHit(this, HitResult);

    Server_Hit(HitResult);
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

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleProjectileComponent::Server_Hit(const FHitResult& HitResult)
{
    COG_LOG_OBJECT(LogCogProjectile, ELogVerbosity::Verbose, Creator.Get(), TEXT("Projectile:%s | Role:%s | Other:%s | Comp:%s | Bone:%s"),
        *GetNameSafe(GetOwner()),
        *GetRoleName(),
        *GetNameSafe(HitResult.GetActor()),
        *GetNameSafe(HitResult.GetComponent()),
        *HitResult.BoneName.ToString());

#if ENABLE_COG
    DrawDebugCurrentState(FColor::White, false);
    DrawDebugHitResult(HitResult);
#endif //ENABLE_COG

    AActor* HitActor = HitResult.GetActor();

    UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
    if (TargetAbilitySystem == nullptr)
    {
        return;
    }

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

            Spec->GetContext().AddHitResult(HitResult, true);
            TargetAbilitySystem->ApplyGameplayEffectSpecToSelf(*Spec);
        }
    }
}