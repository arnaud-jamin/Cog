#include "CogAbilityWindow_Metrics.h"

#include "CogInterfaceMetricActor.h"
#include "CogImguiHelper.h"
#include "imgui.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogMetricInstance::Restart()
{
    Last = 0.0f;
    Min = 0.0f;
    Max = 0.0f;
    PerSecond = 0.0f;
    PerFrame = 0.0f;
    Total = 0.0f;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogMetricInstance::AddMetric(const float Metric)
{
    Last = Metric;
    Min = Min == 0.0f ? Metric : FMath::Min(Min, Metric);
    Max = FMath::Max(Max, Metric);
    PerFrame += Metric;
    Total += Metric;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogMetricInstance::UpdateMetricPerSecond(const float Duration)
{
    PerSecond = Duration > 1.0f ? Total / Duration : Total;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogMetricInfo::Restart()
{
    Count = 0;
    Crits = 0;
    TotalCritChances = 0.0f;

    IsInProgress = false;
    Timer = 0.0f;
    RestartTimer = 0.0f;

    Mitigated.Restart();
    Unmitigated.Restart();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogMetricInfo::AddEvent(const FCogInterfaceMetricEventParams& Params)
{
    // If the max duration is reached, stop adding
    if (MaxDurationSetting != 0 && Timer >= MaxDurationSetting)
    {
        return;
    }

    IsInProgress = true;
    Count++;
    Crits += Params.IsCritical ? 1 : 0;

    Mitigated.AddMetric(Params.MitigatedValue);
    Unmitigated.AddMetric(Params.UnmitigatedValue);
    Mitigated.UpdateMetricPerSecond(Timer);
    Unmitigated.UpdateMetricPerSecond(Timer);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogMetricInfo::Tick(const float DeltaSeconds)
{
    if (IsInProgress)
    {
        // If the max duration is reached, stop increasing time.
        if (MaxDurationSetting <= 0 || Timer < MaxDurationSetting)
        {
            Timer += DeltaSeconds;
        }
        else
        {
            IsInProgress = false;
            Timer = MaxDurationSetting;
            Mitigated.UpdateMetricPerSecond(Timer);
            Unmitigated.UpdateMetricPerSecond(Timer);
        }
    }

    if (RestartDelaySetting > 0.0f)
    {
        if (Unmitigated.PerFrame == 0.0f)
        {
            RestartTimer += DeltaSeconds;
            if (RestartTimer > RestartDelaySetting)
            {
                Restart();
            }
        }
        else
        {
            RestartTimer = 0.0f;
        }
    }

    Mitigated.PerFrame = 0.0f;
    Unmitigated.PerFrame = 0.0f;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Metrics::DrawMetricRow(const char* Title, float MitigatedValue, float UnmitigatedValue, const ImVec4& Color)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Selectable(Title, false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_AllowDoubleClick);
    ImGui::TableNextColumn();
    ImGui::TextColored(Color, "%.1f", MitigatedValue);
    ImGui::TableNextColumn();
    ImGui::Text("%.1f", UnmitigatedValue);
    ImGui::TableNextColumn();
    ImGui::Text("%.0f%%", UnmitigatedValue <= 0 ? 0.0 : 100.0f * (1.0f - (MitigatedValue / UnmitigatedValue)));
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Metrics::DrawMetrics(FCogMetricInfo& Metric)
{
    FCogWindowWidgets::PushBackColor(ImVec4(0.8f, 0.8f, 0.8f, 1.0f));

    if (ImGui::BeginTable("MetricTable", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("");
        ImGui::TableSetupColumn("Mitigated");
        ImGui::TableSetupColumn("Unmitigated");
        ImGui::TableSetupColumn("Mitigation %");
        ImGui::TableHeadersRow();

        DrawMetricRow("Per Second", Metric.Mitigated.PerSecond, Metric.Unmitigated.PerSecond, ImVec4(1.0f, 1.0, 0.0f, 1.0f));
        DrawMetricRow("Total", Metric.Mitigated.Total, Metric.Unmitigated.Total, ImVec4(1.0f, 1.0, 1.0f, 1.0f));
        DrawMetricRow("Last", Metric.Mitigated.Last, Metric.Unmitigated.Last, ImVec4(1.0f, 1.0, 1.0f, 1.0f));
        DrawMetricRow("Min", Metric.Mitigated.Min, Metric.Unmitigated.Min, ImVec4(1.0f, 1.0, 1.0f, 1.0f));
        DrawMetricRow("Min", Metric.Mitigated.Max, Metric.Unmitigated.Max, ImVec4(1.0f, 1.0, 1.0f, 1.0f));

        ImGui::EndTable();
    }

    ImGui::Text("Crits");
    ImGui::SameLine(FCogWindowWidgets::TextBaseWidth * 20);
    FCogWindowWidgets::ProgressBarCentered(Metric.Count == 0 ? 0.0f : Metric.Crits / (float)Metric.Count, ImVec2(-1, 0), TCHAR_TO_ANSI(*FString::Printf(TEXT("%d / %d"), Metric.Crits, Metric.Count)));

    if (Metric.MaxDurationSetting > 0.0f)
    {
        ImGui::Text("Timer");
        ImGui::SameLine(FCogWindowWidgets::TextBaseWidth * 20);
        FCogWindowWidgets::ProgressBarCentered(Metric.Timer / (float)Metric.MaxDurationSetting, ImVec2(-1, 0), TCHAR_TO_ANSI(*FString::Printf(TEXT("%0.1f / %0.1f"), Metric.Timer, Metric.MaxDurationSetting)));
    }
    else
    {
        ImGui::Text("Timer");
        ImGui::SameLine(FCogWindowWidgets::TextBaseWidth * 20);
        ImGui::Text("%0.1f", Metric.Timer);
    }

    ImGui::Spacing();

    if (ImGui::Button("Restart"))
    {
        Metric.Restart();
    }

    FCogWindowWidgets::PopBackColor();

    ImGui::Spacing();
    ImGui::Spacing();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Metrics::RenderContent()
{
    Super::RenderContent();

    AActor* Selection = GetSelection();
    if (Selection == nullptr)
    {
        return;
    }

    int32 Index = 0; 
    for (auto& Entry : Metrics)
    {
        FName MetricName = Entry.Key;
        FCogMetricInfo& Metric = Entry.Value;

        if (ImGui::CollapsingHeader(TCHAR_TO_ANSI(*MetricName.ToString())))
        {
            ImGui::PushID(Index);
            DrawMetrics(Metric);
            ImGui::PopID();
        }
        
        Index++;
    }

    if (ImGui::CollapsingHeader("Settings"))
    {
        ImGui::PushItemWidth(-1);

        bool Reset = false;

        ImGui::Text("Auto Restart");
        ImGui::SameLine(FCogWindowWidgets::TextBaseWidth * 20);
        bool AutoRestart = RestartDelaySetting > 0;
        if (ImGui::Checkbox("##Auto Restart", &AutoRestart))
        {
            RestartDelaySetting = AutoRestart ? 5.0f : 0.0f;
            Reset = true;
        }

        if (AutoRestart)
        {
            ImGui::Text("Auto Restart Delay");
            ImGui::SameLine(FCogWindowWidgets::TextBaseWidth * 20);
            if (ImGui::InputFloat("##Auto Restart Delay", &RestartDelaySetting))
            {
                Reset = true;
            }
        }

        ImGui::Text("Max Time");
        ImGui::SameLine(FCogWindowWidgets::TextBaseWidth * 20);
        if (ImGui::InputFloat("##Max Time", &MaxDurationSetting, 0.0f, 0.0f, "%0.1f"))
        {
            Reset = true;
        }

        if (Reset)
        {
            for (auto& Entry : Metrics)
            {
                FCogMetricInfo& Metric = Entry.Value;

                Metric.MaxDurationSetting = MaxDurationSetting;
                Metric.RestartDelaySetting = RestartDelaySetting;
                Metric.Restart();
            }
        }

        ImGui::PopItemWidth();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Metrics::OnSelectionChanged(AActor* OldSelection, AActor* NewSelection)
{
    if (ICogInterfaceMetricActor* DamageActor = Cast<ICogInterfaceMetricActor>(OldSelection))
    {
        DamageActor->OnMetricEvent().Remove(OnMetricDelegate);
    }

    if (ICogInterfaceMetricActor* DamageActor = Cast<ICogInterfaceMetricActor>(NewSelection))
    {
        OnMetricDelegate = DamageActor->OnMetricEvent().AddUObject(this, &UCogAbilityWindow_Metrics::OnEvent);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Metrics::GameTick(float DeltaSeconds)
{
    Super::GameTick(DeltaSeconds);

    for (auto& Entry : Metrics)
    {
        FCogMetricInfo& Metric = Entry.Value;
        Metric.Tick(DeltaSeconds);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Metrics::OnEvent(const FCogInterfaceMetricEventParams& Params)
{
    FCogMetricInfo& MetricInfo = Metrics.FindOrAdd(Params.Name);

    MetricInfo.AddEvent(Params);
}