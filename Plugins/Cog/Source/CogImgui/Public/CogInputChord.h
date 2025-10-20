#pragma once

#include "Framework/Commands/InputChord.h"

#include "CogInputChord.generated.h"

USTRUCT()
struct FCogInputChord : public FInputChord
{
	GENERATED_BODY()

	FCogInputChord()
	{
	}

	FCogInputChord(const FKey InKey, const bool InTriggerWhenImguiWantTextInput = false)
		: FInputChord(InKey)
		, bTriggerWhenImguiWantTextInput(InTriggerWhenImguiWantTextInput)
	{
	}

    bool bTriggerWhenImguiWantTextInput = false;
};
