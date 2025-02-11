#pragma once

#include "CoreMinimal.h"
#include "CogDebugTrack.h"

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugPlotTrack : FCogDebugTrack
{
    virtual void Clear() override;

    void Plot(float X, float Y);

    bool FindValue(float Time, float& Value) const;

    void SetNumPlots(const int32 Value);

    int32 ValueOffset = 0;

    ImVector<ImVec2> Values;

    bool ShowValuesMarkers = false;
};
