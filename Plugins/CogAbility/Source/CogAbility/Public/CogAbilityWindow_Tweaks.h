#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"

class ACogAbilityReplicator;
class UCogAbilityDataAsset;

//--------------------------------------------------------------------------------------------------------------------------
class COGABILITY_API FCogAbilityWindow_Tweaks : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void DrawTweak(ACogAbilityReplicator* Replicator, int32 TweakIndex, int32 TweakCategoryIndex);

private:

    TWeakObjectPtr<const UCogAbilityDataAsset> Asset;
};
