#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Pools.generated.h"

class UCogAbilityDataAsset;
class UAbilitySystemComponent;
struct FCogAbilityPool;

UCLASS()
class COGABILITY_API UCogAbilityWindow_Pools : public UCogWindow
{
    GENERATED_BODY()

public:
    
    const UCogAbilityDataAsset* GetAsset() const { return Asset.Get(); }

    void SetAsset(const UCogAbilityDataAsset* Value) { Asset  = Value; }

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void DrawPool(const UAbilitySystemComponent* AbilitySystemComponent, const FCogAbilityPool& Pool);

    UPROPERTY()
    TObjectPtr<const UCogAbilityDataAsset> Asset = nullptr;
};
