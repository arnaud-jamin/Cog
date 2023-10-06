#include "CogSampleLogCategories.h"

#include "AbilitySystemLog.h"
#include "CogDefines.h"

#if USE_COG
#include "CogDebugLog.h"
#endif //USE_COG

DEFINE_LOG_CATEGORY(LogCogAlways);
DEFINE_LOG_CATEGORY(LogCogCollision);
DEFINE_LOG_CATEGORY(LogCogInput);
DEFINE_LOG_CATEGORY(LogCogPosition);
DEFINE_LOG_CATEGORY(LogCogRotation);
DEFINE_LOG_CATEGORY(LogCogControlRotation);
DEFINE_LOG_CATEGORY(LogCogBaseAimRotation);
DEFINE_LOG_CATEGORY(LogCogSkeleton);
DEFINE_LOG_CATEGORY(LogCogTargetAcquisition);

namespace CogSampleLog
{
    void RegiterAllLogCategories()
    {
#if USE_COG
        FCogDebugLog::AddLogCategory(LogCogAlways, "Always", false);
        FCogDebugLog::AddLogCategory(LogAbilitySystem, "Ability System");
        FCogDebugLog::AddLogCategory(LogGameplayEffects, "Gameplay Effects");
        FCogDebugLog::AddLogCategory(LogCogCollision, "Collision");
        FCogDebugLog::AddLogCategory(LogCogInput, "Input");
        FCogDebugLog::AddLogCategory(LogCogPosition, "Position");
        FCogDebugLog::AddLogCategory(LogCogRotation, "Rotation");
        FCogDebugLog::AddLogCategory(LogCogControlRotation, "ControlRotation");
        FCogDebugLog::AddLogCategory(LogCogBaseAimRotation, "BaseAimRotation");
        FCogDebugLog::AddLogCategory(LogCogSkeleton, "Skeleton");
        FCogDebugLog::AddLogCategory(LogCogTargetAcquisition, "Target Acquisition");
#endif //USE_COG
    }
}

