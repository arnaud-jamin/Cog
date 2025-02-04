#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "Engine/EngineTypes.h"
#include "CogEngineWindow_CollisionViewer.generated.h"

class UCogEngineConfig_CollisionViewer;
class UCogEngineDataAsset;

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_CollisionViewer : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    struct FChannel
    {
        bool IsValid = false;
        FColor Color;
    };

    TObjectPtr<UCogEngineConfig_CollisionViewer> Config = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_CollisionViewer : public UCogCommonConfig
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
    }
};