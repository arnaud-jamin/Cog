#pragma once

#include "CoreMinimal.h"
#include "CogCommon.h"
#include "GameFramework/GameStateBase.h"
#include "CogSampleGameState.generated.h"

class UCogSubsystem;
class UCogSampleAbilitySystemComponent;

UCLASS()
class ACogSampleGameState : public AGameStateBase
{
    GENERATED_BODY()

    ACogSampleGameState(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void Tick(float DeltaSeconds) override;

private:

    //---------------------------------------------------------------------------------
    // The game state has an AbilitySystemComponent to serve as the default instigator 
    // for effects that are not applied by characters.
    //---------------------------------------------------------------------------------
    UPROPERTY()
    UCogSampleAbilitySystemComponent* AbilitySystemComponent = nullptr;

protected:
    UPROPERTY(Replicated)
    float _ServerFramerateRaw = 0.0f;

#if ENABLE_COG
    
    float _ClientFramerateSmooth = 0.0f;

    float _ServerFramerateSmooth = 0.0f;

#endif //ENABLE_COG
};
