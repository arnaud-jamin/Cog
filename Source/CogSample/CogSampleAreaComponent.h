#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffect.h"
#include "CogSampleAreaComponent.generated.h"

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
struct FCogSampleAreaEffectConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECogSampleAreaEventType Event = ECogSampleAreaEventType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/CogSample.ECogSampleAllegianceFilter"))
    int32 Allegiance = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool AffectDead = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayEffect>> Effects;
};

//--------------------------------------------------------------------------------------------------------------------------
// ECogSampleAreaDurationType
//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogSampleAreaDurationType : uint8
{
    Instant,
    Infinite,
    HasDuration
};

//--------------------------------------------------------------------------------------------------------------------------
// ACogSampleArea
//--------------------------------------------------------------------------------------------------------------------------
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class UCogSampleAreaComponent : public UActorComponent
{
    GENERATED_UCLASS_BODY()
    
public:	

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void InitializeComponent() override;

    //virtual void PreInitializeComponents() override;

    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    FVector GetHalfExtent() const { return HalfExtent; }

    void SetHalfExtent(const FVector& Value);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
    ECogSampleAreaDurationType DurationType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area", meta = (EditConditionHides, EditCondition = "DurationType == ECogSampleAreaDurationType::HasDuration"))
    float Duration = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area", meta = (EditConditionHides, EditCondition = "DurationType != ECogSampleAreaDurationType::Instant"))
    float TickRate = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area", meta = (EditConditionHides, EditCondition = "DurationType != ECogSampleAreaDurationType::Instant"))
    float InitialTickDelay = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area", meta = (EditConditionHides, EditCondition = "DurationType != ECogSampleAreaDurationType::Instant"))
    bool ApplyTickEffectOnEnter = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area", meta = (EditConditionHides, EditCondition = "DurationType != ECogSampleAreaDurationType::Instant"))
    bool ApplyTickEffectOnExit = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
    bool OnlyDetectOnAuthority = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
    FName HalfExtentName = "HalfExtent";
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
    FName TickRateName = "TickRate";
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
    FName DurationName = "Duration";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
    TArray<FCogSampleAreaEffectConfig> Effects;

    UPROPERTY(Transient, BlueprintReadWrite, Category = "Internal")
    TArray<FGameplayEffectSpecHandle> BakedEffects;

protected:

    UFUNCTION(Category = "Area")
    void OnRep_HalfExtent();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Area")
    void OnTickEffect();
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Area")
    void OnDurationElapsed();

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
    bool IsAlreadyAffected(AActor* OtherActor) const;

    UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Area")
    bool MakeAreaOutgoingEffectSpecs(TSubclassOf<UGameplayEffect> EffectClass, FGameplayEffectSpecHandle& EffectSpecHandle) const;
    
    UPROPERTY(ReplicatedUsing = OnRep_HalfExtent, EditAnywhere, BlueprintReadWrite, Category = "Area")
    FVector HalfExtent = FVector(100.f, 100.f, 100.f);

    virtual void RegisterAllEffects();

    virtual void ApplyEffectsOnActors(const TArray<AActor*>& HitActors, int32 EventTypeFilter);

    virtual void RefreshOtherComponentsValues();

    UPROPERTY(BlueprintReadOnly, Transient)
    bool IsAtStart = true;

    bool IsAtEnd = false;

    UPROPERTY(BlueprintReadOnly, Transient)
    TArray<TObjectPtr<AActor>> InsideActors;

    TMap<TSubclassOf<UGameplayEffect>, FGameplayEffectSpecHandle> EffectsMap;

    TMap<AActor*, TArray<FActiveGameplayEffectHandle>> InsideEffects;

    FTimerHandle TickTimerHandle;

    FTimerHandle DurationTimerHandle;
};
