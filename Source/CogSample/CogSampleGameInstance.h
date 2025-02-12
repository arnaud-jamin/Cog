#pragma once

#include "CoreMinimal.h"
#include "CogSampleGameInstance.generated.h"

class UCogSubsystem;
class UCogSampleAbilitySystemComponent;

UCLASS()
class UCogSampleGameInstance : public UGameInstance
{
    GENERATED_BODY()

    virtual void Init() override;
};
