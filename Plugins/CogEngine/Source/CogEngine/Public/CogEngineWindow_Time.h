#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Time.generated.h"

UCLASS()
class COGENGINE_API UCogEngineWindow_Time : public UCogWindow
{
    GENERATED_BODY()

public:
    UCogEngineWindow_Time();

    virtual void RenderContent() override;

    TArray<float> TimingScales;

private:

};
