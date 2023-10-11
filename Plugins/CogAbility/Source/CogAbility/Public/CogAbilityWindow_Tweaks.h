#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Tweaks.generated.h"

class ACogAbilityReplicator;
class UCogAbilityDataAsset;

UCLASS()
class COGABILITY_API UCogAbilityWindow_Tweaks : public UCogWindow
{
    GENERATED_BODY()

public:
        TWeakObjectPtr<const UCogAbilityDataAsset> Asset;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void DrawTweak(ACogAbilityReplicator* Replicator, int32 TweakIndex, int32 TweakCategoryIndex);

private:
};
