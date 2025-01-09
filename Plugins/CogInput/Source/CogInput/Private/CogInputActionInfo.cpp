#include "CogInputActionInfo.h"

#include "EnhancedInputSubsystems.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputActionInfo::Inject(UEnhancedInputLocalPlayerSubsystem& EnhancedInput, bool IsTimeToRepeat)
{
    if (Action == nullptr)
    {
        return;
    }

    switch (Action->ValueType)
    {
        case EInputActionValueType::Boolean:
        {
            if (bRepeat)
            {
                EnhancedInput.InjectInputForAction(Action, FInputActionValue(IsTimeToRepeat), {}, {});
            }
            else
            {
                EnhancedInput.InjectInputForAction(Action, FInputActionValue(bPressed), {}, {});
            }
            break;
        }

        case EInputActionValueType::Axis1D:
        {
            EnhancedInput.InjectInputForAction(Action, FInputActionValue(X), {}, {});
            break;
        }

        case EInputActionValueType::Axis2D:
        {
            EnhancedInput.InjectInputForAction(Action, FInputActionValue(FVector2D(X, Y)), {}, {});
            break;
        }

        case EInputActionValueType::Axis3D:
        {
            EnhancedInput.InjectInputForAction(Action, FInputActionValue(FVector(X, Y, Z)), {}, {});
            break;
        }

    }
}
