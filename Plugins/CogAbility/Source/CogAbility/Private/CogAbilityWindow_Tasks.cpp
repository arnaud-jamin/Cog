#include "CogAbilityWindow_Tasks.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "CogAbilityHelper.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "imgui.h"

class UCogAbilityConfig_Tasks;
//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    Config = GetConfig<UCogAbilityConfig_Tasks>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::RenderHelp()
{
    ImGui::Text(
    "This window displays the gameplay tasks. ");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::PreBegin(ImGuiWindowFlags& WindowFlags)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::PostBegin()
{
    ImGui::PopStyleVar();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::RenderContent()
{
    Super::RenderContent();

    AActor* Selection = GetSelection();
    if (Selection == nullptr)
    {
        ImGui::TextDisabled("Invalid selection");
        return;
    }

    UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Selection, true);
    if (AbilitySystemComponent == nullptr)
    {
        ImGui::TextDisabled("Selection has no ability system component");
        return;
    }

    RenderTaskMenu(Selection);

    RenderTasksTable(*AbilitySystemComponent);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::RenderTaskMenu(AActor* Selection)
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Sort by Name", &Config->SortByName);
            ImGui::Checkbox("Show Uninitialized", &Config->ShowUninitialized);
            ImGui::Checkbox("Show Awaiting Activation", &Config->ShowAwaitingActivation);
            ImGui::Checkbox("Show Active", &Config->ShowActive);
            ImGui::Checkbox("Show Paused", &Config->ShowPaused);
            ImGui::Checkbox("Show Finished", &Config->ShowFinished);
            ImGui::Checkbox("Show Ticking", &Config->ShowTicking);
            ImGui::Checkbox("Show Simulating", &Config->ShowSimulating);

            ImGui::Separator();

            ImGui::ColorEdit4("Uninitialized Color", &Config->UninitializedColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Awaiting Activation Color", &Config->AwaitingActivationColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Active Color", &Config->ActiveColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Paused Color", &Config->PausedColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Finished Color", &Config->FinishedColor.X, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);

            ImGui::Separator();

            if (ImGui::MenuItem("Reset"))
            {
                ResetConfig();
            }

            ImGui::EndMenu();
        }

        FCogWidgets::SearchBar("##Filter", Filter);

        ImGui::EndMenuBar();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::RenderTasksTable(UAbilitySystemComponent& AbilitySystemComponent)
{
    TArray<const UGameplayTask*> FilteredTasks;
    FilteredTasks.Reserve(16);

    for (FConstGameplayTaskIterator it = AbilitySystemComponent.GetKnownTaskIterator(); it; ++it)
    {
        const UGameplayTask* Task = Cast<const UGameplayTask>(*it);
        if (Task == nullptr)
        {
            continue;
        }

        const EGameplayTaskState TaskState = Task->GetState();

        if (Config->ShowUninitialized == false && TaskState == EGameplayTaskState::Uninitialized)
        {
            continue;
        }

        if (Config->ShowAwaitingActivation == false && TaskState == EGameplayTaskState::AwaitingActivation)
        {
            continue;
        }

        if (Config->ShowActive == false && TaskState == EGameplayTaskState::Active)
        {
            continue;
        }

        if (Config->ShowPaused == false && TaskState == EGameplayTaskState::Paused)
        {
            continue;
        }

        if (Config->ShowFinished == false && TaskState == EGameplayTaskState::Finished)
        {
            continue;
        }

        if (Config->ShowTicking == false && Task->IsTickingTask())
        {
            continue;
        }

        if (Config->ShowSimulating == false && Task->IsSimulating())
        {
            continue;
        }

        const auto& TaskName = StringCast<ANSICHAR>(*Task->GetName());
        bool PassFilter = Filter.PassFilter(TaskName.Get());

        if (PassFilter == false)
        {
            if (const UGameplayAbility* Ability = Cast<UGameplayAbility>(Task->GetTaskOwner()))
            {
                const auto& AbilityName = StringCast<ANSICHAR>(*FCogAbilityHelper::CleanupName(Ability->GetName()));
                PassFilter = Filter.PassFilter(AbilityName.Get());
            }
        }

        if (PassFilter == false)
        {
            continue;
        }

        FilteredTasks.Add(Task);
    }

    if (Config->SortByName)
    {
        FilteredTasks.Sort([](const UGameplayTask& Lhs, const UGameplayTask& Rhs)
        {
            return Lhs.GetName().Compare(Rhs.GetName()) < 0;
        });
    }
    
    if (ImGui::BeginTable("Tasks", 4, ImGuiTableFlags_SizingFixedFit
                                        | ImGuiTableFlags_Resizable
                                        | ImGuiTableFlags_NoBordersInBodyUntilResize
                                        | ImGuiTableFlags_ScrollY
                                        | ImGuiTableFlags_RowBg
                                        | ImGuiTableFlags_BordersV
                                        | ImGuiTableFlags_Reorderable
                                        | ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("State");
        ImGui::TableSetupColumn("Task");
        ImGui::TableSetupColumn("Owner");
        ImGui::TableSetupColumn("Is Ticking");
        ImGui::TableHeadersRow();

        static int SelectedIndex = -1;

        ImGuiListClipper Clipper;
        Clipper.Begin(FilteredTasks.Num());
        while (Clipper.Step())
        {
            for (int32 LineIndex = Clipper.DisplayStart; LineIndex < Clipper.DisplayEnd; LineIndex++)
            {
                const UGameplayTask* Task = FilteredTasks[LineIndex];

                ImGui::TableNextRow();
                ImGui::PushID(LineIndex);

                //------------------------
                // State
                //------------------------
                ImGui::TableNextColumn();
                RenderTaskState(Task);

                //------------------------
                // Name
                //------------------------
                ImGui::TableNextColumn();

                const auto& TaskName = StringCast<ANSICHAR>(*Task->GetName());
                if (ImGui::Selectable(TaskName.Get(), SelectedIndex == LineIndex, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick))
                {
                    SelectedIndex = LineIndex;
                }

                //------------------------
                // Popup
                //------------------------
                if (FCogWidgets::BeginItemTableTooltip())
                {
                    RenderTaskInfo(Task);
                    FCogWidgets::EndItemTableTooltip();
                }

                //------------------------
                // Owner
                //------------------------
                ImGui::TableNextColumn();
                RenderTaskOwner(Task);

                //------------------------
                // IsTicking
                //------------------------
                ImGui::TableNextColumn();
                ImGui::Text(Task->IsTickingTask() ? "Yes" : "No");

                ImGui::PopID();
            }
        }
        Clipper.End();

        ImGui::EndTable();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::RenderTaskInfo(const UGameplayTask* Task)
{
    if (Task == nullptr)
    {
        return;
    }

    if (ImGui::BeginTable("Task", 2, ImGuiTableFlags_Borders))
    {
        constexpr ImVec4 TextColor(1.0f, 1.0f, 1.0f, 0.5f);

        ImGui::TableSetupColumn("Property");
        ImGui::TableSetupColumn("Value");

        //------------------------
        // Name
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Name");
        ImGui::TableNextColumn();
        ImGui::Text("%s", StringCast<ANSICHAR>(*Task->GetName()).Get());

        //------------------------
        // Instance Name
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Instance Name");
        ImGui::TableNextColumn();
        ImGui::Text("%s", StringCast<ANSICHAR>(*Task->GetInstanceName().ToString()).Get());

        //------------------------
        // Owner
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Ability");
        ImGui::TableNextColumn();
        RenderTaskOwner(Task);

        //------------------------
        // State
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "State");
        ImGui::TableNextColumn();
        RenderTaskState(Task);

        //------------------------
        // Priority
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Priority");
        ImGui::TableNextColumn();
        ImGui::Text("%d", static_cast<int32>(Task->GetPriority()));

        //------------------------
        // IsTicking
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Is Ticking");
        ImGui::TableNextColumn();
        ImGui::Text(Task->IsTickingTask() ? "Yes" : "No");

        //------------------------
        // IsSimulated
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Is Simulated");
        ImGui::TableNextColumn();
        ImGui::Text(Task->IsSimulatedTask() ? "Yes" : "No");

        //------------------------
        // IsSimulating
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Is Simulating");
        ImGui::TableNextColumn();
        ImGui::Text(Task->IsSimulating() ? "Yes" : "No");

        //------------------------
        // Debug
        //------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Debug");
        ImGui::TableNextColumn();
        ImGui::PushTextWrapPos(FCogWidgets::GetFontWidth() * 80);
        ImGui::Text("%s", StringCast<ANSICHAR>(*Task->GetDebugString()).Get());
        ImGui::PopTextWrapPos();

        ImGui::EndTable();
    }
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::RenderTaskOwner(const UGameplayTask* Task)
{
    IGameplayTaskOwnerInterface* TaskOwner = Task->GetTaskOwner();

    FString OwnerName;
    if (const UGameplayAbility* Ability = Cast<UGameplayAbility>(TaskOwner))
    {
        OwnerName = FCogAbilityHelper::CleanupName(Ability->GetName());
    }
    else if (const UObject* Object = Cast<UObject>(TaskOwner))
    {
        OwnerName = GetNameSafe(Object);
    }

    ImGui::Text("%s", StringCast<ANSICHAR>(*OwnerName).Get());
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::RenderTaskState(const UGameplayTask* Task)
{
    switch (Task->GetState())
    {
    case EGameplayTaskState::Uninitialized:
        FCogWidgets::SmallButton("Uninitialized", FCogImguiHelper::ToImVec4(Config->UninitializedColor));
        break;

    case EGameplayTaskState::AwaitingActivation:
        FCogWidgets::SmallButton("Awaiting Activation", FCogImguiHelper::ToImVec4(Config->AwaitingActivationColor));
        break;

    case EGameplayTaskState::Active:
        FCogWidgets::SmallButton("Active", FCogImguiHelper::ToImVec4(Config->ActiveColor));
        break;

    case EGameplayTaskState::Paused:
        FCogWidgets::SmallButton("Paused", FCogImguiHelper::ToImVec4(Config->PausedColor));
        break;

    case EGameplayTaskState::Finished:
        FCogWidgets::SmallButton("Finished", FCogImguiHelper::ToImVec4(Config->FinishedColor));
        break;
    }
}
