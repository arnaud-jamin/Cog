#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Tweaks.generated.h"

class UCogAbilityDataAsset_Tweaks;

UCLASS()
class COGABILITY_API UCogAbilityWindow_Tweaks : public UCogWindow
{
    GENERATED_BODY()

public:
        TWeakObjectPtr<UCogAbilityDataAsset_Tweaks> TweaksAsset;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void DrawTweak(ACogAbilityReplicator* Replicator, int32 TweakIndex, int32 TweakCategoryIndex);

private:
};
