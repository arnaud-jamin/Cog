#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "CogSampleBTTask_BlueprintBase.generated.h"

UCLASS(Abstract, Blueprintable)
class UCogSampleBTTask_BlueprintBase : public UBTTask_BlueprintBase
{
    GENERATED_BODY()

public:

	virtual FString GetStaticDescription() const override;

    UFUNCTION(BlueprintImplementableEvent, Category = AI)
    FString GetCustomStaticDescription() const;
};