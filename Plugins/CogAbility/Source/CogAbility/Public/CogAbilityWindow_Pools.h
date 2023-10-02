#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Pools.generated.h"

class UCogAbilityDataAsset_Pools;

UCLASS()
class COGABILITY_API UCogAbilityWindow_Pools : public UCogWindow
{
    GENERATED_BODY()

public:
    UCogAbilityWindow_Pools();
    virtual void RenderContent() override;

    TWeakObjectPtr<UCogAbilityDataAsset_Pools> PoolsAsset;
};
