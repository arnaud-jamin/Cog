#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"

class UCogAbilityDataAsset;
class UAbilitySystemComponent;
struct FCogAbilityPool;

//--------------------------------------------------------------------------------------------------------------------------
class COGABILITY_API FCogAbilityWindow_Pools : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void DrawPool(const UAbilitySystemComponent* AbilitySystemComponent, const FCogAbilityPool& Pool);

    TObjectPtr<const UCogAbilityDataAsset> Asset = nullptr;
};
