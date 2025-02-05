#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Cheats.generated.h"

class AActor;
class UCogAbilityConfig_Alignment;
class UCogAbilityConfig_Cheats;
class UCogAbilityDataAsset;
struct FCogAbilityCheat;

//--------------------------------------------------------------------------------------------------------------------------
class COGABILITY_API FCogAbilityWindow_Cheats : public FCogWindow
{
    typedef FCogWindow Super;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAbilityConfig_Cheats : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool bReapplyCheatsBetweenPlays = true;

    UPROPERTY(Config)
    bool bReapplyCheatsBetweenLaunches = true;

    UPROPERTY(Config)
    TArray<FString> AppliedCheats;

    virtual void Reset() override
    {
        Super::Reset();

        bReapplyCheatsBetweenPlays = true;
        bReapplyCheatsBetweenLaunches = true;
    }
};
