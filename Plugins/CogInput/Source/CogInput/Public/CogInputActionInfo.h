#pragma once

#include "CoreMinimal.h"
#include "CogDebugRob.h"
#include "EnhancedPlayerInput.h"

class UInputAction;
class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;

typedef TMap<TObjectPtr<const UInputMappingContext>, FAppliedInputContextData>  CogInputMappingContextMap;
DEFINE_PRIVATE_ACCESSOR_VARIABLE(UEnhancedPlayerInput_AppliedInputContexts, UEnhancedPlayerInput, CogInputMappingContextMap, AppliedInputContextData);

struct FCogInputActionInfo
{
    TObjectPtr<const UInputAction> Action;

    bool bPressed = false;

    bool bRepeat = false;

    float X = 0.0f;

    float Y = 0.0f;

    float Z = 0.0f;

    bool bIsMouseDownOnStick = false;

    bool bIsMouseDraggingStick = false;

    void Reset()
    {
        bPressed = false;
        bRepeat = false;
        X = 0.0f;
        Y = 0.0f;
        Z = 0.0f;
        bIsMouseDownOnStick = false;
        bIsMouseDraggingStick = false;
    }

    void Inject(UEnhancedInputLocalPlayerSubsystem& EnhancedInput, bool IsTimeToRepeat);
};

struct FCogInputMappingContextInfo
{
    TObjectPtr<const UInputMappingContext> MappingContext;

	FAppliedInputContextData AppliedInputContextData;

    TArray<FCogInputActionInfo> Actions;
};