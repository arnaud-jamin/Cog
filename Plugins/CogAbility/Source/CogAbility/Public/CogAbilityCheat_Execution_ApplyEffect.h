#pragma once

#include "CoreMinimal.h"
#include "CogEngineDataAsset.h"
#include "GameplayEffect.h"
#include "CogAbilityCheat_Execution_ApplyEffect.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(DisplayName = "Apply Effect")
class COGABILITY_API UCogAbilityCheat_Execution_ApplyEffect
    : public UCogEngineCheat_Execution
{
    GENERATED_BODY()

public:
    void Execute_Implementation(const AActor* Instigator, const TArray<AActor*>& Targets) const override;

    ECogEngineCheat_ActiveState IsActiveOnTargets_Implementation(const TArray<AActor*>& Targets) const override;

    virtual bool GetColor(const FCogWindow& InCallingWindow, FLinearColor& OutColor) const override;

	UPROPERTY(EditAnywhere)
    TSubclassOf<UGameplayEffect> Effect;
};
