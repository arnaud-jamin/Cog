#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CogDefines.h"
#include "CogSampleGameState.generated.h"

class UCogWindowManager;

UCLASS()
class ACogSampleGameState : public AGameStateBase
{
	GENERATED_BODY()

    ACogSampleGameState(const FObjectInitializer& ObjectInitializer);
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaSeconds) override;

private:

    UPROPERTY()
    TObjectPtr<UObject> CogWindowManagerRef = nullptr;

#if USE_COG
    void InitializeCog();
    void TickCog(float DeltaTime);
    void RenderCog(float DeltaTime);

    TObjectPtr<UCogWindowManager> CogWindowManager = nullptr;
#endif //USE_COG
};
