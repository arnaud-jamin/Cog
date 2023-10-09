#include "CogSampleLogCategories.h"

#include "AbilitySystemLog.h"
#include "CogSampleDefines.h"

#if USE_COG
#include "CogDebugLog.h"
#endif //USE_COG

DEFINE_LOG_CATEGORY(LogCogAlways);

DEFINE_LOG_CATEGORY(LogCogAbility);
DEFINE_LOG_CATEGORY(LogCogBaseAimRotation);
DEFINE_LOG_CATEGORY(LogCogCollision);
DEFINE_LOG_CATEGORY(LogCogControlRotation);
DEFINE_LOG_CATEGORY(LogCogInput);
DEFINE_LOG_CATEGORY(LogCogPosition);
DEFINE_LOG_CATEGORY(LogCogPossession);
DEFINE_LOG_CATEGORY(LogCogRotation);
DEFINE_LOG_CATEGORY(LogCogSkeleton);
DEFINE_LOG_CATEGORY(LogCogTargetAcquisition);

namespace CogSampleLog
{
    void RegiterAllLogCategories()
    {
#if USE_COG
        FCogDebugLog::AddLogCategory(LogCogAlways, "Always", false);

        FCogDebugLog::AddLogCategory(LogCogAbility, "Ability");
        FCogDebugLog::AddLogCategory(LogAbilitySystem, "Ability System");
        FCogDebugLog::AddLogCategory(LogCogBaseAimRotation, "BaseAimRotation");
        FCogDebugLog::AddLogCategory(LogCogCollision, "Collision");
        FCogDebugLog::AddLogCategory(LogCogControlRotation, "ControlRotation");
        FCogDebugLog::AddLogCategory(LogCogInput, "Input");
        FCogDebugLog::AddLogCategory(LogCogPosition, "Position");
        FCogDebugLog::AddLogCategory(LogCogPossession, "Possession");
        FCogDebugLog::AddLogCategory(LogCogRotation, "Rotation");
        FCogDebugLog::AddLogCategory(LogCogSkeleton, "Skeleton");
        FCogDebugLog::AddLogCategory(LogCogTargetAcquisition, "Target Acquisition");
        FCogDebugLog::AddLogCategory(LogGameplayEffects, "Gameplay Effects");
#endif //USE_COG
    }
}

