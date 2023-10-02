#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Collisions.generated.h"

class UCogEngineDataAsset_Collisions;

UCLASS(Config = Cog)
class COGENGINE_API UCogEngineWindow_Collisions : public UCogWindow
{
    GENERATED_BODY()

public:
    UCogEngineWindow_Collisions();

    virtual void RenderContent() override;

    void SetCollisionsAsset(const UCogEngineDataAsset_Collisions* Asset);

private:

    struct FChannel
    {
        bool IsValid = false;
        FColor Color;
    };

    TWeakObjectPtr<UCogEngineDataAsset_Collisions> CollisionsAsset;

    FChannel Channels[ECC_MAX];

    UPROPERTY(Config)
    int32 ObjectTypesToQuery;

    UPROPERTY(Config)
    int32 ProfileIndex = 0;
};
