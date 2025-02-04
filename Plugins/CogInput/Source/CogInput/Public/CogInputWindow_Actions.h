#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogInputActionInfo.h"
#include "CogWindow.h"
#include "CogInputWindow_Actions.generated.h"

class UInputAction;
class UCogInputConfig_Actions;

//--------------------------------------------------------------------------------------------------------------------------
class COGINPUT_API FCogInputWindow_Actions : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void RenderTick(float DeltaSeconds) override;

    virtual void DrawAxis(const char* Format, const char* ActionName, float CurrentValue, float* InjectValue);

private:

    float RepeatTime = 0.0f;

    TObjectPtr<UCogInputConfig_Actions> Config = nullptr;

    TArray<FCogInputMappingContextInfo> Mappings;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogInputConfig_Actions : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    float RepeatPeriod = 0.5f;

    UPROPERTY(Config)
    bool ShowHeader = true;

    virtual void Reset() override
    {
        Super::Reset();

        RepeatPeriod = 0.5f;
        ShowHeader = true;
    }
};