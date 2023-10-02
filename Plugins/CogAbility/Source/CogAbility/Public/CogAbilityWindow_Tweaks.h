#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Tweaks.generated.h"

class AActor;
class UCogAbilityDataAsset_Tweaks;
class ACogAbilityReplicator;

UCLASS()
class COGABILITY_API UCogAbilityWindow_Tweaks : public UCogWindow
{
    GENERATED_BODY()

public:
    UCogAbilityWindow_Tweaks();
    virtual void RenderContent() override;

    TWeakObjectPtr<UCogAbilityDataAsset_Tweaks> TweaksAsset;

private:
};
