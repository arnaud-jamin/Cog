#include "CogSampleLogCategories.h"

#include "AbilitySystemLog.h"
#include "CogSampleDefines.h"

#if ENABLE_COG
#include "CogDebugLog.h"
#endif //ENABLE_COG

DEFINE_LOG_CATEGORY(LogCogAlways);

DEFINE_LOG_CATEGORY(LogCogAbility);
DEFINE_LOG_CATEGORY(LogCogAI);
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
#if ENABLE_COG
        FCogDebugLog::AddLogCategory(LogCogAlways, "Always", "Debug Category that is always active", false);

        FCogDebugLog::AddLogCategory(LogCogAbility, "Ability", "Log and debug draw of gameplay abilities");
        FCogDebugLog::AddLogCategory(LogCogAI, "AI", "Log and debug draw of AI behaviors");
        FCogDebugLog::AddLogCategory(LogAbilitySystem, "Ability System", "Unreal Ability System Log");
        FCogDebugLog::AddLogCategory(LogCogBaseAimRotation, "Base Aim Rotation", "Debug Draw of a Character BaseAimRotation");
        FCogDebugLog::AddLogCategory(LogCogCollision, "Collision", "Debug Draw a Character Collision");
        FCogDebugLog::AddLogCategory(LogCogControlRotation, "Control Rotation", "Debug Draw of the Character Control Rotation");
        FCogDebugLog::AddLogCategory(LogCogInput, "Input", "Log about the input actions");
        FCogDebugLog::AddLogCategory(LogCogPosition, "Position", "Debug draw of a character position");
        FCogDebugLog::AddLogCategory(LogCogPossession, "Possession", "Log about the possession of player controller over a Character");
        FCogDebugLog::AddLogCategory(LogCogRotation, "Rotation", "Debug Draw a Character Rotation");
        FCogDebugLog::AddLogCategory(LogCogSkeleton, "Skeleton", "Debug Draw a Character Skeleton");
        FCogDebugLog::AddLogCategory(LogCogTargetAcquisition, "Target Acquisition", "Debug Draw a Character Collision");
        FCogDebugLog::AddLogCategory(LogGameplayEffects, "Gameplay Effects", "Unreal Gameplay Effect Log");
#endif //ENABLE_COG
    }
}

