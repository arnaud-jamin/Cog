#include "CogCommonUI_ActionRouter.h"

#include "CogImguiInputHelper.h"

ERouteUIInputResult UCogCommonUI_ActionRouter::ProcessInput(FKey Key, EInputEvent InputEvent) const
{
	if (const UWorld* World = GetWorld())
	{
		if (const UPlayerInput* PlayerInput = FCogImguiInputHelper::GetPlayerInput(*World))
		{
			if (FCogImguiInputHelper::IsTopPriorityKey(*PlayerInput, Key))
			{
				return ERouteUIInputResult::Unhandled;
			}
		}
	}
	
	return UCommonUIActionRouterBase::ProcessInput(Key, InputEvent);
}
