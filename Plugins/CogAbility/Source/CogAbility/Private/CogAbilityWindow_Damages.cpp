#include "CogAbilityWindow_Damages.h"

#include "CogInterfaceDamageActor.h"
#include "CogImguiHelper.h"
#include "imgui.h"

//--------------------------------------------------------------------------------------------------------------------------
float FCogDamageStats::MaxDurationSetting = 0.0f;
float FCogDamageStats::RestartDelaySetting = 5.0f;

//--------------------------------------------------------------------------------------------------------------------------
void FCogDamageInstance::Restart()
{
    DamageLast = 0.0f;
    DamageMin = 0.0f;
    DamageMax = 0.0f;
    DamagePerSecond = 0.0f;
    DamagePerFrame = 0.0f;
    DamageTotal = 0.0f;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDamageInstance::AddDamage(const float Damage)
{
    DamageLast = Damage;
    DamageMin = DamageMin == 0.0f ? Damage : FMath::Min(DamageMin, Damage);
    DamageMax = FMath::Max(DamageMax, Damage);
    DamagePerFrame += Damage;
    DamageTotal += Damage;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDamageInstance::UpdateDamagePerSecond(const float Duration)
{
    DamagePerSecond = Duration > 1.0f ? DamageTotal / Duration : DamageTotal;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDamageStats::Restart()
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
void FCogDamageStats::AddDamage(const float InMitigatedDamage, const float InUnmitigatedDamage, const bool bIsCrit)
{
    // If the max duration is reached, stop adding
    if (MaxDuration != 0 && Timer >= MaxDuration)
    {
        return;
    }

    IsInProgress = true;
    Count++;
    Crits += bIsCrit ? 1 : 0;
    MaxDuration = MaxDurationSetting;

    Mitigated.AddDamage(InMitigatedDamage);
    Unmitigated.AddDamage(InUnmitigatedDamage);
    Mitigated.UpdateDamagePerSecond(Timer);
    Unmitigated.UpdateDamagePerSecond(Timer);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDamageStats::Tick(const float DeltaSeconds)
{
    if (IsInProgress)
    {
        // If the max duration is reached, stop increasing time.
        if (MaxDuration <= 0 || Timer < MaxDuration)
        {
            Timer += DeltaSeconds;
        }
        else
        {
            IsInProgress = false;
            Timer = MaxDuration;
            Mitigated.UpdateDamagePerSecond(Timer);
            Unmitigated.UpdateDamagePerSecond(Timer);
        }
    }

    if (RestartDelaySetting > 0.0f)
    {
        if (Unmitigated.DamagePerFrame == 0.0f)
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

    Mitigated.DamagePerFrame = 0.0f;
    Unmitigated.DamagePerFrame = 0.0f;
}

//--------------------------------------------------------------------------------------------------------------------------
static void DrawRow(const char* Title, float MitigatedValue, float UnmitigatedValue, ImVec4 Color)
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
static void DrawDamages(FCogDamageStats& Damage)
{
    ImGui::PushStyleColor(ImGuiCol_FrameBg,         IM_COL32(150, 150, 150, 60));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,  IM_COL32(150, 150, 150, 80));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,   IM_COL32(150, 150, 150, 120));
    ImGui::PushStyleColor(ImGuiCol_CheckMark,       IM_COL32(150, 150, 150, 200));
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram,   IM_COL32(120, 120, 120, 255));

    if (ImGui::BeginTable("Damages", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("");
        ImGui::TableSetupColumn("Mitigated");
        ImGui::TableSetupColumn("Unmitigated");
        ImGui::TableSetupColumn("Mitigation %");
        ImGui::TableHeadersRow();

        DrawRow("Damage Per Second", Damage.Mitigated.DamagePerSecond, Damage.Unmitigated.DamagePerSecond, ImVec4(1.0f, 1.0, 0.0f, 1.0f));
        DrawRow("Damage Total", Damage.Mitigated.DamageTotal, Damage.Unmitigated.DamageTotal, ImVec4(1.0f, 1.0, 1.0f, 1.0f));
        DrawRow("Damage Last", Damage.Mitigated.DamageLast, Damage.Unmitigated.DamageLast, ImVec4(1.0f, 1.0, 1.0f, 1.0f));
        DrawRow("Damage Min", Damage.Mitigated.DamageMin, Damage.Unmitigated.DamageMin, ImVec4(1.0f, 1.0, 1.0f, 1.0f));
        DrawRow("Damage Min", Damage.Mitigated.DamageMax, Damage.Unmitigated.DamageMax, ImVec4(1.0f, 1.0, 1.0f, 1.0f));

        ImGui::EndTable();
    }

    ImGui::Text("Timer");
    ImGui::SameLine(FCogWindowWidgets::TextBaseWidth * 20);
    ImGui::Text("%0.2f", Damage.Timer);

    ImGui::Text("Crits");
    ImGui::SameLine(FCogWindowWidgets::TextBaseWidth * 20);
    FCogWindowWidgets::ProgressBarCentered(Damage.Count == 0 ? 0.0f : Damage.Crits / (float)Damage.Count, ImVec2(-1, 0), TCHAR_TO_ANSI(*FString::Printf(TEXT("%d / %d"), Damage.Crits, Damage.Count)));

    if (FCogDamageStats::MaxDurationSetting > 0)
    {
        ImGui::Text("Timer");
        ImGui::SameLine(FCogWindowWidgets::TextBaseWidth * 20);
        FCogWindowWidgets::ProgressBarCentered(Damage.Timer / Damage.MaxDuration, ImVec2(-1, 0), TCHAR_TO_ANSI(*FString::Printf(TEXT("%0.1f / %0.1f"), Damage.Timer, Damage.MaxDuration)));
    }

    ImGui::Spacing();

    if (ImGui::Button("Restart"))
    {
        Damage.Restart();
    }

    ImGui::PopStyleColor(5);

    ImGui::Spacing();
    ImGui::Spacing();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Damages::RenderContent()
{
    Super::RenderContent();

    AActor* Selection = GetSelection();
    if (Selection == nullptr)
    {
        return;
    }

    if (ImGui::CollapsingHeader("Damage Dealt"))
    {
        ImGui::PushID("Damage Dealt");
        DrawDamages(DamageDealtStats);
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Damage Received"))
    {
        ImGui::PushID("Damage Received");
        DrawDamages(DamageReceivedStats);
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Settings"))
    {
        ImGui::PushItemWidth(-1);

        ImGui::Text("Auto Restart");
        ImGui::SameLine(FCogWindowWidgets::TextBaseWidth * 20);
        bool AutoRestart = FCogDamageStats::RestartDelaySetting > 0;
        if (ImGui::Checkbox("##Auto Restart", &AutoRestart))
        {
            FCogDamageStats::RestartDelaySetting = AutoRestart ? 5.0f : 0.0f;
        }

        if (AutoRestart)
        {
            ImGui::Text("Auto Restart Delay");
            ImGui::SameLine(FCogWindowWidgets::TextBaseWidth * 20);
            ImGui::InputFloat("##Auto Restart Delay", &FCogDamageStats::RestartDelaySetting);
        }

        ImGui::Text("Max Time");
        ImGui::SameLine(FCogWindowWidgets::TextBaseWidth * 20);
        if (ImGui::InputFloat("##Max Time", &FCogDamageStats::MaxDurationSetting, 0.0f, 0.0f, "%0.1f"))
        {
            DamageDealtStats.Restart();
            DamageReceivedStats.Restart();
        }

        ImGui::PopItemWidth();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Damages::OnSelectionChanged(AActor* OldSelection, AActor* NewSelection)
{
    if (ICogInterfaceDamageActor* DamageActor = Cast<ICogInterfaceDamageActor>(OldSelection))
    {
        DamageActor->OnDamageEvent().Remove(OnDamageEventDelegate);
    }

    if (ICogInterfaceDamageActor* DamageActor = Cast<ICogInterfaceDamageActor>(NewSelection))
    {
        OnDamageEventDelegate = DamageActor->OnDamageEvent().AddUObject(this, &UCogAbilityWindow_Damages::OnDamageEvent);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Damages::GameTick(float DeltaSeconds)
{
    Super::GameTick(DeltaSeconds);

    DamageReceivedStats.Tick(DeltaSeconds);
    DamageDealtStats.Tick(DeltaSeconds);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAbilityWindow_Damages::OnDamageEvent(const FCogInterfaceDamageParams& Params)
{
    AActor* Selection = GetSelection();
    if (Params.Type == ECogInterfaceDamageEventType::DamageDealt)
    {
        DamageDealtStats.AddDamage(Params.MitigatedDamage, Params.IncomingDamage, Params.IsCritical);
    }
    else if (Params.Type == ECogInterfaceDamageEventType::DamageReceived)
    {
        DamageReceivedStats.AddDamage(Params.MitigatedDamage, Params.IncomingDamage, Params.IsCritical);
    }
}