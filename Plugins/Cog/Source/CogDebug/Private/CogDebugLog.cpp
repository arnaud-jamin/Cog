#include "CogDebugLog.h"

#include "CogCommonLog.h"
#include "CogDebugReplicator.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

//--------------------------------------------------------------------------------------------------------------------------
DEFINE_LOG_CATEGORY(LogCogNone);
DEFINE_LOG_CATEGORY(LogCogServerDebug);

TMap<FName, FCogDebugLogCategoryInfo> FCogDebugLog::LogCategories;


//--------------------------------------------------------------------------------------------------------------------------
// FCogDebugLogCategoryInfo
//--------------------------------------------------------------------------------------------------------------------------
FString FCogDebugLogCategoryInfo::GetDisplayName() const
{
    if (DisplayName.IsEmpty() == false)
    {
        return DisplayName;
    }

    if (LogCategory != nullptr)
    {
        return LogCategory->GetCategoryName().ToString();
    }

    return FString("Invalid");
}

//--------------------------------------------------------------------------------------------------------------------------
// FCogDebugLogCategoryManager
//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLog::AddLogCategory(FLogCategoryBase& LogCategory, const FString& DisplayName, const FString& Description, const bool bVisible)
{
    LogCategories.Add(LogCategory.GetCategoryName(), 
        FCogDebugLogCategoryInfo
        {
            &LogCategory, 
            ELogVerbosity::NumVerbosity,
            DisplayName,
            Description,
            bVisible,
            });
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugLog::IsVerbosityActive(const ELogVerbosity::Type Verbosity)
{
    return Verbosity >= ELogVerbosity::Verbose;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugLog::IsLogCategoryActive(const FLogCategoryBase& LogCategory)
{
    return IsVerbosityActive(LogCategory.GetVerbosity());
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugLog::IsLogCategoryActive(const FName CategoryName)
{
    if (const FLogCategoryBase* LogCategory = FindLogCategory(CategoryName))
    {
        return IsVerbosityActive(LogCategory->GetVerbosity());
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLog::SetLogCategoryActive(FLogCategoryBase& LogCategory, const bool Value)
{
    LogCategory.SetVerbosity(Value ? ELogVerbosity::Verbose : ELogVerbosity::Warning);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLog::OnServerVerbosityChanged(const FName CategoryName, const ELogVerbosity::Type Verbosity)
{
    if (FCogDebugLogCategoryInfo* LogCategoryInfo = FindLogCategoryInfo(CategoryName))
    {
        LogCategoryInfo->ServerVerbosity = Verbosity;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
ELogVerbosity::Type FCogDebugLog::GetServerVerbosity(const FName CategoryName)
{
    if (const FCogDebugLogCategoryInfo* LogCategoryInfo = FindLogCategoryInfo(CategoryName))
    {
        return LogCategoryInfo->ServerVerbosity;
    }

    return ELogVerbosity::NoLogging;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLog::SetServerVerbosity(const UWorld& World, const FName CategoryName, ELogVerbosity::Type Verbosity)
{
    if (ACogDebugReplicator* Replicator = ACogDebugReplicator::GetLocalReplicator(World))
    {
        Replicator->Server_SetCategoryVerbosity(CategoryName, static_cast<ECogLogVerbosity>(Verbosity));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLog::SetServerVerbosityActive(const UWorld& World, const FName CategoryName, const bool Value)
{
    SetServerVerbosity(World, CategoryName, Value ? ELogVerbosity::Verbose : ELogVerbosity::Warning);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugLog::IsServerVerbosityActive(const FName CategoryName)
{
    return IsVerbosityActive(GetServerVerbosity(CategoryName));
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugLogCategoryInfo* FCogDebugLog::FindLogCategoryInfo(const FName CategoryName)
{
    return LogCategories.Find(CategoryName);
}

//--------------------------------------------------------------------------------------------------------------------------
FLogCategoryBase* FCogDebugLog::FindLogCategory(const FName CategoryName)
{
    if (const FCogDebugLogCategoryInfo* LogCategoryInfo = FindLogCategoryInfo(CategoryName))
    {
        return LogCategoryInfo->LogCategory;
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLog::DeactivateAllLogCategories(UWorld& World)
{
	const FString ToggleStr = TEXT("Log LogCogNone Only");
    GEngine->Exec(&World, *ToggleStr);

    if (APlayerController* PlayerController = World.GetFirstPlayerController())
    {
        PlayerController->ServerExec(ToggleStr);
    }
}


//--------------------------------------------------------------------------------------------------------------------------
FLogCategoryBase* FCogDebugLog::GetLogCategoryBase(const FCogLogCategory& LogCategory)
{
#if NO_LOGGING

    return nullptr;

#else

    if (LogCategory.Name.IsNone() || LogCategory.Name.IsValid() == false)
    {
        return nullptr;
    }

    if (LogCategory.LogCategory == nullptr)
    {
        if (const FCogDebugLogCategoryInfo* CategoryInfo = FCogDebugLog::GetLogCategories().Find(LogCategory.Name))
        {
            LogCategory.LogCategory = CategoryInfo->LogCategory;
        }
    }

    return LogCategory.LogCategory;

#endif //NO_LOGGING
}

