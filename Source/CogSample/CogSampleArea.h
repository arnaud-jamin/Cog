#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "GameplayEffect.h"

#include "CogSampleArea.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
// ECogSampleAreaEventType
//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ECogSampleAreaEventType : uint8
{
    None        = 0 UMETA(Hidden),
    OnStart     = 1 << 0,
    OnEnd       = 1 << 1,
    OnTick      = 1 << 2,
    OnInside    = 1 << 3,
    OnEnter     = 1 << 4,
    OnExit      = 1 << 5,
};
ENUM_CLASS_FLAGS(ECogSampleAreaEventType);

//--------------------------------------------------------------------------------------------------------------------------
// FCogSampleAreaEffectConfig
//--------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct COGSAMPLE_API FCogSampleAreaEffectConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECogSampleAreaEventType Event = ECogSampleAreaEventType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/CogSample.ECogSampleAllegianceFilter"))
    int32 Allegiance = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayEffect>> Effects;
};

//--------------------------------------------------------------------------------------------------------------------------
// ACogSampleArea
//--------------------------------------------------------------------------------------------------------------------------
UCLASS()
class COGSAMPLE_API ACogSampleArea : public AActor
{
    GENERATED_UCLASS_BODY()
    
public:	
   
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|General", meta = (ExposeOnSpawn = true))
    TObjectPtr<AActor> ParentActor = nullptr;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Area|General", meta = (ExposeOnSpawn = true))
    FVector HalfExtent = FVector(100.f, 100.f, 100.f);

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Area|General", meta = (ExposeOnSpawn = true))
    int32 Level = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|General", meta = (ExposeOnSpawn = true))
    int32 Team = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|General", meta = (ExposeOnSpawn = true))
    bool IsInstant = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|General", meta = (ExposeOnSpawn = true))
    float TickRate = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|General", meta = (ExposeOnSpawn = true))
    float InitialTickDelay = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|General")
    bool ApplyTickEffectOnEnter = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|General")
    bool ApplyTickEffectOnExit = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|General")
    bool OnlyDetectOnAuthority = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|General")
    bool AffectDead = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|Effects", meta = (ExposeOnSpawn = true))
    TArray<FCogSampleAreaEffectConfig> Effects;

    UPROPERTY(Transient, BlueprintReadWrite, Category = "Internal", meta = (ExposeOnSpawn = true))
    TArray<FGameplayEffectSpecHandle> BakedEffects;

protected:

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Area")
    void OnTickEffect();
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Area")
    void OnActorEntered(AActor* OverlappedActor, AActor* EnteringActor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Area")
    void OnActorExited(AActor* OverlappedActor, AActor* OtherExitingActorActor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Area")
    void ApplyInstantEffects(AActor* HitActor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Area")
    void ApplyTickEffects(AActor* HitActor);

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Area")
    bool CanBeAffected(AActor* OtherActor) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Area")
    void AffectEnteringActor(AActor* EnteringActor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Area")
    void AffectExitingActor(AActor* ExitingActor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Area")
    void ApplyInsideEffects(AActor* HitActor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Area")
    void RemoveInsideEffects(AActor* HitActor);

    UFUNCTION(BlueprintPure, Category = "Area")
    bool CanPerformDetection() const;

    UFUNCTION(BlueprintPure, Category = "Area")
    bool IsAliveOfAffectDead(AActor* OtherActor) const;

    UFUNCTION(BlueprintPure, Category = "Area")
    bool IsAlreadyAffected(AActor* OtherActor) const;

    UFUNCTION(BlueprintPure, Category = "Area")
    AActor* GetInstigatorActor() const;

    UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Area")
    bool MakeAreaOutgoingEffectSpecs(TSubclassOf<UGameplayEffect> EffectClass, FGameplayEffectSpecHandle& EffectSpecHandle) const;
    
    void RegisterAllEffects();

    void ApplyEffectsOnActors(const TArray<AActor*>& HitActors, int32 EventTypeFilter);

    UPROPERTY(BlueprintReadOnly, Transient)
    bool IsAtStart = true;

    bool IsAtEnd = false;

    UPROPERTY(BlueprintReadOnly, Transient)
    TArray<TObjectPtr<AActor>> InsideActors;

    TMap<TSubclassOf<UGameplayEffect>, FGameplayEffectSpecHandle> EffectsMap;

    TMap<AActor*, TArray<FActiveGameplayEffectHandle>> InsideEffects;

    FTimerHandle TickTimerHandle;
};
