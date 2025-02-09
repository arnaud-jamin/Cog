#pragma  once

#include "CoreMinimal.h"
#include "Logging/LogVerbosity.h"

struct FCogLogCategory;

//--------------------------------------------------------------------------------------------------------------------------
DECLARE_LOG_CATEGORY_EXTERN(LogCogNone, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCogServerDebug, Verbose, All);

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugLogCategoryInfo
{
    FLogCategoryBase* LogCategory = nullptr;
    
    ELogVerbosity::Type ServerVerbosity = ELogVerbosity::NoLogging;
    
    FString DisplayName;

    FString Description;

    bool bVisible = true;

    FString GetDisplayName() const;
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugLog
{
    static void AddLogCategory(FLogCategoryBase& LogCategory, const FString& DisplayName = "", const FString& Description = "", bool bVisible = true);

    static bool IsVerbosityActive(ELogVerbosity::Type Verbosity);

    static bool IsLogCategoryActive(const FLogCategoryBase& LogCategory);

    static bool IsLogCategoryActive(FName CategoryName);

    static void SetLogCategoryActive(FLogCategoryBase& LogCategory, bool Value);

    static FLogCategoryBase* FindLogCategory(FName CategoryName);

    static FLogCategoryBase* GetLogCategoryBase(const FCogLogCategory& LogCategory);

    static FCogDebugLogCategoryInfo* FindLogCategoryInfo(FName CategoryName);

    static TMap<FName, FCogDebugLogCategoryInfo>& GetLogCategories() { return LogCategories; }

    static void SetServerVerbosityActive(const UWorld& World, FName CategoryName, bool Value);

    static bool IsServerVerbosityActive(FName CategoryName);

    static ELogVerbosity::Type GetServerVerbosity(FName CategoryName);

    static void SetServerVerbosity(const UWorld& World, FName CategoryName, ELogVerbosity::Type Verbosity);

    static void OnServerVerbosityChanged(FName CategoryName, ELogVerbosity::Type Verbosity);

    static void DeactivateAllLogCategories(UWorld& World);

private:

    static TMap<FName, FCogDebugLogCategoryInfo> LogCategories;

};
