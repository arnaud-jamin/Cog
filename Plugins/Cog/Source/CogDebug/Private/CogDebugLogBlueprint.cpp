#include "CogDebugLogBlueprint.h"

#include "CogCommon.h"
#include "CogDebugLog.h"
#include "CogCommonLog.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugLogBlueprint::Log(const UObject* WorldContextObject, const FCogLogCategory LogCategory, ECogLogVerbosity Verbosity, const FString& Text)
{
#if ENABLE_COG

    const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory);

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
// ReSharper disable once CppPassValueParameterByConstReference
bool UCogDebugLogBlueprint::IsLogActive(const UObject* WorldContextObject, const FCogLogCategory LogCategory)
{
#if ENABLE_COG

    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        if (FCogDebugLog::IsLogCategoryActive(*LogCategoryPtr) == false)
        {
            return false;
        }

        if (FCogDebug::IsDebugActiveForObject(WorldContextObject) == false)
        {
            return false;
        }

        return true;
    }

#endif //ENABLE_COG

    return false;
}
