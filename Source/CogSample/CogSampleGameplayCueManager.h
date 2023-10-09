#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "CogSampleGameplayCueManager.generated.h"

UCLASS()
class UCogSampleGameplayCueManager : public UGameplayCueManager
{
    GENERATED_BODY()

    virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override;
};
