#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogWindow.h"
#include "implot.h"
#include "CogEngineWindow_Plots.generated.h"

struct FCogDebugEventHistory;
struct FCogDebugValueHistory;
struct ImVec2;
struct FCogDebugPlotEvent;
struct FCogDebugHistory;
class UCogEngineConfig_Plots;

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_Plots : public FCogWindow
{
    typedef FCogWindow Super;

protected:

    virtual void Initialize() override;

    virtual void RenderHelp() override;

    virtual void ResetConfig() override;

    virtual void RenderTick(float DeltaTime) override;

    virtual void PreBegin(ImGuiWindowFlags& WindowFlags) override;

    virtual void PostBegin() override;

    virtual void RenderContent() override;

    virtual void RenderAllEntriesNames(const ImVec2& InSize);

    virtual void RenderEntryName(const int Index, FCogDebugHistory& Entry);

    virtual void RenderPlots();

    virtual void RenderMenu();

    virtual void RenderValues(FCogDebugValueHistory& Entry, const char* Label) const;

    virtual void RenderEvents(FCogDebugEventHistory& Entry, const char* Label, const ImVec2& PlotMin, const ImVec2& PlotMax) const;

    static void RenderEventTooltip(const FCogDebugPlotEvent* HoveredEvent, const FCogDebugHistory& Entry);

    virtual void AssignToGraphAndAxis(const FName InName, const int32 InGraphIndex, const ImAxis InYAxis);

    virtual void UnassignToGraphAndAxis(const FName InName);

    virtual void RefreshPlotSettings();

    static FName GetDroppedEntryName(const ImGuiPayload* Payload);
    
    TObjectPtr<UCogEngineConfig_Plots> Config = nullptr;

    bool bApplyTimeScale = false;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogEngineConfig_Plots_GraphEntryInfo
{
    GENERATED_BODY()

    UPROPERTY(Config)
    FName Name;

    UPROPERTY(Config)
    int32 YAxis = 0;
};


//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogEngineConfig_Plots_GraphInfo
{
    GENERATED_BODY()

    UPROPERTY(Config)
    TArray<FCogEngineConfig_Plots_GraphEntryInfo> Entries;

};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_Plots : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    static constexpr int32 MaxNumGraphs = 5;

    UPROPERTY(Config)
    int NumGraphs = 1;

    UPROPERTY(Config)
    int NumYAxis = 1;

    UPROPERTY(Config)
    bool RecordValuesWhenPaused = true;
    
    UPROPERTY(Config)
    float TimeRange = 20.0f;

    UPROPERTY(Config)
    int32 NumRecordedValues = 2000;
    
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

    UPROPERTY(Config)
    float AutoFitPadding = 0.1f;

    UPROPERTY(Config)
    FCogEngineConfig_Plots_GraphInfo Graphs[MaxNumGraphs];

    virtual void Reset() override
    {
        NumGraphs = 1;
        TimeRange = 20.0f;
        RecordValuesWhenPaused = true;
        ShowTimeBarAtGameTime = true;
        ShowTimeBarAtCursor = true;
        ShowValueAtCursor = true;
        DragPauseSensitivity = 10.0f;
        PauseBackgroundColor = FColor(10, 0, 0, 255);
        DockEntries = false;
        NumRecordedValues = 2000;
        AutoFitPadding = 0.1f;

        for (FCogEngineConfig_Plots_GraphInfo& Graph : Graphs)
        {
            Graph.Entries.Empty();
        }
    }
};