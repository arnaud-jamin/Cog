#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"

class COG_API FCogWindow_Spacing : public FCogWindow
{
    typedef FCogWindow Super;

public:
    
    FCogWindow_Spacing();

    virtual void PreBegin(ImGuiWindowFlags& WindowFlags) override;
    
    virtual void PostBegin() override;
};
