#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"

class COGWINDOW_API FCogWindow_Spacing : public FCogWindow
{
    typedef FCogWindow Super;

protected: 

    virtual void PreBegin(ImGuiWindowFlags& WindowFlags) override;
    
    virtual void PostBegin() override;

};
