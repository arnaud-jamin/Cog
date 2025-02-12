#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"

class UPlayerInput;

class COG_API FCogWindow_Layouts : public FCogWindow
{
    typedef FCogWindow Super;

public:

    FCogWindow_Layouts();
    
protected: 

    virtual void RenderContent() override;

    virtual void RenderLoadLayoutMenuItem(const UPlayerInput* PlayerInput, int LayoutIndex);

    virtual void RenderSaveLayoutMenuItem(const UPlayerInput* PlayerInput, int LayoutIndex);
};
