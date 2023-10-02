#include "CogEngineWindow_Plots.h"

#include "CogImGuiHelper.h"
#include "CogDebugPlot.h"
#include "CogWindowWidgets.h"
#include "imgui.h"
#include "implot_internal.h"
#include "Kismet/GameplayStatics.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogEngineWindow_Plots::UCogEngineWindow_Plots()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Plots::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);
    FCogDebugPlot::IsVisible = GetIsVisible();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Plots::RenderContent()
{
    Super::RenderContent();

    static int Rows = 1;
    static int Cols = 1;

    if (ImGui::BeginTable("PlotTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
    {
        ImGui::TableSetupColumn("Settings", ImGuiTableColumnFlags_WidthFixed, FCogWindowWidgets::TextBaseWidth * 20.0f);
        ImGui::TableSetupColumn("Graph", ImGuiTableColumnFlags_WidthStretch, 0.0f);
        ImGui::TableNextRow();

        //--------------------------------------------------------------------------------------
        // Settings and Entries
        //--------------------------------------------------------------------------------------
        ImGui::TableNextColumn();
        if (ImGui::Button("Settings"))
        {
            ImGui::OpenPopup("SettingsPopup");
        }
        ImGui::SameLine();
        FCogWindowWidgets::ToggleButton(&FCogDebugPlot::Pause, "Pause", "Pause", ImVec4(1.0f, 0.0f, 0.0f, 1.0f), ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

        if (ImGui::BeginPopup("SettingsPopup"))
        {
            ImGui::SliderInt("Rows", &Rows, 1, 5);

            if (ImGui::Button("Clear Data", ImVec2(-1, 0)))
            {
                FCogDebugPlot::Clear();
            }

            if (ImGui::Button("Reset Layout", ImVec2(-1, 0)))
            {
                FCogDebugPlot::Pause = false;
                Rows = 1;

                FCogDebugPlot::Reset();
            }

            ImGui::EndPopup();
        }

        if (ImGui::BeginChild("Separator", ImVec2(0, 2)))
        {
            ImGui::Separator();
        }
        ImGui::EndChild();

        TArray<FCogDebugPlotEntry*> VisiblePlots;
        if (ImGui::BeginChild("Plots", ImVec2(0, -1)))
        {
            int Index = 0;

            for (FCogDebugPlotEntry& Plot : FCogDebugPlot::Plots)
            {
                const auto Label = StringCast<ANSICHAR>(*Plot.Name.ToString());

                if (Plot.CurrentYAxis != ImAxis_COUNT && Plot.CurrentRow != INDEX_NONE)
                {
                    VisiblePlots.Add(&Plot);
                }

                ImGui::PushID(Index);

                ImGui::PushStyleColor(ImGuiCol_Text, Plot.IsEventPlot ? IM_COL32(128, 128, 255, 255) : IM_COL32(255, 255, 255, 255));
                ImGui::Selectable(Label.Get(), false, 0);
                ImGui::PopStyleColor();

                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    ImGui::SetDragDropPayload("DragAndDrop", Label.Get(), Label.Length() + 1);
                    ImGui::TextUnformatted(Label.Get());
                    ImGui::EndDragDropSource();
                }

                ImGui::PopID();
                Index++;
            }
        }
        ImGui::EndChild();

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("DragAndDrop"))
            {
                if (FCogDebugPlotEntry* Plot = FCogDebugPlot::FindPlot(FName((const char*)Payload->Data)))
                {
                    Plot->ResetAxis();
                }
            }
            ImGui::EndDragDropTarget();
        }

        //--------------------------------------------------------------------------------------
        // Graph
        //--------------------------------------------------------------------------------------
        ImGui::TableNextColumn();
        if (ImGui::BeginChild("Graph", ImVec2(0, -1)))
        {
            static float RowRatios[] = { 1, 1, 1, 1, 1, 1 };
            static float ColRatios[] = { 1 };
            static ImPlotSubplotFlags SubplotsFlags = ImPlotSubplotFlags_LinkCols;
            if (ImPlot::BeginSubplots("", Rows, Cols, ImVec2(-1, -1), SubplotsFlags, RowRatios, ColRatios))
            {
                for (int PlotIndex = 0; PlotIndex < Rows; ++PlotIndex)
                {
                    if (ImPlot::BeginPlot("##Plot", ImVec2(-1, 250)))
                    {
                        ImPlotAxisFlags HasPlotOnAxisY1 = false;
                        ImPlotAxisFlags HasPlotOnAxisY2 = false;
                        ImPlotAxisFlags HasPlotOnAxisY3 = false;

                        for (FCogDebugPlotEntry* PlotPtr : VisiblePlots)
                        {
                            HasPlotOnAxisY1 |= PlotPtr->CurrentYAxis == ImAxis_Y1 && PlotPtr->CurrentRow == PlotIndex;
                            HasPlotOnAxisY2 |= PlotPtr->CurrentYAxis == ImAxis_Y2 && PlotPtr->CurrentRow == PlotIndex;
                            HasPlotOnAxisY3 |= PlotPtr->CurrentYAxis == ImAxis_Y3 && PlotPtr->CurrentRow == PlotIndex;
                        }

                        ImPlot::SetupAxis(ImAxis_X1, NULL, ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines);
                        ImPlot::SetupAxis(ImAxis_Y1, HasPlotOnAxisY1 ? "" : "[drop here]", (HasPlotOnAxisY1 ? ImPlotAxisFlags_None : (ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines)) | ImPlotAxisFlags_AutoFit);
                        ImPlot::SetupAxis(ImAxis_Y2, HasPlotOnAxisY2 ? "" : "[drop here]", (HasPlotOnAxisY2 ? ImPlotAxisFlags_None : (ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines)) | ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_Opposite);
                        ImPlot::SetupAxis(ImAxis_Y3, HasPlotOnAxisY3 ? "" : "[drop here]", (HasPlotOnAxisY3 ? ImPlotAxisFlags_None : (ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines)) | ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_Opposite);

                        //--------------------------------------------------------------------------------------------------
                        // Set the initial X axis range. After it is automatically updated to move with the current time.
                        //--------------------------------------------------------------------------------------------------
                        ImPlot::SetupAxisLimits(ImAxis_X1, 0, 10.0f, ImGuiCond_Appearing);

                        const ImPlotRange& Range = ImPlot::GetCurrentPlot()->Axes[ImAxis_X1].Range;
                        const float AxisXRange = Range.Max - Range.Min;

                        //----------------------------------------------------------------
                        // Draw a vertical lines representing the current time and the mouse time
                        //----------------------------------------------------------------
                        ImDrawList* PlotDrawList = ImPlot::GetPlotDrawList();
                        const ImVec2 PlotMin = ImPlot::GetPlotPos();
                        const ImVec2 PlotSize = ImPlot::GetPlotSize();
                        const ImVec2 PlotMax = ImVec2(PlotMin.x + PlotSize.x, PlotMin.y + PlotSize.y);
                        const float PlotTop = PlotMin.y;
                        const float TimeBarBottom = PlotTop + PlotSize.y;

                        ImPlot::PushPlotClipRect();
                        PlotDrawList->AddLine(ImVec2(ImGui::GetMousePos().x, PlotTop), ImVec2(ImGui::GetMousePos().x, TimeBarBottom), IM_COL32(128, 128, 128, 64));
                        if (FCogDebugPlot::Pause)
                        {
                            const float Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
                            const float TimeBarX = ImPlot::PlotToPixels(Time, 0.0f).x;
                            PlotDrawList->AddLine(ImVec2(TimeBarX, PlotTop), ImVec2(TimeBarX, TimeBarBottom), IM_COL32(255, 255, 255, 64));
                        }
                        ImPlot::PopPlotClipRect();

                        for (FCogDebugPlotEntry* PlotPtr : VisiblePlots)
                        {
                            if (PlotPtr == nullptr)
                            {
                                continue;
                            }

                            FCogDebugPlotEntry& Entry = *PlotPtr;

                            if (Entry.CurrentRow == PlotIndex)
                            {
                                //--------------------------------------------------------------------------------
                                // Make the time axis move forward automatically, unless the user pauses or zoom.
                                //--------------------------------------------------------------------------------
                                if (FCogDebugPlot::Pause == false && ImGui::GetIO().MouseWheel == 0)
                                {
                                    ImPlot::SetupAxisLimits(ImAxis_X1, Entry.Time - AxisXRange, Entry.Time, ImGuiCond_Always);
                                }

                                ImPlot::SetAxis(Entry.CurrentYAxis);

                                ImPlot::SetNextLineStyle(IMPLOT_AUTO_COL);
                                const auto Label = StringCast<ANSICHAR>(*Entry.Name.ToString());

                                //----------------------------------------------------------------
                                // Pause the scrolling if the user drag inside
                                //----------------------------------------------------------------
                                ImVec2 Mouse = ImGui::GetMousePos();
                                if (Mouse.x > PlotMin.x
                                    && Mouse.y > PlotMin.y
                                    && Mouse.x < PlotMax.x
                                    && Mouse.y < PlotMax.y
                                    && ImGui::GetDragDropPayload() == nullptr)
                                {
                                    ImVec2 Drag = ImGui::GetMouseDragDelta(0);
                                    if (FMath::Abs(Drag.x) > 10)
                                    {
                                        FCogDebugPlot::Pause = true;
                                    }
                                }

                                //-------------------------------------------------------
                                // Plot Events
                                //-------------------------------------------------------
                                const bool IsEventPlot = Entry.Events.Num() > 0;
                                if (IsEventPlot)
                                {
                                    //--------------------------------------------------------------------
                                    // Update plot time for events as events are not pushed every frames
                                    //--------------------------------------------------------------------
                                    Entry.UpdateTime(GetWorld());

                                    ImPlot::SetupAxisLimits(Entry.CurrentYAxis, 0, Entry.MaxRow + 2, ImGuiCond_Always);

                                    ImPlot::PushPlotClipRect();


                                    //----------------------------------------------------------------
                                    // Plot line only to make the plotter move in time and auto scale
                                    //----------------------------------------------------------------
                                    ImVector<ImVec2> DummyData;
                                    DummyData.push_back(ImVec2(0, 0));
                                    DummyData.push_back(ImVec2(0, 8));
                                    ImPlot::PlotLine(Label.Get(), &DummyData[0].x, &DummyData[0].y, DummyData.size(), Entry.ValueOffset, 2 * sizeof(float));

                                    const FCogDebugPlotEvent* HoveredEvent = nullptr;

                                    for (const FCogDebugPlotEvent& Event : Entry.Events)
                                    {
                                        const ImVec2 PosBot = ImPlot::PlotToPixels(ImPlotPoint(Event.StartTime, Event.Row + 0.8f));
                                        const ImVec2 PosTop = ImPlot::PlotToPixels(ImPlotPoint(Event.StartTime, Event.Row + 0.2f));
                                        const ImVec2 PosMid(PosBot.x, PosBot.y + (PosTop.y - PosBot.y) * 0.5f);

                                        const bool IsInstant = Event.StartTime == Event.EndTime;
                                        if (IsInstant)
                                        {
                                            const float Radius = 10.0f;
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

                                            ImDrawFlags Flags = Event.EndTime == 0.0f ? ImDrawFlags_RoundCornersLeft : ImDrawFlags_RoundCornersAll;
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
                                    if (ImPlot::IsPlotHovered() && HoveredEvent != nullptr)
                                    {
                                        FCogWindowWidgets::BeginTableTooltip();
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
                                    
                                    ImPlot::PopPlotClipRect();
                                }
                                //-------------------------------------------------------
                                // Plot Values
                                //-------------------------------------------------------
                                else
                                {
                                    //----------------------------------------------------------------
                                    // Custom tooltip
                                    //----------------------------------------------------------------
                                    if (ImPlot::IsPlotHovered())
                                    {
                                        float Value;
                                        if (Entry.FindValue(ImPlot::GetPlotMousePos().x, Value))
                                        {
                                            ImGui::BeginTooltip();
                                            ImGui::Text("%s: %0.1f", Label.Get(), Value);
                                            ImGui::EndTooltip();
                                        }
                                    }

                                    if (Entry.ShowValuesMarkers)
                                    {
                                        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
                                    }

                                    ImPlot::PlotLine(Label.Get(), &Entry.Values[0].x, &Entry.Values[0].y, Entry.Values.size(), ImPlotLineFlags_None, Entry.ValueOffset, 2 * sizeof(float));

                                    if (ImPlot::BeginLegendPopup(Label.Get()))
                                    {
                                        if (ImGui::Button("Clear"))
                                        {
                                            Entry.Clear();
                                        }
                                        ImGui::Checkbox("Show Markers", &Entry.ShowValuesMarkers);
                                        ImPlot::EndLegendPopup();
                                    }
                                }

                                //-------------------------------------------------------
                                // Allow legend item labels to be drag and drop sources
                                //-------------------------------------------------------
                                if (ImPlot::BeginDragDropSourceItem(Label.Get()))
                                {
                                    ImGui::SetDragDropPayload("DragAndDrop", Label.Get(), Label.Length() + 1);
                                    ImGui::TextUnformatted(Label.Get());
                                    ImPlot::EndDragDropSource();
                                }
                            }

                        }

                        //-------------------------------------------------------
                        // Allow the main plot area to be a drag and drop target
                        //-------------------------------------------------------
                        if (ImPlot::BeginDragDropTargetPlot())
                        {
                            if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("DragAndDrop"))
                            {
                                if (FCogDebugPlotEntry* Plot = FCogDebugPlot::FindPlot(FName((const char*)Payload->Data)))
                                {
                                    Plot->AssignAxis(PlotIndex, ImAxis_Y1);
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
                                    if (FCogDebugPlotEntry* Plot = FCogDebugPlot::FindPlot(FName((const char*)Payload->Data)))
                                    {
                                        Plot->AssignAxis(PlotIndex, y);
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
                                if (FCogDebugPlotEntry* Plot = FCogDebugPlot::FindPlot(FName((const char*)Payload->Data)))
                                {
                                    Plot->AssignAxis(PlotIndex, ImAxis_Y1);
                                }
                            }
                            ImPlot::EndDragDropTarget();
                        }

                        ImPlot::EndPlot();
                    }
                }
                ImPlot::EndSubplots();
            }
        }
        ImGui::EndChild();

        ImGui::EndTable();
    }
}