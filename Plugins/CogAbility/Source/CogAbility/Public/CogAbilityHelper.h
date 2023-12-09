#pragma once

#include "CoreMinimal.h"
#include "imgui.h"


class UCogAbilityDataAsset;
class UGameplayEffect;
namespace EGameplayModOp { enum Type : int; }
struct FGameplayTagContainer;

class COGABILITY_API FCogAbilityHelper
{
public:

    static FString CleanupName(FString Str);

    static void RenderTagContainer(const FGameplayTagContainer& Container);
};
