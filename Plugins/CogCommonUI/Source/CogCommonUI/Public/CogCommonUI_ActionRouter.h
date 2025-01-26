#pragma once

#include "CoreMinimal.h"
#include "Input/CommonUIActionRouterBase.h"
#include "CogCommonUI_ActionRouter.generated.h"

UCLASS()
class COGCOMMONUI_API UCogCommonUI_ActionRouter : public UCommonUIActionRouterBase
{
	GENERATED_BODY()
	
	virtual ERouteUIInputResult ProcessInput(FKey Key, EInputEvent InputEvent) const override;
};
