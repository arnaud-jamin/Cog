#pragma once

#include "CoreMinimal.h"
#include "CogCommon.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "GameplayPrediction.h"
#include "CogSampleSpawnPredictionComponent.generated.h"

class UGameplayAbility;
class ACogSamplePlayerController;

//--------------------------------------------------------------------------------------------------------------------------
// FCogSamplePredictedActorKey
//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogSampleSpawnPredictionKey
{
    GENERATED_BODY()

    UPROPERTY()
    FName Creator;

    UPROPERTY()
    FName Ability;

    UPROPERTY()
    FPredictionKey PredictionKey;

    UPROPERTY()
    int32 InstanceIndex = 0;

    UPROPERTY()
    float GameTime = 0;

    FString ToString() const;

    bool operator==(const FCogSampleSpawnPredictionKey& other) const;

    bool operator!=(const FCogSampleSpawnPredictionKey& other) const;

    static FCogSampleSpawnPredictionKey MakeFromAbility(const UGameplayAbility& InAbility, int32 InInstanceIndex);
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogSampleSpawnPredictionRole : uint8
{
    Server,
    Predicted,
    Replicated,
    Remote,
};

//--------------------------------------------------------------------------------------------------------------------------
// UCogSamplePredictedActorComponent
//--------------------------------------------------------------------------------------------------------------------------
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class UCogSampleSpawnPredictionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    
    UCogSampleSpawnPredictionComponent(const class FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

#if ENABLE_COG
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#endif //ENABLE_COG

    virtual TWeakObjectPtr<AActor> GetCreator() const { return Creator; }

    virtual void SetCreator(TWeakObjectPtr<AActor> Value) { Creator = Value; }

    virtual const FCogSampleSpawnPredictionKey& GetSpawnPredictionKey() const { return PredictedActorKey; }

    virtual void SetSpawnPredictionKey(const FCogSampleSpawnPredictionKey& Value) { PredictedActorKey = Value; }

    virtual void SetSpawnTransform(FTransform Transform) { SpawnTransform = Transform; }
    
    virtual FTransform GetSpawnTransform() { return SpawnTransform; }

    virtual ECogSampleSpawnPredictionRole GetRole() const { return Role; }

    virtual void SetRole(ECogSampleSpawnPredictionRole Value) { Role = Value; }

    UCogSampleSpawnPredictionComponent* GetPredicted() const { return PredictedSpawn; }

    UCogSampleSpawnPredictionComponent* GetReplicated() const { return ReplicatedActor; }

    virtual void ReconcileReplicatedWithPredicted();

    FString GetRoleName() const;

    FColor GetRoleColor() const;

protected:

    virtual int32 FindSpawnPredictionIndex();
    
    virtual void SyncReplicatedWithPredicted(UCogSampleSpawnPredictionComponent* PredictedSpawn);

    void CleanInvalidPredictedActors();

    void SetVisibility(bool Value);

    UFUNCTION()
    void OnRep_PredictedActorKey();

    UPROPERTY(Replicated)
    TWeakObjectPtr<AActor> Creator = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadonly)
    bool DestroyReplicatedActor = false;

    UPROPERTY(EditAnywhere, BlueprintReadonly)
    bool HideReplicatedActor = true;

    UPROPERTY(BlueprintReadonly)
    UCogSampleSpawnPredictionComponent* PredictedSpawn = nullptr;

    UPROPERTY(BlueprintReadonly)
    UCogSampleSpawnPredictionComponent* ReplicatedActor = nullptr;

    UPROPERTY(BlueprintReadonly)
    ECogSampleSpawnPredictionRole Role = ECogSampleSpawnPredictionRole::Replicated;

    UPROPERTY(BlueprintReadonly)
    TWeakObjectPtr<ACogSamplePlayerController> PlayerController = nullptr;

    UPROPERTY(ReplicatedUsing = OnRep_PredictedActorKey)
    FCogSampleSpawnPredictionKey PredictedActorKey;

    FTransform SpawnTransform;

#if ENABLE_COG
    FVector LastDebugLocation = FVector::ZeroVector;
#endif //ENABLE_COG
};

