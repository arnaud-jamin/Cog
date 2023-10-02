#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogInjectActionInfo.h"
#include "CogInputWindow_Actions.generated.h"

class UInputAction;
class UCogInputDataAsset_Actions;

UCLASS()
class COGINPUT_API UCogInputWindow_Actions : public UCogWindow
{
    GENERATED_BODY()

public:
    UCogInputWindow_Actions();

    virtual void PreRender(ImGuiWindowFlags& WindowFlags) override;
    virtual void RenderContent() override;
    virtual void RenderTick(float DeltaSeconds) override;

    TWeakObjectPtr<UCogInputDataAsset_Actions> ActionsAsset;

private:

    float RepeatPeriod = 0.5f;
    float RepeatTime = 0.0f;
    TArray<FCogInjectActionInfo> Actions;
};
