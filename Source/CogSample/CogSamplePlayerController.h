#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "CogSampleDamageEvent.h"
#include "GameFramework/PlayerController.h"
#include "CogSamplePlayerController.generated.h"

class UCogSampleTargetAcquisition;

//--------------------------------------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCogSampleTargetChangedEventDelegate, ACogSamplePlayerController*, Controller, AActor*, NewTarget, AActor*, OldTarget);

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(config=Game)
class ACogSamplePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    
    ACogSamplePlayerController();

    virtual void BeginPlay() override;

    virtual void AcknowledgePossession(APawn* P);

    virtual void Tick(float DeltaSeconds);

    void SetTarget(AActor* Value);

    AActor* GetTarget() const { return Target.Get(); }

    UPROPERTY(BlueprintAssignable)
    FCogSampleTargetChangedEventDelegate OnTargetChanged;

    UPROPERTY(BlueprintAssignable)
    FCogSampleDamageEventDelegate OnPawnDealtDamage;

private:

    UFUNCTION(Reliable, Server)
    void Server_SetTarget(AActor* Value);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TargetAcquisition, meta = (AllowPrivateAccess = "true"))
    UCogSampleTargetAcquisition* TargetAcquisition = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = TargetAcquisition, meta = (AllowPrivateAccess = "true"))
    TWeakObjectPtr<AActor> Target = nullptr;
};

