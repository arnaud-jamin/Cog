#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "CogSamplePlayerController.generated.h"

class UCogSampleTargetAcquisition;
class ACogSampleCharacter;

//--------------------------------------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCogSampleTargetChangedEventDelegate, ACogSamplePlayerController*, Controller, AActor*, NewTarget, AActor*, OldTarget);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCogSampleControlledCharacterChangedEventDelegate, ACogSamplePlayerController*, Controller, ACogSampleCharacter*, NewCharacter, ACogSampleCharacter*, OldCharacter);

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(config=Game)
class ACogSamplePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    
    ACogSamplePlayerController();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    //----------------------------------------------------------------------------------------------------------------------
    // Control
    //----------------------------------------------------------------------------------------------------------------------
    virtual void OnPossess(APawn* NewPawn) override;

    virtual void AcknowledgePossession(APawn* NewPawn) override;

    UFUNCTION(BlueprintCallable)
    void ControlCharacter(ACogSampleCharacter* NewCharacter);

    UFUNCTION(BlueprintCallable)
    void ResetControlledPawn();

    UPROPERTY(BlueprintAssignable)
    FCogSampleControlledCharacterChangedEventDelegate OnControlledCharacterChanged;

    //----------------------------------------------------------------------------------------------------------------------
    // Targeting
    //----------------------------------------------------------------------------------------------------------------------
    void SetTarget(AActor* Value);

    AActor* GetTarget() const { return Target.Get(); }


    UPROPERTY(BlueprintAssignable)
    FCogSampleTargetChangedEventDelegate OnTargetChanged;

private:

    //----------------------------------------------------------------------------------------------------------------------
    // Control
    //----------------------------------------------------------------------------------------------------------------------

    UPROPERTY(BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
    TWeakObjectPtr<ACogSampleCharacter> ControlledCharacter = nullptr;

    UPROPERTY(BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
    TWeakObjectPtr<ACogSampleCharacter> InitialControlledCharacter = nullptr;

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

