#pragma once

#include "CoreMinimal.h"
#include "CogSampleDefines.h"
#include "GameFramework/GameStateBase.h"
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

#if ENABLE_COG

    void InitializeCog();

    void RegisterCommand(const TCHAR* Name, const TCHAR* Help, const FConsoleCommandWithArgsDelegate& Command);

    void CogToggleInput(const TArray<FString>& Args);

    TObjectPtr<UCogWindowManager> CogWindowManager = nullptr;

#endif //ENABLE_COG
};
