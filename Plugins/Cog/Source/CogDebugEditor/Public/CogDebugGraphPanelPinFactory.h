#pragma once

#include "CoreMinimal.h"
#include "CogDebugGraphPanelPinFactory.h"
#include "CogCommonLog.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"
#include "SCogDebugLogCategoryGraphPin.h"
#include "SGraphPin.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class FCogGraphPanelPinFactory : public FGraphPanelPinFactory
{
    virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override
    {
        if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && InPin->PinType.PinSubCategoryObject == FCogLogCategory::StaticStruct())
        {
            return SNew(SCogLogCategoryGraphPin, InPin);
        }
        return nullptr;
    }
};
