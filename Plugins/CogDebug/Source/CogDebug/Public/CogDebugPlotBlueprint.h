#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CogDebugPlotBlueprint.generated.h"

UCLASS(meta = (ScriptName = "CogDebugPlot"))
class COGDEBUG_API UCogDebugPlotBlueprint : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly))
    static void Plot(const UObject* Owner, const FName Name, const float Value);
};