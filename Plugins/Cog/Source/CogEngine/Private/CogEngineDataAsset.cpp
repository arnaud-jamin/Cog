#include "CogEngineDataAsset.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineCheat_Execution::Execute_Implementation(const AActor* Instigator, const TArray<AActor*>& Targets) const
{
}

//--------------------------------------------------------------------------------------------------------------------------
ECogEngineCheat_ActiveState UCogEngineCheat_Execution::IsActiveOnTargets_Implementation(const TArray<AActor*>& Targets) const
{
	return ECogEngineCheat_ActiveState::Inactive;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineCheat_Execution::GetColor(const FCogWindow& InCallingWindow, FLinearColor& OutColor) const
{
	return false;
}
