#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogImGuiKeyInfo.h"
#include "CogWindow_Settings.generated.h"


UCLASS(Config = Cog)
class COGWINDOW_API UCogWindow_Settings : public UCogWindow
{
    GENERATED_BODY()

public:
    
    UCogWindow_Settings();

protected: 

    virtual void PostInitProperties() override;

    virtual void RenderContent() override;

private:

};
