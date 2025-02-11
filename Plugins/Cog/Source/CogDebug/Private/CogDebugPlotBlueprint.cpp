#include "CogDebugPlotBlueprint.h"

#include "CogCommon.h"
#include "CogDebugTracker.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugPlotBlueprint::Plot(const UObject* Owner, const FName Name, const float Value)
{
#if ENABLE_COG
    FCogDebug::Plot(Owner, Name, Value);
#endif //ENABLE_COG
}
