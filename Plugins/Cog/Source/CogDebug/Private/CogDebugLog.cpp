#include "CogDebugLog.h"

#include "CogDebugModule.h"
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
void FCogDebugLog::AddLogCategory(FLogCategoryBase& LogCategory, const FString& DisplayName, const FString& Description, bool bVisible)
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
bool FCogDebugLog::IsVerbosityActive(ELogVerbosity::Type Verbosity)
{
    return Verbosity >= ELogVerbosity::Verbose;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugLog::IsLogCategoryActive(const FLogCategoryBase& LogCategory)
{
    return IsVerbosityActive(LogCategory.GetVerbosity());
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugLog::IsLogCategoryActive(FName CategoryName)
{
    if (FLogCategoryBase* LogCategory = FindLogCategory(CategoryName))
    {
        return IsVerbosityActive(LogCategory->GetVerbosity());
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLog::SetLogCategoryActive(FLogCategoryBase& LogCategory, bool Value)
{
    LogCategory.SetVerbosity(Value ? ELogVerbosity::Verbose : ELogVerbosity::Warning);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLog::OnServerVerbosityChanged(FName CategoryName, ELogVerbosity::Type Verbosity)
{
    if (FCogDebugLogCategoryInfo* LogCategoryInfo = FindLogCategoryInfo(CategoryName))
    {
        LogCategoryInfo->ServerVerbosity = Verbosity;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
ELogVerbosity::Type FCogDebugLog::GetServerVerbosity(FName CategoryName)
{
    if (FCogDebugLogCategoryInfo* LogCategoryInfo = FindLogCategoryInfo(CategoryName))
    {
        return LogCategoryInfo->ServerVerbosity;
    }

    return ELogVerbosity::NoLogging;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLog::SetServerVerbosity(UWorld& World, FName CategoryName, ELogVerbosity::Type Verbosity)
{
    if (ACogDebugReplicator* Replicator = ACogDebugReplicator::GetLocalReplicator(World))
    {
        Replicator->Server_SetCategoryVerbosity(CategoryName, (ECogLogVerbosity)Verbosity);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLog::SetServerVerbosityActive(UWorld& World, FName CategoryName, bool Value)
{
    SetServerVerbosity(World, CategoryName, Value ? ELogVerbosity::Verbose : ELogVerbosity::Warning);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugLog::IsServerVerbosityActive(FName CategoryName)
{
    return IsVerbosityActive(GetServerVerbosity(CategoryName));
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugLogCategoryInfo* FCogDebugLog::FindLogCategoryInfo(FName CategoryName)
{
    return LogCategories.Find(CategoryName);
}

//--------------------------------------------------------------------------------------------------------------------------
FLogCategoryBase* FCogDebugLog::FindLogCategory(FName CategoryName)
{
    if (FCogDebugLogCategoryInfo* LogCategoryInfo = FindLogCategoryInfo(CategoryName))
    {
        return LogCategoryInfo->LogCategory;
    }
    else
    {
        return nullptr;
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLog::DeactivateAllLogCateories(UWorld& World)
{
    FString ToggleStr = TEXT("Log LogCogNone Only");
    GEngine->Exec(&World, *ToggleStr);

    if (APlayerController* PlayerController = World.GetFirstPlayerController())
    {
        PlayerController->ServerExec(ToggleStr);
    }
}
