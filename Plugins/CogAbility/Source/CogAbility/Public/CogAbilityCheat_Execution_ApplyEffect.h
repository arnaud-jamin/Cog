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
    virtual void Execute_Implementation(const UObject* WorldContextObject, const AActor* Instigator, const TArray<AActor*>& Targets) const override;

    virtual ECogEngineCheat_ActiveState IsActiveOnTargets_Implementation(const UObject* WorldContextObject, const TArray<AActor*>& Targets) const override;

    virtual bool GetColor(const FCogWindow& InCallingWindow, FLinearColor& OutColor) const override;

	UPROPERTY(EditAnywhere, Category = "Cog")
    TSubclassOf<UGameplayEffect> Effect;
};
