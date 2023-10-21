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

    UCogAbilityWindow_Tweaks();

    const UCogAbilityDataAsset* GetAsset() const { return Asset.Get(); }

    void SetAsset(const UCogAbilityDataAsset* Value) { Asset = Value; }

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void DrawTweak(ACogAbilityReplicator* Replicator, int32 TweakIndex, int32 TweakCategoryIndex);

private:

    UPROPERTY()
    TObjectPtr<const UCogAbilityDataAsset> Asset = nullptr;
};
