#include "CogDebugLogCategoryManager.h"

#include "CogDebugModule.h"
#include "CogDebugReplicator.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

//--------------------------------------------------------------------------------------------------------------------------
DEFINE_LOG_CATEGORY(LogCogNone);
DEFINE_LOG_CATEGORY(LogCogServerDebug);

TMap<FName, FCogDebugLogCategoryInfo> FCogDebugLogCategoryManager::LogCategories;

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLogCategoryManager::AddLogCategory(FLogCategoryBase& LogCategory)
{
    LogCategories.Add(LogCategory.GetCategoryName(), FCogDebugLogCategoryInfo{ &LogCategory, ELogVerbosity::NumVerbosity });
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugLogCategoryManager::IsVerbosityActive(ELogVerbosity::Type Verbosity)
{
    return Verbosity >= ELogVerbosity::Verbose;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugLogCategoryManager::IsLogCategoryActive(const FLogCategoryBase& LogCategory)
{
    return IsVerbosityActive(LogCategory.GetVerbosity());
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugLogCategoryManager::IsLogCategoryActive(FName CategoryName)
{
    if (FLogCategoryBase* LogCategory = FindLogCategory(CategoryName))
    {
        return IsVerbosityActive(LogCategory->GetVerbosity());
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLogCategoryManager::SetLogCategoryActive(FLogCategoryBase& LogCategory, bool Value)
{
    LogCategory.SetVerbosity(Value ? ELogVerbosity::Verbose : ELogVerbosity::Warning);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLogCategoryManager::OnServerVerbosityChanged(FName CategoryName, ELogVerbosity::Type Verbosity)
{
    if (FCogDebugLogCategoryInfo* LogCategoryInfo = FindLogCategoryInfo(CategoryName))
    {
        LogCategoryInfo->ServerVerbosity = Verbosity;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
ELogVerbosity::Type FCogDebugLogCategoryManager::GetServerVerbosity(FName CategoryName)
{
    if (FCogDebugLogCategoryInfo* LogCategoryInfo = FindLogCategoryInfo(CategoryName))
    {
        return LogCategoryInfo->ServerVerbosity;
    }

    return ELogVerbosity::NoLogging;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLogCategoryManager::SetServerVerbosity(FName CategoryName, ELogVerbosity::Type Verbosity)
{
    if (ACogDebugReplicator* Replicator = FCogDebugModule::Get().GetLocalReplicator())
    {
        Replicator->Server_SetCategoryVerbosity(CategoryName, (ECogLogVerbosity)Verbosity);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLogCategoryManager::SetServerVerbosityActive(FName CategoryName, bool Value)
{
    SetServerVerbosity(CategoryName, Value ? ELogVerbosity::Verbose : ELogVerbosity::Warning);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogDebugLogCategoryManager::IsServerVerbosityActive(FName CategoryName)
{
    return IsVerbosityActive(GetServerVerbosity(CategoryName));
}

//--------------------------------------------------------------------------------------------------------------------------
FCogDebugLogCategoryInfo* FCogDebugLogCategoryManager::FindLogCategoryInfo(FName CategoryName)
{
    return LogCategories.Find(CategoryName);
}

//--------------------------------------------------------------------------------------------------------------------------
FLogCategoryBase* FCogDebugLogCategoryManager::FindLogCategory(FName CategoryName)
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
void FCogDebugLogCategoryManager::DeactivateAllLogCateories(UWorld& World)
{
    FString ToggleStr = TEXT("Log LogCogNone Only");
    GEngine->Exec(&World, *ToggleStr);

    if (APlayerController* PlayerController = World.GetFirstPlayerController())
    {
        PlayerController->ServerExec(ToggleStr);
    }
}
