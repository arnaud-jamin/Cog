#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CogSampleSpawnPredictionComponent.h"
#include "GameplayEffect.h"
#include "CogSampleProjectileComponent.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
// FCogSampleProjectileEffectConfig
//--------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FCogSampleProjectileEffectConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/CogSample.ECogSampleAllegianceFilter"))
    int32 Allegiance = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool AffectDead = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayEffect>> Effects;
};

//--------------------------------------------------------------------------------------------------------------------------
// FCogSampleHitConsequence
//--------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FCogSampleHitConsequence
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadWrite)
    bool Stop = false;

    UPROPERTY(BlueprintReadWrite)
    bool AttachToComponent = false;

    UPROPERTY(BlueprintReadWrite)
    bool HideOnStop = false;
};

//--------------------------------------------------------------------------------------------------------------------------
// UCogSampleProjectileComponent
//--------------------------------------------------------------------------------------------------------------------------
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class UCogSampleProjectileComponent : public UProjectileMovementComponent
{
    GENERATED_BODY()

public:
    
    virtual void BeginPlay() override;

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

    UFUNCTION(BlueprintCallable)
    void ClearHitActors();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    TArray<FCogSampleProjectileEffectConfig> Effects;

    UPROPERTY(Transient, BlueprintReadWrite, Category = "Internal")
    TArray<FGameplayEffectSpecHandle> BakedEffects;

protected:

    virtual FString GetRoleName() const;

    //virtual void CatchupReplicatedActor(float CatchupDuration);

    virtual bool ShouldProcessOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, bool RequireValidActor);

    virtual void TryHit(const FHitResult& Hit);

    UFUNCTION()
    virtual void OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool IsFromSweep, const FHitResult& SweepHit);

    UFUNCTION()
    virtual void OnAssistanceOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool IsFromSweep, const FHitResult& SweepHit);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    bool ShouldHit(const FHitResult& hit);

    /** Blueprint hook called when projectile hits something */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void Hit(const FHitResult& Hit, FCogSampleHitConsequence& HitConsequence);

    void RegisterAllEffects();

    UPROPERTY(EditAnywhere)
    FComponentReference CollisionReference;

    UPROPERTY(EditAnywhere)
    FComponentReference OverlapReference;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    bool CanHitCreator = false;

    /** Sphere collision component */
    UPROPERTY()
    TObjectPtr<USphereComponent> Collision = nullptr;

    /** Overlap collision component */
    UPROPERTY()
    TObjectPtr<USphereComponent> AssistanceOverlap = nullptr;

    UPROPERTY()
    TWeakObjectPtr<AActor> Creator = nullptr;

    UPROPERTY()
    TWeakObjectPtr<UCogSampleSpawnPredictionComponent> SpawnPrediction = nullptr;

    /** Re-entrancy guard */
    bool IsAlreadyProcessingAnOverlap = false;

    /** The actors that were hit with this projectile. Used to prevent touching the same actor multiple times */
    TSet<AActor*> HitActors;

    TMap<TSubclassOf<UGameplayEffect>, FGameplayEffectSpecHandle> EffectsMap;

#if ENABLE_COG
    FVector LastDebugLocation = FVector::ZeroVector;
#endif //ENABLE_COG
};