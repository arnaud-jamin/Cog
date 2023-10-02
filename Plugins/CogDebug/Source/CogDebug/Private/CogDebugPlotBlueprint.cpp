#include "CogDebugPlotBlueprint.h"

#include "CogDebugDefines.h"
#include "CogDebugPlot.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugPlotBlueprint::Plot(const UObject* Owner, const FName Name, const float Value)
{
#if ENABLE_COG
    FCogDebugPlot::PlotValue(Owner, Name, Value);
#endif //ENABLE_COG
}
