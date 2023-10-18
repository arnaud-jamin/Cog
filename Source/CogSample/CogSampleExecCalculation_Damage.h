#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CogSampleExecCalculation_Damage.generated.h"

class UCogSampleAbilitySystemComponent;
struct FHitResult;
struct FGameplayEffectContextHandle;

UCLASS()
class UCogSampleExecCalculation_Damage : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UCogSampleExecCalculation_Damage();

    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};

UCLASS(meta = (ScriptName = "CogSampleFunctionLibrary_Damage"))
class COGSAMPLE_API UCogSampleFunctionLibrary_Damage : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable)
    static void ExecuteDamageGameplayCue(
        UCogSampleAbilitySystemComponent* TargetAbilitySystem,
        const FHitResult& HitResult,
        float HealthDamage,
        const FGameplayEffectContextHandle& EffectContext,
        bool IsPredicted);

};
