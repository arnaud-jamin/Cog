#pragma once

#include "CoreMinimal.h"
#include "CogCommon.h"
#include "CogCommonLog.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECogLogVerbosity : uint8
{
    NoLogging = ELogVerbosity::NoLogging,
    Fatal = ELogVerbosity::Fatal,
    Error = ELogVerbosity::Error,
    Warning = ELogVerbosity::Warning,
    Display = ELogVerbosity::Display,
    Log = ELogVerbosity::Log,
    Verbose = ELogVerbosity::Verbose,
    VeryVerbose = ELogVerbosity::VeryVerbose
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct COGCOMMON_API FCogLogCategory
{
    GENERATED_USTRUCT_BODY()

    FCogLogCategory() {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cog")
    FName Name;

    FString GetName() const { return Name.ToString(); }

    mutable FCogLogCategoryAlias* LogCategory = nullptr;
};
