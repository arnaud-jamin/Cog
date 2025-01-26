#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "CogCommonPossessorInterface.h"
#include "GameFramework/PlayerController.h"
#include "CogSamplePlayerController.generated.h"

class ACogSampleCharacter;
class UCogSampleSpawnPredictionComponent;
class UCogSampleTargetAcquisition;

//--------------------------------------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCogSampleTargetChangedEventDelegate, ACogSamplePlayerController*, Controller, AActor*, NewTarget, AActor*, OldTarget);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCogSampleControlledCharacterChangedEventDelegate, ACogSamplePlayerController*, Controller, ACogSampleCharacter*, NewCharacter, ACogSampleCharacter*, OldCharacter);

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(config=Game)
class ACogSamplePlayerController 
    : public APlayerController
    , public ICogCommonPossessorInterface
{
	GENERATED_BODY()

public:
    
    static const ACogSamplePlayerController* GetFirstLocalPlayerControllerConst(const UObject* WorldContextObject);

    static ACogSamplePlayerController* GetFirstLocalPlayerController(const UObject* WorldContextObject);

    ACogSamplePlayerController();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    float GetClientLag() const;

    //----------------------------------------------------------------------------------------------------------------------
    // Possession
    //----------------------------------------------------------------------------------------------------------------------
    UFUNCTION(BlueprintCallable)
    virtual void SetPossession(APawn* NewPawn) override;

    UFUNCTION(BlueprintCallable)
    virtual void ResetPossession() override;

    virtual void OnPossess(APawn* NewPawn) override;

    virtual void AcknowledgePossession(APawn* NewPawn) override;

    UPROPERTY(BlueprintAssignable)
    FCogSampleControlledCharacterChangedEventDelegate OnControlledCharacterChanged;

    //----------------------------------------------------------------------------------------------------------------------
    // Targeting
    //----------------------------------------------------------------------------------------------------------------------
    void SetTarget(AActor* Value);

    AActor* GetTarget() const { return Target.Get(); }

    UPROPERTY(BlueprintAssignable)
    FCogSampleTargetChangedEventDelegate OnTargetChanged;

    //----------------------------------------------------------------------------------------------------------------------
    // Spawn Predictions
    //----------------------------------------------------------------------------------------------------------------------
    UPROPERTY()
    TArray<UCogSampleSpawnPredictionComponent*> SpawnPredictions;

    //----------------------------------------------------------------------------------------------------------------------
    // Projectile Hit
    //----------------------------------------------------------------------------------------------------------------------
    UFUNCTION(Reliable, Server)
    void Server_ProjectileHit(UCogSampleProjectileComponent* Projectile, const FHitResult& HitResult);

private:

    //----------------------------------------------------------------------------------------------------------------------
    // Possession
    //----------------------------------------------------------------------------------------------------------------------

    UPROPERTY(BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
    TWeakObjectPtr<ACogSampleCharacter> PossessedCharacter = nullptr;

    UPROPERTY(BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
    TWeakObjectPtr<ACogSampleCharacter> InitialPossessedCharacter = nullptr;

    //----------------------------------------------------------------------------------------------------------------------
    // Targeting
    //----------------------------------------------------------------------------------------------------------------------

    virtual void TickTargeting(float DeltaSeconds);

    UFUNCTION(Reliable, Server)
    void Server_SetTarget(AActor* Value);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TargetAcquisition, meta = (AllowPrivateAccess = "true"))
    UCogSampleTargetAcquisition* TargetAcquisition = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = TargetAcquisition, meta = (AllowPrivateAccess = "true"))
    TWeakObjectPtr<AActor> Target = nullptr;
};

