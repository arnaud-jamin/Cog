#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCogAlways, VeryVerbose, All);

DECLARE_LOG_CATEGORY_EXTERN(LogCogAbility, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCogBaseAimRotation, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCogCollision, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCogControlRotation, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCogInput, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCogPosition, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCogPossession, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCogRotation, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCogSkeleton, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCogTargetAcquisition, Warning, All);

namespace CogSampleLog
{
    void RegiterAllLogCategories();
}