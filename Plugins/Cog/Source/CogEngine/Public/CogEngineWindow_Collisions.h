#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogWindowConfig.h"
#include "CogEngineWindow_Collisions.generated.h"

class UCogEngineConfig_Collisions;
class UCogEngineDataAsset;

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_Collisions : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void ResetConfig() override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void SetAsset(const UCogEngineDataAsset* Value);

    struct FChannel
    {
        bool IsValid = false;
        FColor Color;
    };

    FChannel Channels[ECC_MAX];

    TObjectPtr<const UCogEngineDataAsset> Asset = nullptr;

    TObjectPtr<UCogEngineConfig_Collisions> Config = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_Collisions : public UCogWindowConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    int32 ObjectTypesToQuery = 0;

    UPROPERTY(Config)
    int32 ProfileIndex = 0;

    UPROPERTY(Config)
    int QueryType = 0;

    UPROPERTY(Config)
    float QueryDistance = 5000.0f;

    UPROPERTY(Config)
    float QueryThickness = 0.0f;

    UPROPERTY(Config)
    bool UseComplexCollisions = false;

    UPROPERTY(Config)
    bool ShowActorsNames = false;

    UPROPERTY(Config)
    bool ShowQuery = false;

    virtual void Reset() override
    {
        Super::Reset();

        ObjectTypesToQuery = 0;
        ProfileIndex = 0;
        QueryType = 0;
        QueryDistance = 5000.0f;
        QueryThickness = 0.0f;
        UseComplexCollisions = false;
        ShowActorsNames = false;
        ShowQuery = false;
    }
};