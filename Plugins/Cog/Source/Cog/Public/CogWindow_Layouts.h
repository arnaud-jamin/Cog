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

    virtual void RenderLoadLayoutMenuItem(int InLayoutIndex, const FInputChord& InInputChord);

    virtual void RenderSaveLayoutMenuItem(int InLayoutIndex, const FInputChord& InInputChord);
};
