#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "CogSamplePlayerController.generated.h"

class UCogSampleTargetAcquisition;

UCLASS(config=Game)
class ACogSamplePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    
    ACogSamplePlayerController();

    virtual void BeginPlay() override;

    virtual void AcknowledgePossession(APawn* P);

    virtual void Tick(float DeltaSeconds);

private:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TargetAcquisition, meta = (AllowPrivateAccess = "true"))
    UCogSampleTargetAcquisition* TargetAcquisition = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = TargetAcquisition, meta = (AllowPrivateAccess = "true"))
    TSoftObjectPtr<AActor> Target = nullptr;
};

