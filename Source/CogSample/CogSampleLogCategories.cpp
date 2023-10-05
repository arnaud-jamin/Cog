#include "CogSampleLogCategories.h"

#include "AbilitySystemLog.h"
#include "CogDefines.h"

#if USE_COG
#include "CogDebugLogCategoryManager.h"
#endif //USE_COG

DEFINE_LOG_CATEGORY(LogCogAlways);
DEFINE_LOG_CATEGORY(LogCogCollision);
DEFINE_LOG_CATEGORY(LogCogInput);
DEFINE_LOG_CATEGORY(LogCogPosition);
DEFINE_LOG_CATEGORY(LogCogRotation);
DEFINE_LOG_CATEGORY(LogCogControlRotation);
DEFINE_LOG_CATEGORY(LogCogBaseAimRotation);
DEFINE_LOG_CATEGORY(LogCogSkeleton);

namespace CogSampleLog
{
    void RegiterAllLogCategories()
    {
#if USE_COG
        FCogDebugLogCategoryManager::AddLogCategory(LogCogAlways, "Always", false);
        FCogDebugLogCategoryManager::AddLogCategory(LogAbilitySystem, "Ability System");
        FCogDebugLogCategoryManager::AddLogCategory(LogGameplayEffects, "Gameplay Effects");
        FCogDebugLogCategoryManager::AddLogCategory(LogCogCollision, "Collision");
        FCogDebugLogCategoryManager::AddLogCategory(LogCogInput, "Input");
        FCogDebugLogCategoryManager::AddLogCategory(LogCogPosition, "Position");
        FCogDebugLogCategoryManager::AddLogCategory(LogCogRotation, "Rotation");
        FCogDebugLogCategoryManager::AddLogCategory(LogCogControlRotation, "ControlRotation");
        FCogDebugLogCategoryManager::AddLogCategory(LogCogBaseAimRotation, "BaseAimRotation");
        FCogDebugLogCategoryManager::AddLogCategory(LogCogSkeleton, "Skeleton");
#endif //USE_COG
    }
}

