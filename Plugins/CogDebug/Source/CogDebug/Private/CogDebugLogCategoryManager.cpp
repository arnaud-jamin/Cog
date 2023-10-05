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
void FCogDebugLogCategoryManager::AddLogCategory(FLogCategoryBase& LogCategory, const FString& DisplayName, bool bVisible)
{
    LogCategories.Add(LogCategory.GetCategoryName(), 
        FCogDebugLogCategoryInfo
        {
            &LogCategory, 
            ELogVerbosity::NumVerbosity,
            DisplayName,
            bVisible,
            });
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
void FCogDebugLogCategoryManager::SetServerVerbosity(UWorld& World, FName CategoryName, ELogVerbosity::Type Verbosity)
{
    if (ACogDebugReplicator* Replicator = FCogDebugModule::Get().GetLocalReplicator(World))
    {
        Replicator->Server_SetCategoryVerbosity(CategoryName, (ECogLogVerbosity)Verbosity);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugLogCategoryManager::SetServerVerbosityActive(UWorld& World, FName CategoryName, bool Value)
{
    SetServerVerbosity(World, CategoryName, Value ? ELogVerbosity::Verbose : ELogVerbosity::Warning);
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
