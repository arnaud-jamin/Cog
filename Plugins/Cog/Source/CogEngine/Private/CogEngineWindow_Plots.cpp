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

    Config->Reset();
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

    TArray<FCogDebugPlotEntry*> VisiblePlots;
    for (FCogDebugPlotEntry& Plot : FCogDebugPlot::Plots)
    {
        if (Plot.YAxis != ImAxis_COUNT && Plot.GraphIndex != INDEX_NONE)
        {
            VisiblePlots.Add(&Plot);
        }
    }

    for (FCogDebugPlotEntry& Event : FCogDebugPlot::Events)
    {
        if (Event.YAxis != ImAxis_COUNT && Event.GraphIndex != INDEX_NONE)
        {
            VisiblePlots.Add(&Event);
        }
    }

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
            RenderPlots(VisiblePlots);

            ImGui::EndTable();
        }
        
    }
    else
    {
        RenderPlots(VisiblePlots);
    }

    bApplyTimeScale = false;
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
            if (ImGui::MenuItem("Reset"))
            {
                FCogDebugPlot::Pause = false;
                FCogDebugPlot::Reset();
                ResetConfig();
            }

            ImGui::Separator();

            FCogWindowWidgets::SetNextItemToShortWidth();
            if (ImGui::SliderInt("Num Graphs", &Config->NumGraphs, 1, 5))
            {
                bApplyTimeScale = true;
            }

            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::SliderInt("Num YAxis", &Config->NumYAxis, 0, 3);

            FCogWindowWidgets::SetNextItemToShortWidth();
            if (ImGui::SliderFloat("Time range", &Config->TimeRange, 1.0f, 100.0f, "%0.1f"))
            {
                bApplyTimeScale = true;
            }

            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::SliderFloat("Drag pause sensitivity", &Config->DragPauseSensitivity, 1.0f, 50.0f, "%0.0f");

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
void FCogEngineWindow_Plots::RenderEntryName(const int Index, FCogDebugPlotEntry& Entry)
{
    ImGui::PushID(Index);

    const bool IsAssignedToRow = Entry.GraphIndex != INDEX_NONE;
    if (ImGui::Selectable(TCHAR_TO_ANSI(*Entry.Name.ToString()), IsAssignedToRow, ImGuiSelectableFlags_AllowDoubleClick))
    {
        if (IsAssignedToRow)
        {
            Entry.ResetGraphAndAxis();
        }
        else
        {
            Entry.AssignGraphAndAxis(0, ImAxis_Y1);
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
    if (ImGui::BeginChild("Entries", InSize))
    {
        if (Config->DockEntries)
        {
            ImGui::Indent(6);
        }

        int Index = 0;

        if (FCogWindowWidgets::DarkCollapsingHeader("Events", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (FCogDebugPlot::Events.IsEmpty())
            {
                ImGui::TextDisabled("No event added yet");
            }
            else
            {
                for (FCogDebugPlotEntry& Event : FCogDebugPlot::Events)
                {
                    RenderEntryName(Index, Event);
                    Index++;
                }
            }
        }

        if (FCogWindowWidgets::DarkCollapsingHeader("Plots", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (FCogDebugPlot::Plots.IsEmpty())
            {
                ImGui::TextDisabled("No plot added yet");
            }
            else
            {
                for (FCogDebugPlotEntry& Plot : FCogDebugPlot::Plots)
                {
                    RenderEntryName(Index, Plot);
                    Index++;
                }
            }
        }

        if (Config->DockEntries)
        {
            ImGui::Unindent();
        }
    }
    ImGui::EndChild();

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("DragAndDrop"))
        {
            if (FCogDebugPlotEntry* Plot = FCogDebugPlot::FindEntry(FName((const char*)Payload->Data)))
            {
                Plot->ResetGraphAndAxis();
            }
        }
        ImGui::EndDragDropTarget();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::RenderPlots(const TArray<FCogDebugPlotEntry*>& VisiblePlots) const
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

        if (ImPlot::BeginSubplots("", Config->NumGraphs, 1, ImVec2(-1, -1), SubplotsFlags, RowRatios, ColRatios))
        {
            for (int PlotIndex = 0; PlotIndex < Config->NumGraphs; ++PlotIndex)
            {
                if (ImPlot::BeginPlot("##Plot", ImVec2(-1, 250)))
                {
                    ImPlotAxisFlags HasPlotOnAxisY1 = false;
                    ImPlotAxisFlags HasPlotOnAxisY2 = false;
                    ImPlotAxisFlags HasPlotOnAxisY3 = false;

                    for (const FCogDebugPlotEntry* PlotPtr : VisiblePlots)
                    {
                        HasPlotOnAxisY1 |= PlotPtr->YAxis == ImAxis_Y1 && PlotPtr->GraphIndex == PlotIndex;
                        HasPlotOnAxisY2 |= PlotPtr->YAxis == ImAxis_Y2 && PlotPtr->GraphIndex == PlotIndex;
                        HasPlotOnAxisY3 |= PlotPtr->YAxis == ImAxis_Y3 && PlotPtr->GraphIndex == PlotIndex;
                    }

                    ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines);

                    if (Config->NumYAxis > 0)
                    {
                		ImPlot::SetupAxis(ImAxis_Y1, HasPlotOnAxisY1 ? "" : "[drop here]", (HasPlotOnAxisY1 ? ImPlotAxisFlags_None : (ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines)) | ImPlotAxisFlags_AutoFit);
                    }

                    if (Config->NumYAxis > 1)
                    {
                		ImPlot::SetupAxis(ImAxis_Y2, HasPlotOnAxisY2 ? "" : "[drop here]", (HasPlotOnAxisY2 ? ImPlotAxisFlags_None : (ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines)) | ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_Opposite);
					}

                    if (Config->NumYAxis > 2)
                    {
                		ImPlot::SetupAxis(ImAxis_Y3, HasPlotOnAxisY3 ? "" : "[drop here]", (HasPlotOnAxisY3 ? ImPlotAxisFlags_None : (ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines)) | ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_Opposite);
					}

                    //--------------------------------------------------------------------------------------------------
                    // Set the initial X axis range. After, it is automatically updated to move with the current time.
                    //--------------------------------------------------------------------------------------------------
                    ImPlot::SetupAxisLimits(ImAxis_X1, 0, Config->TimeRange, ImGuiCond_Appearing);

                    const ImPlotRange& PlotRange = ImPlot::GetCurrentPlot()->Axes[ImAxis_X1].Range;
                    const float TimeRange = PlotRange.Max - PlotRange.Min;

                    if (bApplyTimeScale == false)
                    {
                        Config->TimeRange = TimeRange;
                    }

                    const float Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

                    //------------------------------------------------------------------
                    // Setup all the Z and Y axis limits. Must be done before calling
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

                        //--------------------------------------------------------------------------------
                        // Set the Y axis limit for Events. 
                        //--------------------------------------------------------------------------------
                        for (const FCogDebugPlotEntry* PlotPtr : VisiblePlots)
                        {
                            if (PlotPtr == nullptr)
                            { continue; }

                            if (PlotPtr->GraphIndex != PlotIndex)
                            { continue; }

                            if (PlotPtr->IsEventPlot)
                            {
                                ImPlot::SetupAxisLimits(PlotPtr->YAxis, 0, PlotPtr->MaxRow + 2, ImGuiCond_Always);
                            }
                        }
                    }

                    const ImVec2 PlotMin = ImPlot::GetPlotPos();
                    const ImVec2 PlotSize = ImPlot::GetPlotSize();
                    const ImVec2 PlotMax = PlotMin + PlotSize;

                    //----------------------------------------------------------------
                    // Pause the scrolling if the user drag inside
                    //----------------------------------------------------------------
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
                    // Draw all the plots assigned to this row
                    //-----------------------------------------------------------
                    for (FCogDebugPlotEntry* PlotPtr : VisiblePlots)
                    {
                        if (PlotPtr == nullptr)
                        { continue; }

                        FCogDebugPlotEntry& Plot = *PlotPtr;
                        if (Plot.GraphIndex != PlotIndex)
                        { continue; }

                        ImPlot::SetAxis(Plot.YAxis);

                        ImPlot::SetNextLineStyle(IMPLOT_AUTO_COL);
                        const auto Label = StringCast<ANSICHAR>(*Plot.Name.ToString());

                        //-------------------------------------------------------
                        // Plot Events
                        //-------------------------------------------------------
                        if (Plot.IsEventPlot)
                        {
                            RenderEvents(Plot, Label.Get(), PlotMin, PlotMax);
                        }
                        //-------------------------------------------------------
                        // Plot Values
                        //-------------------------------------------------------
                        else if (Plot.Values.empty() == false)
                        {
                            RenderValues(Plot, Label.Get());
                        }

                        //-------------------------------------------------------
                        // Allow legend item labels to be drag and drop sources
                        //-------------------------------------------------------
                        if (ImPlot::BeginDragDropSourceItem(Label.Get()))
                        {
                            const auto EntryName = StringCast<ANSICHAR>(*Plot.Name.ToString());
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
                            if (FCogDebugPlotEntry* Plot = FCogDebugPlot::FindEntry(FName((const char*)Payload->Data)))
                            {
                                Plot->AssignGraphAndAxis(PlotIndex, ImAxis_Y1);
                            }
                        }
                        ImPlot::EndDragDropTarget();
                    }

                    //-------------------------------------------------------
                    // Allow each y-axis to be a drag and drop target
                    //-------------------------------------------------------
                    for (int y = ImAxis_Y1; y <= ImAxis_Y3; ++y)
                    {
                        if (ImPlot::BeginDragDropTargetAxis(y))
                        {
                            if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("DragAndDrop"))
                            {
                                if (FCogDebugPlotEntry* Plot = FCogDebugPlot::FindEntry(FName((const char*)Payload->Data)))
                                {
                                    Plot->AssignGraphAndAxis(PlotIndex, y);
                                }
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
                            if (FCogDebugPlotEntry* Plot = FCogDebugPlot::FindEntry(FName((const char*)Payload->Data)))
                            {
                                Plot->AssignGraphAndAxis(PlotIndex, ImAxis_Y1);
                            }
                        }
                        ImPlot::EndDragDropTarget();
                    }

                    ImPlot::EndPlot();
                }
            }
            ImPlot::EndSubplots();
        }

        if (PushPlotBgStyle)
        {
            ImPlot::PopStyleColor();
        }
    }
    ImGui::EndChild();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Plots::RenderValues(FCogDebugPlotEntry& Entry, const char* Label) const
{
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
void FCogEngineWindow_Plots::RenderEvents(FCogDebugPlotEntry& Entry, const char* Label, const ImVec2& PlotMin, const ImVec2& PlotMax) const
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
    ImPlot::PlotLine(Label, &DummyData[0].x, &DummyData[0].y, DummyData.size(), Entry.ValueOffset, 2 * sizeof(float));

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
void FCogEngineWindow_Plots::RenderEventTooltip(const FCogDebugPlotEvent* HoveredEvent, const FCogDebugPlotEntry& Entry)
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
                        (int32)(ActualEndFrame - HoveredEvent->StartFrame),
                        (int32)(HoveredEvent->StartFrame % 1000),
                        (int32)(ActualEndFrame % 1000));
                }
                else
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Frame");
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", (int32)(HoveredEvent->StartFrame % 1000));
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
