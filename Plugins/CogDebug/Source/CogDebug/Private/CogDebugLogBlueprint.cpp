#include "CogDebugLogBlueprint.h"

#include "CogDebugLogCategoryManager.h"
#include "CogDebugLogMacros.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugLogBlueprint::Log(const UObject* WorldContextObject, FCogLogCategory LogCategory, ECogLogVerbosity Verbosity, const FString& Text)
{
#if ENABLE_COG

    const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory();

    if (LogCategoryPtr == nullptr)
    {
        COG_LOG(LogCogNone, ELogVerbosity::Warning, TEXT("Blueprint Log - Invalid Log Category: %s"), *Text);
        return;
    }

    if (WorldContextObject != nullptr)
    {
        COG_LOG_OBJECT_NO_CONTEXT(*LogCategoryPtr, (ELogVerbosity::Type)Verbosity, WorldContextObject, TEXT("%s"), *Text);
    }
    else
    {
        COG_LOG(*LogCategoryPtr, (ELogVerbosity::Type)Verbosity, TEXT("%s"), *Text);
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogDebugLogBlueprint::IsLogActive(const UObject* WorldContextObject, FCogLogCategory LogCategory)
{
#if ENABLE_COG

    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        if (FCogDebugLogCategoryManager::IsLogCategoryActive(*LogCategoryPtr) == false)
        {
            return false;
        }

        if (FCogDebugSettings::IsDebugActiveForObject(WorldContextObject) == false)
        {
            return false;
        }

        return true;
    }

#endif //ENABLE_COG

    return false;
}
