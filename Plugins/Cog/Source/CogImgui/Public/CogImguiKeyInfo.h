#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "CogImGuiKeyInfo.generated.h"

USTRUCT()
struct COGIMGUI_API FCogImGuiKeyInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
	FKey Key = EKeys::Invalid;

	UPROPERTY(EditAnywhere, Category = "Input")
    ECheckBoxState Shift = ECheckBoxState::Undetermined;

	UPROPERTY(EditAnywhere, Category = "Input")
    ECheckBoxState Ctrl = ECheckBoxState::Undetermined;

	UPROPERTY(EditAnywhere, Category = "Input")
    ECheckBoxState Alt = ECheckBoxState::Undetermined;

	UPROPERTY(EditAnywhere, Category = "Input")
    ECheckBoxState Cmd = ECheckBoxState::Undetermined;

    FCogImGuiKeyInfo()
    {
    }

    FCogImGuiKeyInfo(const FKey InKey, 
                     const ECheckBoxState InShift = ECheckBoxState::Undetermined, 
                     const ECheckBoxState InCtrl = ECheckBoxState::Undetermined,
                     const ECheckBoxState InAlt = ECheckBoxState::Undetermined,
                     const ECheckBoxState InCmd = ECheckBoxState::Undetermined)
        : Key(InKey)
        , Shift(InShift)
        , Ctrl(InCtrl)
        , Alt(InAlt)
        , Cmd(InCmd)
    {
    }

	friend bool operator==(const FCogImGuiKeyInfo& Lhs, const FCogImGuiKeyInfo& Rhs)
	{
		return Lhs.Key == Rhs.Key
			&& Lhs.Shift == Rhs.Shift
			&& Lhs.Ctrl == Rhs.Ctrl
			&& Lhs.Alt == Rhs.Alt
			&& Lhs.Cmd == Rhs.Cmd;
	}

	friend bool operator!=(const FCogImGuiKeyInfo& Lhs, const FCogImGuiKeyInfo& Rhs)
	{
		return !(Lhs == Rhs);
	}
};