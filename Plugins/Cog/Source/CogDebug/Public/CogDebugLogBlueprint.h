#pragma  once

#include "CoreMinimal.h"
#include "CogCommonLog.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "CogDebugLogBlueprint.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(meta = (ScriptName = "CogLogBlueprint"))
class COGDEBUG_API UCogDebugLogBlueprint : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static void Log(const UObject* WorldContextObject, FCogLogCategory LogCategory, ECogLogVerbosity Verbosity = ECogLogVerbosity::Verbose, const FString& Text = FString(""));

    UFUNCTION(BlueprintPure, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"), Category = "Cog")
    static bool IsLogActive(const UObject* WorldContextObject, const FCogLogCategory LogCategory);

};