#include "CogAbilityWindow_Tasks.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "CogAbilityDataAsset.h"
#include "CogAbilityHelper.h"
#include "CogAbilityReplicator.h"
#include "CogImguiHelper.h"
#include "CogWindowHelper.h"
#include "CogWindowWidgets.h"
#include "imgui.h"

class UCogAbilityConfig_Tasks;
//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::Initialize()
{
    Super::Initialize();

    bHasMenu = true;
    bNoPadding = true;

    Config = GetConfig<UCogAbilityConfig_Tasks>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::RenderHelp()
{
    ImGui::Text(
    "This window displays the gameplay tasks. ");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::ResetConfig()
{
    Super::ResetConfig();

    Config->Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::RenderTick(float DetlaTime)
{
    Super::RenderTick(DetlaTime);
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

            ImGui::ColorEdit4("Uninitialized Color", (float*)&Config->UninitializedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Awaiting Activation Color", (float*)&Config->AwaitingActivationColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Active Color", (float*)&Config->ActiveColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Paused Color", (float*)&Config->PausedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Finished Color", (float*)&Config->FinishedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);

            ImGui::Separator();

            if (ImGui::MenuItem("Reset"))
            {
                ResetConfig();
            }

            ImGui::EndMenu();
        }

        FCogWindowWidgets::SearchBar(Filter);

        ImGui::EndMenuBar();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::RenderTasksTable(UAbilitySystemComponent& AbilitySystemComponent)
{
    TArray<const UGameplayTask*> FilteredTasks;
    FilteredTasks.Reserve(16);

    const AActor* Selection = GetSelection();

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

        const char* TaskName = StringCast<ANSICHAR>(*Task->GetName()).Get();
        bool PassFilter = Filter.PassFilter(TaskName);

        if (PassFilter == false)
        {
            if (const UGameplayAbility* Ability = Cast<UGameplayAbility>(Task->GetTaskOwner()))
            {
                const char* AbilityName = StringCast<ANSICHAR>(*FCogAbilityHelper::CleanupName(Ability->GetName())).Get();
                PassFilter = Filter.PassFilter(AbilityName);
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

                const char* TaskName = StringCast<ANSICHAR>(*Task->GetName()).Get();
                if (ImGui::Selectable(TaskName, SelectedIndex == LineIndex, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick))
                {
                    SelectedIndex = LineIndex;
                }

                //------------------------
                // Popup
                //------------------------
                if (ImGui::IsItemHovered())
                {
                    FCogWindowWidgets::BeginTableTooltip();
                    RenderTaskInfo(Task);
                    FCogWindowWidgets::EndTableTooltip();
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
        ImGui::Text(StringCast<ANSICHAR>(*Task->GetName()).Get());

        //------------------------
		// Instance Name
		//------------------------
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(TextColor, "Instance Name");
        ImGui::TableNextColumn();
        ImGui::Text(StringCast<ANSICHAR>(*Task->GetInstanceName().ToString()).Get());

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
        ImGui::Text("%d", (int32)Task->GetPriority());

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
        ImGui::PushTextWrapPos(FCogWindowWidgets::GetFontWidth() * 80);
        ImGui::Text(StringCast<ANSICHAR>(*Task->GetDebugString()).Get());
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

    ImGui::Text(StringCast<ANSICHAR>(*OwnerName).Get());
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAbilityWindow_Tasks::RenderTaskState(const UGameplayTask* Task)
{
    switch (Task->GetState())
    {
    case EGameplayTaskState::Uninitialized:
        FCogWindowWidgets::SmallButton("Uninitialized", FCogImguiHelper::ToImVec4(Config->UninitializedColor));
        break;

    case EGameplayTaskState::AwaitingActivation:
        FCogWindowWidgets::SmallButton("Awaiting Activation", FCogImguiHelper::ToImVec4(Config->AwaitingActivationColor));
        break;

    case EGameplayTaskState::Active:
        FCogWindowWidgets::SmallButton("Active", FCogImguiHelper::ToImVec4(Config->ActiveColor));
        break;

    case EGameplayTaskState::Paused:
        FCogWindowWidgets::SmallButton("Paused", FCogImguiHelper::ToImVec4(Config->PausedColor));
        break;

    case EGameplayTaskState::Finished:
        FCogWindowWidgets::SmallButton("Finished", FCogImguiHelper::ToImVec4(Config->FinishedColor));
        break;
    }
}
