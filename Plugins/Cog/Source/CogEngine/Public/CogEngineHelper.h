#pragma once

#include "CoreMinimal.h"
#include "CogEngineDataAsset.h"

class AActor;

class COGENGINE_API FCogEngineHelper
{
public:

    static void ActorContextMenu(AActor& Actor);

    static void RenderConfigureMessage(TWeakObjectPtr<const UCogEngineDataAsset> InAsset);
};
