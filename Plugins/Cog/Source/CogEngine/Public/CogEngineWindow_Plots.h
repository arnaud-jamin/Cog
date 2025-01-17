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

    virtual void RenderAllEntriesNames(const ImVec2& InSize);

    virtual void RenderEntryName(const int Index, FCogDebugPlotEntry& Entry);

    virtual void RenderPlots(const TArray<FCogDebugPlotEntry*>& VisiblePlots) const;

    virtual void RenderMenu();

    virtual void RenderValues(FCogDebugPlotEntry& Entry, const char* Label) const;

    virtual void RenderEvents(FCogDebugPlotEntry& Entry, const char* Label, const ImVec2& PlotMin, const ImVec2& PlotMax) const;

    static void RenderEventTooltip(const FCogDebugPlotEvent* HoveredEvent, const FCogDebugPlotEntry& Entry);

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
    int NumGraphs = 1;

    UPROPERTY(Config)
    int NumYAxis = 1;

    UPROPERTY(Config)
    float TimeRange = 20.0f;

    UPROPERTY(Config)
    bool ShowTimeBarAtGameTime = true;

    UPROPERTY(Config)
    bool ShowTimeBarAtCursor = true;

    UPROPERTY(Config)
    bool ShowValueAtCursor = true;

    UPROPERTY(Config)
    float DragPauseSensitivity = 10.0f;

    UPROPERTY(Config)
    FColor PauseBackgroundColor = FColor(10, 0, 0, 255);

    UPROPERTY(Config)
    bool DockEntries = false;

    virtual void Reset() override
    {
        NumGraphs = 1;
        TimeRange = 20.0f;
        ShowTimeBarAtGameTime = true;
        ShowTimeBarAtCursor = true;
        ShowValueAtCursor = true;
        DragPauseSensitivity = 10.0f;
        PauseBackgroundColor = FColor(10, 0, 0, 255);
        DockEntries = false;
    }
};