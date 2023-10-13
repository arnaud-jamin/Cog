#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogWindow_Spacing.generated.h"

UCLASS()
class COGWINDOW_API UCogWindow_Spacing : public UCogWindow
{
    GENERATED_BODY()

public:
    
protected: 

    virtual void PreRender(ImGuiWindowFlags& WindowFlags) override;
    
    virtual void PostRender() override;

private:
    
};
