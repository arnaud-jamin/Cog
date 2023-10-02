#include "CogDebugLogBlueprint.h"

#include "CogDebugLogCategoryManager.h"
#include "CogDebugLogMacros.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugLogBlueprint::Log(FCogLogCategory LogCategory, ECogLogVerbosity Verbosity, const AActor* Actor, const FString& Text)
{
#if ENABLE_COG

    const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory();

    if (LogCategoryPtr == nullptr)
    {
        COG_LOG(LogCogNone, ELogVerbosity::Warning, TEXT("Blueprint Log - Invalid Log Category: %s"), *Text);
        return;
    }

    if (Actor != nullptr)
    {
        COG_LOG_ACTOR_NO_CONTEXT(*LogCategoryPtr, (ELogVerbosity::Type)Verbosity, Actor, TEXT("%s"), *Text);
    }
    else
    {
        COG_LOG(*LogCategoryPtr, (ELogVerbosity::Type)Verbosity, TEXT("%s"), *Text);
    }

#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogDebugLogBlueprint::IsLogActive(FCogLogCategory LogCategory, const AActor* Actor)
{
#if ENABLE_COG

    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        if (FCogDebugLogCategoryManager::IsLogCategoryActive(*LogCategoryPtr) == false)
        {
            return false;
        }

        if (FCogDebugSettings::IsDebugActiveForActor(Actor) == false)
        {
            return false;
        }

        return true;
    }

#endif //ENABLE_COG

    return false;
}
