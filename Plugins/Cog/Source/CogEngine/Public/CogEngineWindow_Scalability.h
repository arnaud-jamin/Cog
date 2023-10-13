#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_Scalability.generated.h"

UCLASS()
class COGENGINE_API UCogEngineWindow_Scalability : public UCogWindow
{
    GENERATED_BODY()

public:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;
};
