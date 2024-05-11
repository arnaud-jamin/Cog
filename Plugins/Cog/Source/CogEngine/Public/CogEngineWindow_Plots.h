#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "CogEngineWindow_Plots.generated.h"

struct ImVec2;
struct FCogDebugPlotEvent;
struct FCogDebugPlotEntry;
class UCogEngineConfig_Plots;

class COGENGINE_API FCogEngineWindow_Plots : public FCogWindow
{
    typedef FCogWindow Super;

protected:

    virtual void Initialize() override;

    virtual void RenderHelp() override;

    virtual void ResetConfig() override;

    virtual void RenderTick(float DeltaTime) override;

    virtual void RenderContent() override;

    static void RenderPlotsList(TArray<FCogDebugPlotEntry*>& VisiblePlots);

    void RenderPlots(const TArray<FCogDebugPlotEntry*>& VisiblePlots) const;

    void RenderMenu();

    void RenderTimeMarker() const;

    static void RenderValues(FCogDebugPlotEntry& Entry, const char* Label);

    void RenderEvents(FCogDebugPlotEntry& Entry, const char* Label, const ImVec2& PlotMin, const ImVec2& PlotMax) const;

    static void RenderEventTooltip(const FCogDebugPlotEvent* HoveredEvent, FCogDebugPlotEntry& Entry);

    TObjectPtr<UCogEngineConfig_Plots> Config = nullptr;

    bool bApplyTimeScale = false;

private:

};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_Plots : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    int Rows = 1;

    UPROPERTY(Config)
    float TimeRange = 10.0f;

    virtual void Reset() override
    {
        Rows = 1;
        TimeRange = 10.0f;
    }
};