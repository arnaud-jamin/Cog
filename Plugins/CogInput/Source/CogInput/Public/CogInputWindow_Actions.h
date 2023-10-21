#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogInjectActionInfo.h"
#include "CogInputWindow_Actions.generated.h"

class UInputAction;
class UCogInputDataAsset;

UCLASS(Config = Cog)
class COGINPUT_API UCogInputWindow_Actions : public UCogWindow
{
    GENERATED_BODY()

public:

    UCogInputWindow_Actions();

    const UCogInputDataAsset* GetAsset() const { return Asset.Get(); }

    void SetAsset(const UCogInputDataAsset* Value) { Asset = Value; }

protected:

    void RenderHelp();

    virtual void RenderContent() override;

    virtual void RenderTick(float DeltaSeconds) override;

    virtual void DrawAxis(const char* Format, const char* ActionName, float CurrentValue, float* InjectValue);

private:

    UPROPERTY(Config)
    float RepeatPeriod = 0.5f;

    float RepeatTime = 0.0f;

    UPROPERTY()
    TObjectPtr<const UCogInputDataAsset> Asset = nullptr;

    TArray<FCogInjectActionInfo> Actions;
};
