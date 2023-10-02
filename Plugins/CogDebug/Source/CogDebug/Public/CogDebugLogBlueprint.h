#pragma  once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/LogVerbosity.h"
#include "CogDebugLogBlueprint.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogLogVerbosity : uint8
{
    Fatal           = ELogVerbosity::Fatal,
    Error           = ELogVerbosity::Error,
    Warning         = ELogVerbosity::Warning,
    Display         = ELogVerbosity::Display,
    Log             = ELogVerbosity::Log,
    Verbose         = ELogVerbosity::Verbose,
    VeryVerbose     = ELogVerbosity::VeryVerbose
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(meta = (ScriptName = "CogLogBlueprint"))
class COGDEBUG_API UCogDebugLogBlueprint : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "Log", meta = (DevelopmentOnly))
    static void Log(FCogLogCategory LogCategory, ECogLogVerbosity Verbosity = ECogLogVerbosity::Verbose, const AActor* Actor = nullptr, const FString& Text = FString(""));

    UFUNCTION(BlueprintPure, Category = "Log", meta = (DevelopmentOnly, AdvancedDisplay = "ScreenTextColor"))
    static bool IsLogActive(FCogLogCategory LogCategory, const AActor* Actor = nullptr);

};