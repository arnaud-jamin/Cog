#include "CogCommonUI_ActionRouter.h"

#include "CogImguiInputHelper.h"

ERouteUIInputResult UCogCommonUI_ActionRouter::ProcessInput(FKey Key, EInputEvent InputEvent) const
{
	UWorld* World = GetWorld();

	if (FCogImguiInputHelper::IsTopPriorityKey(World, Key))
	{
		return ERouteUIInputResult::Unhandled;
	}
	
	return UCommonUIActionRouterBase::ProcessInput(Key, InputEvent);
}
