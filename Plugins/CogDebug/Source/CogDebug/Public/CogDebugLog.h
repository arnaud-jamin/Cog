#pragma  once

#include "CoreMinimal.h"
#include "CogDebugDefines.h"
#include "Logging/LogVerbosity.h"

//--------------------------------------------------------------------------------------------------------------------------
DECLARE_LOG_CATEGORY_EXTERN(LogCogNone, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCogServerDebug, Verbose, All);

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugLogCategoryInfo
{
    FLogCategoryBase* LogCategory = nullptr;
    ELogVerbosity::Type ServerVerbosity = ELogVerbosity::NoLogging;
    FString DisplayName;
    bool bVisible = true;

    FString GetDisplayName() const;
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugLog
{
    static void AddLogCategory(FLogCategoryBase& LogCategory, const FString& DisplayName = "", bool bVisible = true);

    static bool IsVerbosityActive(ELogVerbosity::Type Verbosity);

    static bool IsLogCategoryActive(const FLogCategoryBase& LogCategory);

    static bool IsLogCategoryActive(FName CategoryName);

    static void SetLogCategoryActive(FLogCategoryBase& LogCategory, bool Value);

    static FLogCategoryBase* FindLogCategory(FName LogCategory);

    static FCogDebugLogCategoryInfo* FindLogCategoryInfo(FName LogCategory);

    static TMap<FName, FCogDebugLogCategoryInfo>& GetLogCategories() { return LogCategories; }

    static void SetServerVerbosityActive(UWorld& World, FName LogCategory, bool Value);

    static bool IsServerVerbosityActive(FName LogCategory);

    static ELogVerbosity::Type GetServerVerbosity(FName LogCategory);

    static void SetServerVerbosity(UWorld& World, FName LogCategory, ELogVerbosity::Type Verbosity);

    static void OnServerVerbosityChanged(FName LogCategory, ELogVerbosity::Type Verbosity);

    static void DeactivateAllLogCateories(UWorld& World);

private:

    static TMap<FName, FCogDebugLogCategoryInfo> LogCategories;

};
