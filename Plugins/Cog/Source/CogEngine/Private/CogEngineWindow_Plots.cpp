#include "CogEngineWindow_Plots.h"

#include "CogDebugPlot.h"
#include "CogImguiHelper.h"
#include "CogWindowWidgets.h"
#include "Engine/World.h"
#include "imgui.h"
#include "implot_internal.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::Initialize()
{
    Super::Initialize();

    bHasMenu = true;
    bNoPadding = true;

    Config = GetConfig<UCogEngineConfig_Plots>();

    if (Config != nullptr)
    {
        RefreshPlotSettings();
    }
    
    FCogDebugPlot::Clear();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::RenderHelp()
{
    ImGui::Text(
        "This window plots values and events overtime. When applicable, only the values of the selected actor are displayed."
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::ResetConfig()
{
    Super::ResetConfig();

    RefreshPlotSettings();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);
    FCogDebugPlot::IsVisible = GetIsVisible();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::RenderContent()
{
    Super::RenderContent();

    RenderMenu();

    if (Config->DockEntries)
    {
        if (ImGui::BeginTable("PlotTable", 2, ImGuiTableFlags_RowBg
            | ImGuiTableFlags_Resizable
            | ImGuiTableFlags_BordersV
            | ImGuiTableFlags_NoPadInnerX
            | ImGuiTableFlags_NoPadOuterX))
        {

            ImGui::TableSetupColumn("PlotsList", ImGuiTableColumnFlags_WidthFixed, FCogWindowWidgets::GetFontWidth() * 20.0f);
            ImGui::TableSetupColumn("Plots", ImGuiTableColumnFlags_WidthStretch, 0.0f);
            ImGui::TableNextRow();


            ImGui::TableNextColumn();
            RenderAllEntriesNames(ImVec2(0, -1));

            ImGui::TableNextColumn();
            RenderPlots();

            ImGui::EndTable();
        }
        
    }
    else
    {
        RenderPlots();
    }

    bApplyTimeScale = false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::RefreshPlotSettings()
{
    FCogDebugPlot::SetNumRecordedValues(Config->NumRecordedValues);
    FCogDebugPlot::RecordValuesWhenPause = Config->RecordValuesWhenPaused;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::RenderMenu()
{
    if (ImGui::BeginMenuBar())
    {
        if (Config->DockEntries == false)
        {
            if (ImGui::BeginMenu("Entries"))
            {
                RenderAllEntriesNames(ImVec2(ImGui::GetFontSize() * 15, ImGui::GetFontSize() * 20));
                ImGui::EndMenu();
            }
        }

        if (ImGui::BeginMenu("Options"))
        {
            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::SliderInt("Num graphs", &Config->NumGraphs, 1, UCogEngineConfig_Plots::MaxNumGraphs);

            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::SliderInt("Num Y axis", &Config->NumYAxis, 1, 3);
            
            FCogWindowWidgets::SetNextItemToShortWidth();
            if (ImGui::SliderFloat("Time range", &Config->TimeRange, 1.0f, 100.0f, "%0.0f"))
            {
                bApplyTimeScale = true;
            }

            FCogWindowWidgets::SetNextItemToShortWidth();
            if (ImGui::SliderInt("Num recorded values", &Config->NumRecordedValues, 100, 10000))
            {
                Config->NumRecordedValues = (Config->NumRecordedValues / 100) * 100; 
            }
            
            if (ImGui::IsItemDeactivatedAfterEdit()) 
            {
                RefreshPlotSettings();
            }

            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::SliderFloat("Auto-fit padding", &Config->AutoFitPadding, 0.0f, 0.2f, "%0.2f");
            
            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::SliderFloat("Drag pause sensitivity", &Config->DragPauseSensitivity, 1.0f, 50.0f, "%0.0f");

            ImGui::Checkbox("Record values when paused", &Config->RecordValuesWhenPaused);
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                RefreshPlotSettings();
            }
            
            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::Checkbox("Show time bar at game time", &Config->ShowTimeBarAtGameTime);

            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::Checkbox("Show time bar at cursor", &Config->ShowTimeBarAtCursor);

            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::Checkbox("Show value at cursor", &Config->ShowValueAtCursor);

            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::Checkbox("Dock entries", &Config->DockEntries);

            constexpr ImGuiColorEditFlags ColorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf;
            FCogImguiHelper::ColorEdit4("Pause background color", Config->PauseBackgroundColor, ColorEditFlags);
            ImGui::SetItemTooltip("Background color of the plot when paused.");

            ImGui::Separator();

            if (ImGui::MenuItem("Reset Settings"))
            {
                FCogDebugPlot::Pause = false;
                FCogDebugPlot::Reset();
                ResetConfig();
                bApplyTimeScale = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Clear"))
        {
            FCogDebugPlot::Clear();
        }

        FCogWindowWidgets::ToggleMenuButton(&FCogDebugPlot::Pause, "Pause", ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

        ImGui::EndMenuBar();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::RenderEntryName(const int Index, FCogDebugHistory& Entry)
{
    ImGui::PushID(Index);

    bool IsAssignedToGraph = false;

    for (int32 i = 0; i < UCogEngineConfig_Plots::MaxNumGraphs; ++i)
    {
        FCogEngineConfig_Plots_GraphInfo& GraphInfo = Config->Graphs[i];
        if (GraphInfo.Entries.ContainsByPredicate([Entry](const auto& InEntry) { return InEntry.Name == Entry.Name; }))
        {
            IsAssignedToGraph = true;
            break;
        }
    }

    if (ImGui::Selectable(TCHAR_TO_ANSI(*Entry.Name.ToString()), IsAssignedToGraph, ImGuiSelectableFlags_AllowDoubleClick))
    {
        if (IsAssignedToGraph)
        {
            UnassignToGraphAndAxis(Entry.Name);
        }
        else
        {
            AssignToGraphAndAxis(Entry.Name, 0, ImAxis_Y1);
        }
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        const auto EntryName = StringCast<ANSICHAR>(*Entry.Name.ToString());
        ImGui::SetDragDropPayload("DragAndDrop", EntryName.Get(), EntryName.Length() + 1);
        ImGui::Text("%s", EntryName.Get());
        ImGui::EndDragDropSource();
    }

    ImGui::PopID();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::RenderAllEntriesNames(const ImVec2& InSize)
{
    const int32 Indent = ImGui::GetFontSize() * 0.5f;

    if (ImGui::BeginChild("Entries", InSize))
    {
        int Index = 0;

        if (FCogWindowWidgets::DarkCollapsingHeader("Events", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent(Indent);
            if (FCogDebugPlot::Events.IsEmpty())
            {
                ImGui::TextDisabled("No event added yet");
            }
            else
            {
                for (auto& kv : FCogDebugPlot::Events)
                {
                    RenderEntryName(Index, kv.Value);
                    Index++;
                }
            }
            ImGui::Unindent(Indent);
        }

        if (FCogWindowWidgets::DarkCollapsingHeader("Plots", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent(Indent);
            if (FCogDebugPlot::Values.IsEmpty())
            {
                ImGui::TextDisabled("No plot added yet");
            }
            else
            {
                for (auto& kv : FCogDebugPlot::Values)
                {
                    RenderEntryName(Index, kv.Value);
                    Index++;
                }
            }
            ImGui::Unindent(Indent);
        }
    }
    ImGui::EndChild();

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("DragAndDrop"))
        {
            UnassignToGraphAndAxis(GetDroppedEntryName(Payload));
        }
        ImGui::EndDragDropTarget();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::RenderPlots()
{
    if (ImGui::BeginChild("Graph", ImVec2(0, -1)))
    {
        static float RowRatios[] = { 1, 1, 1, 1, 1, 1 };
        static float ColRatios[] = { 1 };
        static ImPlotSubplotFlags SubplotsFlags = ImPlotSubplotFlags_LinkCols;

        const bool PushPlotBgStyle = FCogDebugPlot::Pause;
        if (PushPlotBgStyle)
        {
            ImPlot::PushStyleColor(ImPlotCol_PlotBg, FCogImguiHelper::ToImVec4(Config->PauseBackgroundColor));
        }

        ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, ImVec2(0.0f, Config->AutoFitPadding));

        if (ImPlot::BeginSubplots("", Config->NumGraphs, 1, ImVec2(-1, -1), SubplotsFlags, RowRatios, ColRatios))
        {
            for (int32 GraphIndex = 0; GraphIndex < Config->NumGraphs && GraphIndex < UCogEngineConfig_Plots::MaxNumGraphs; ++GraphIndex)
            {
                ImGui::PushID(GraphIndex);

                FCogEngineConfig_Plots_GraphInfo& GraphInfo = Config->Graphs[GraphIndex];

                if (ImPlot::BeginPlot("##Plot", ImVec2(-1, 250)))
                {
                    ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines);

                    //--------------------------------------------------------------------------------------------------
                    // Set the initial X axis range. After, it is automatically updated to move with the current time.
                    //--------------------------------------------------------------------------------------------------
                    ImPlot::SetupAxisLimits(ImAxis_X1, 0, Config->TimeRange, ImGuiCond_Appearing);

                    //--------------------------------------------------------------------------------------------------
                    // Setup the Y axis 
                    //--------------------------------------------------------------------------------------------------
                    for (int32 YAxisIndex = 0; YAxisIndex <= (ImAxis_Y3 - ImAxis_Y1); ++YAxisIndex)
                    {
                        const ImAxis YAxis = ImAxis_Y1 + YAxisIndex;

                        bool IsAssigned = false;
                        int32 YMax = 0;
                        for (const FCogEngineConfig_Plots_GraphEntryInfo& GraphEntry : GraphInfo.Entries)
                        {
                            IsAssigned |= GraphEntry.YAxis == YAxis;

                            if (FCogDebugEventHistory* EventHistory = FCogDebugPlot::Events.Find(GraphEntry.Name))
                            {
                                YMax = FMath::Max(FMath::Max(5, YMax), EventHistory->MaxRow);
                            }
                        }

                        const bool IsAxisVisible = IsAssigned || (YAxisIndex < Config->NumYAxis);
                        if (IsAxisVisible)
                        {
                            ImPlotAxisFlags Flags = IsAssigned ? ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_AutoFit 
                                                               : ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_AutoFit;
                            if (YAxisIndex > 0)
                            {
                                Flags |= ImPlotAxisFlags_Opposite;
                            }

                            ImPlot::SetupAxis(YAxis, IsAssigned || (Config->NumYAxis == 1) ? "" : "[drop here]", Flags);

                            //--------------------------------------------------------------------------------
                            // Set the Y axis limit for Events. 
                            //--------------------------------------------------------------------------------
                            if (YMax > 0)
                            {
                                ImPlot::SetupAxisLimits(YAxis, 0, YMax, ImGuiCond_Always);
                            }
                        }
                    }

                    const ImPlotRange& PlotRange = ImPlot::GetCurrentPlot()->Axes[ImAxis_X1].Range;
                    const float TimeRange = PlotRange.Max - PlotRange.Min;

                    if (bApplyTimeScale == false)
                    {
                        Config->TimeRange = TimeRange;
                    }

                    const float Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

                    //------------------------------------------------------------------
                    // Setup all the X axis limits. Must be done before calling
                    // ImPlot::GetPlotPos or ImPlot::GetPlotSize as it calls SetupLock()
                    //------------------------------------------------------------------
                    {
                        //--------------------------------------------------------------------------------
                        // Make the time axis move forward automatically, unless the user pauses or zoom.
                        //--------------------------------------------------------------------------------
                        if (FCogDebugPlot::Pause == false && ImGui::GetIO().MouseWheel == 0)
                        {
                            ImPlot::SetupAxisLimits(ImAxis_X1, Time - TimeRange, Time, ImGuiCond_Always);
                        }

                        if (bApplyTimeScale)
                        {
                            ImPlot::SetupAxisLimits(ImAxis_X1, Time - Config->TimeRange, Time, ImGuiCond_Always);
                        }
                    }

                    const ImVec2 PlotMin = ImPlot::GetPlotPos();
                    const ImVec2 PlotSize = ImPlot::GetPlotSize();
                    const ImVec2 PlotMax = PlotMin + PlotSize;

                    //----------------------------------------------------------------
                    // Pause the scrolling if the user drag inside
                    //----------------------------------------------------------------
                    if (ImGui::IsWindowFocused())
                    {
                        const ImVec2 Mouse = ImGui::GetMousePos();
                        if (Mouse.x > PlotMin.x
                            && Mouse.y > PlotMin.y
                            && Mouse.x < PlotMax.x
                            && Mouse.y < PlotMax.y
                            && ImGui::GetDragDropPayload() == nullptr)
                        {
                            const ImVec2 Drag = ImGui::GetMouseDragDelta(0);
                        
                            if (FMath::Abs(Drag.x) > Config->DragPauseSensitivity)
                            {
                                FCogDebugPlot::Pause = true;
                            }
                        }
                    }

                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        FCogDebugPlot::Pause = false;
                    }

                    //---------------------------------------------------------------------------
                    // Draw a vertical lines representing the current time and the mouse time
                    //---------------------------------------------------------------------------
                    if (Config->ShowTimeBarAtGameTime || Config->ShowTimeBarAtGameTime)
                    {
                        ImDrawList* PlotDrawList = ImPlot::GetPlotDrawList();

                        const float PlotTop = PlotMin.y;
                        const float TimeBarBottom = PlotTop + PlotSize.y;

                        ImPlot::PushPlotClipRect();
                        if (Config->ShowTimeBarAtCursor)
                        {
                            PlotDrawList->AddLine(ImVec2(ImGui::GetMousePos().x, PlotTop), ImVec2(ImGui::GetMousePos().x, TimeBarBottom), IM_COL32(128, 128, 128, 64));
                        }

                        if (Config->ShowTimeBarAtGameTime && FCogDebugPlot::Pause)
                        {
                            const float TimeBarX = ImPlot::PlotToPixels(Time, 0.0f).x;
                            PlotDrawList->AddLine(ImVec2(TimeBarX, PlotTop), ImVec2(TimeBarX, TimeBarBottom), IM_COL32(255, 255, 255, 64));
                        }
                        ImPlot::PopPlotClipRect();
                    }

                    //-----------------------------------------------------------
                    // Draw all the plots assigned to this graph
                    //-----------------------------------------------------------
                    for (FCogEngineConfig_Plots_GraphEntryInfo& Entry : GraphInfo.Entries)
                    {
                        FCogDebugHistory* History = FCogDebugPlot::FindEntry(Entry.Name);
                        if (History == nullptr)
                        {
                            continue;
                        }

                        ImPlot::SetAxis(Entry.YAxis);

                        ImPlot::SetNextLineStyle(IMPLOT_AUTO_COL);
                        const auto Label = StringCast<ANSICHAR>(*Entry.Name.ToString());

                        //-------------------------------------------------------
                        // Plot Events
                        //-------------------------------------------------------
                        switch (History->Type)
                        {
                            case FCogDebugHistoryType::Event:
                            {
                                RenderEvents(*static_cast<FCogDebugEventHistory*>(History), Label.Get(), PlotMin, PlotMax);
                                break;
                            }

                            case FCogDebugHistoryType::Value:
                            {
                                RenderValues(*static_cast<FCogDebugValueHistory*>(History), Label.Get());
                                break;
                            }
                        }


                        //-------------------------------------------------------
                        // Allow legend item labels to be drag and drop sources
                        //-------------------------------------------------------
                        if (ImPlot::BeginDragDropSourceItem(Label.Get()))
                        {
                            const auto EntryName = StringCast<ANSICHAR>(*Entry.Name.ToString());
                            ImGui::SetDragDropPayload("DragAndDrop", EntryName.Get(), EntryName.Length() + 1);
                            ImGui::TextUnformatted(EntryName.Get());
                            ImPlot::EndDragDropSource();
                        }
                    }

                    //-------------------------------------------------------
                    // Allow the main plot area to be a drag and drop target
                    //-------------------------------------------------------
                    if (ImPlot::BeginDragDropTargetPlot())
                    {
                        if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("DragAndDrop"))
                        {
                            AssignToGraphAndAxis(GetDroppedEntryName(Payload), GraphIndex, ImAxis_Y1);
                        }
                        ImPlot::EndDragDropTarget();
                    }

                    //-------------------------------------------------------
                    // Allow each y-axis to be a drag and drop target
                    //-------------------------------------------------------
                    for (int32 YAxisIndex = 0; YAxisIndex < Config->NumYAxis; ++YAxisIndex)
                    {
                        const ImAxis YAxis = ImAxis_Y1 + YAxisIndex;
                        if (ImPlot::BeginDragDropTargetAxis(YAxis))
                        {
                            if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("DragAndDrop"))
                            {
                                AssignToGraphAndAxis(GetDroppedEntryName(Payload), GraphIndex, YAxis);
                            }
                            ImPlot::EndDragDropTarget();
                        }
                    }

                    //-------------------------------------------------------
                    // Allow the legend to be a drag and drop target
                    //-------------------------------------------------------
                    if (ImPlot::BeginDragDropTargetLegend())
                    {
                        if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("DragAndDrop"))
                        {
                            AssignToGraphAndAxis(GetDroppedEntryName(Payload), GraphIndex, ImAxis_Y1);
                        }
                        ImPlot::EndDragDropTarget();
                    }

                    ImPlot::EndPlot();
                }

                ImGui::PopID();
            }
            ImPlot::EndSubplots();
        }

        ImPlot::PopStyleVar();

        if (PushPlotBgStyle)
        {
            ImPlot::PopStyleColor();
        }
    }
    ImGui::EndChild();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::RenderValues(FCogDebugValueHistory& Entry, const char* Label) const
{
    if (Entry.Values.empty())
    {
        return;
    }

    //----------------------------------------------------------------
    // Value at cursor tooltip
    //----------------------------------------------------------------
    if (Config->ShowValueAtCursor && ImPlot::IsPlotHovered())
    {
        float Value;
        if (Entry.FindValue(ImPlot::GetPlotMousePos().x, Value))
        {
            if (FCogWindowWidgets::BeginTableTooltip())
            {
                if (ImGui::BeginTable("Params", 2, ImGuiTableFlags_Borders))
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", Label);
                    ImGui::TableNextColumn();
                    ImGui::Text("%0.2f", Value);
                    ImGui::EndTable();
                }
                FCogWindowWidgets::EndTableTooltip();
            }
        }
    }

    if (Entry.ShowValuesMarkers)
    {
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
    }

    ImPlot::PlotLine(Label, &Entry.Values[0].x, &Entry.Values[0].y, Entry.Values.size(), ImPlotLineFlags_None, Entry.ValueOffset, 2 * sizeof(float));

    if (ImPlot::BeginLegendPopup(Label))
    {
        if (ImGui::Button("Clear"))
        {
            Entry.Clear();
        }
        ImGui::Checkbox("Show Markers", &Entry.ShowValuesMarkers);
        ImPlot::EndLegendPopup();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::RenderEvents(FCogDebugEventHistory& Entry, const char* Label, const ImVec2& PlotMin, const ImVec2& PlotMax) const
{
    const ImVec2 Mouse = ImGui::GetMousePos();
    ImDrawList* PlotDrawList = ImPlot::GetPlotDrawList();

    ImPlot::PushPlotClipRect();

    //----------------------------------------------------------------
    // Plot line only to make the plotter move in time and auto scale
    //----------------------------------------------------------------
    ImVector<ImVec2> DummyData;
    DummyData.push_back(ImVec2(0, 0));
    DummyData.push_back(ImVec2(0, 8));
    ImPlot::PlotLine(Label, &DummyData[0].x, &DummyData[0].y, DummyData.size(), Entry.EventOffset, 2 * sizeof(float));

    const FCogDebugPlotEvent* HoveredEvent = nullptr;

    for (const FCogDebugPlotEvent& Event : Entry.Events)
    {
        const ImVec2 PosBot = ImPlot::PlotToPixels(ImPlotPoint(Event.StartTime, Event.Row + 0.8f));
        const ImVec2 PosTop = ImPlot::PlotToPixels(ImPlotPoint(Event.StartTime, Event.Row + 0.2f));
        const ImVec2 PosMid(PosBot.x, PosBot.y + (PosTop.y - PosBot.y) * 0.5f);

        const bool IsInstant = Event.StartTime == Event.EndTime;
        if (IsInstant)
        {
            constexpr float Radius = 10.0f;
            PlotDrawList->AddNgon(PosMid, 10, Event.BorderColor, 4);
            PlotDrawList->AddNgonFilled(PosMid, 10, Event.FillColor, 4);
            PlotDrawList->AddText(ImVec2(PosMid.x + 15, PosMid.y - 6), IM_COL32(255, 255, 255, 255), TCHAR_TO_ANSI(*Event.DisplayName));

            if ((Mouse.x > PosMid.x - Radius) && (Mouse.x < PosMid.x + Radius) && (Mouse.y > PosMid.y - Radius) && (Mouse.y < PosMid.y + Radius))
            {
                HoveredEvent = &Event;
            }
        }
        else
        {
            const float ActualEndTime = Event.GetActualEndTime(Entry);
            const ImVec2 PosEnd = ImPlot::PlotToPixels(ImPlotPoint(ActualEndTime, 0));
            const ImVec2 Min = ImVec2(PosBot.x, PosBot.y);
            const ImVec2 Max = ImVec2(PosEnd.x, PosTop.y);

            const ImDrawFlags Flags = Event.EndTime == 0.0f ? ImDrawFlags_RoundCornersLeft : ImDrawFlags_RoundCornersAll;
            PlotDrawList->AddRect(Min, Max, Event.BorderColor, 6.0f, Flags);
            PlotDrawList->AddRectFilled(Min, Max, Event.FillColor, 6.0f, Flags);
            PlotDrawList->PushClipRect(ImMax(Min, PlotMin), ImMin(Max, PlotMax));
            PlotDrawList->AddText(ImVec2(PosMid.x + 5, PosMid.y - 7), IM_COL32(255, 255, 255, 255), TCHAR_TO_ANSI(*Event.DisplayName));
            PlotDrawList->PopClipRect();

            if (Mouse.x > Min.x && Mouse.x < Max.x && Mouse.y > Min.y && Mouse.y < Max.y)
            {
                HoveredEvent = &Event;
            }
        }
    }

    //-------------------------------------------------------
    // Write info on the graph to help debugging
    //-------------------------------------------------------
    //char Buffer[64];
    //ImFormatString(Buffer, 64, "%0.1f %0.1f", Mouse.x, Mouse.y);
    //PlotDrawList->AddText(ImVec2(PlotMin.x + 50, PlotMin.y + 100), IM_COL32(255, 255, 255, 255), Buffer);

    //-------------------------------------------------------
    // Hovered event tooltip
    //-------------------------------------------------------
    RenderEventTooltip(HoveredEvent, Entry);

    ImPlot::PopPlotClipRect();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::RenderEventTooltip(const FCogDebugPlotEvent* HoveredEvent, const FCogDebugHistory& Entry)
{
    if (ImPlot::IsPlotHovered() && HoveredEvent != nullptr)
    {
        if (FCogWindowWidgets::BeginTableTooltip())
        {
            if (ImGui::BeginTable("Params", 2, ImGuiTableFlags_Borders))
            {
                //------------------------
                // Event Name
                //------------------------
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Name");
                ImGui::TableNextColumn();
                ImGui::Text("%s", TCHAR_TO_ANSI(*HoveredEvent->DisplayName));

                //------------------------
                // Owner Name
                //------------------------
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Owner");
                ImGui::TableNextColumn();
                ImGui::Text("%s", TCHAR_TO_ANSI(*HoveredEvent->OwnerName));

                //------------------------
                // Times
                //------------------------
                if (HoveredEvent->EndTime != HoveredEvent->StartTime)
                {
                    const float ActualEndTime = HoveredEvent->GetActualEndTime(Entry);
                    const uint64 ActualEndFrame = HoveredEvent->GetActualEndFrame(Entry);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Duration");
                    ImGui::TableNextColumn();
                    ImGui::Text("%0.2fs", ActualEndTime - HoveredEvent->StartTime);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Frames");
                    ImGui::TableNextColumn();
                    ImGui::Text("%d  [%d-%d]",
                        static_cast<int32>(ActualEndFrame - HoveredEvent->StartFrame),
                        static_cast<int32>(HoveredEvent->StartFrame % 1000),
                        static_cast<int32>(ActualEndFrame % 1000));
                }
                else
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Frame");
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", static_cast<int32>(HoveredEvent->StartFrame % 1000));
                }

                //------------------------
                // Params
                //------------------------
                for (FCogDebugPlotEventParams Param : HoveredEvent->Params)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", TCHAR_TO_ANSI(*Param.Name.ToString()));
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", TCHAR_TO_ANSI(*Param.Value));
                }
                ImGui::EndTable();
            }
            FCogWindowWidgets::EndTableTooltip();
        }
    }
}


//--------------------------------------------------------------------------------------------------------------------------
FName FCogEngineWindow_Plots::GetDroppedEntryName(const ImGuiPayload* Payload)
{
    return FName(static_cast<const char*>(Payload->Data));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::AssignToGraphAndAxis(const FName InName, const int32 InGraphIndex, const ImAxis InYAxis) 
{
    UnassignToGraphAndAxis(InName);

    FCogDebugHistory* History = FCogDebugPlot::FindEntry(InName);
    if (History == nullptr)
    { return; }

    History->GraphIndex = InGraphIndex;

    FCogEngineConfig_Plots_GraphInfo& GraphInfo = Config->Graphs[InGraphIndex];

    FCogEngineConfig_Plots_GraphEntryInfo* CorrespondingEntry = GraphInfo.Entries.FindByPredicate(
        [InName](const auto& InEntry) { return InEntry.Name == InName; });

    if (CorrespondingEntry == nullptr)
    {
        GraphInfo.Entries.Add({InName, InYAxis});
    }
    else
    {
        CorrespondingEntry->YAxis = InYAxis;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::UnassignToGraphAndAxis(const FName InName)
{
    const FCogDebugHistory* History = FCogDebugPlot::FindEntry(InName);
    if (History == nullptr)
    { return; }

    FCogEngineConfig_Plots_GraphInfo& GraphInfo = Config->Graphs[History->GraphIndex];

    const int32 Index = GraphInfo.Entries.IndexOfByPredicate([InName](const auto& InEntry) { return InEntry.Name == InName; });
    if (Index != INDEX_NONE)
    {
        GraphInfo.Entries.RemoveAt(Index);
    }
}


