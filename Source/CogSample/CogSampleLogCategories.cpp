#include "CogSampleLogCategories.h"

#include "AbilitySystemLog.h"
#include "CogDefines.h"

#if USE_COG
#include "CogDebugLogCategoryManager.h"
#endif //USE_COG

DEFINE_LOG_CATEGORY(LogCogCollision);
DEFINE_LOG_CATEGORY(LogCogInput);
DEFINE_LOG_CATEGORY(LogCogPosition);

namespace CogSampleLog
{
    void RegiterAllLogCategories()
    {
#if USE_COG
        FCogDebugLogCategoryManager::AddLogCategory(LogAbilitySystem);
        FCogDebugLogCategoryManager::AddLogCategory(LogGameplayEffects);
        FCogDebugLogCategoryManager::AddLogCategory(LogCogCollision);
        FCogDebugLogCategoryManager::AddLogCategory(LogCogInput);
        FCogDebugLogCategoryManager::AddLogCategory(LogCogPosition);
#endif //USE_COG
    }
}

