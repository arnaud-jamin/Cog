#include "CogAIWindow_Perception.h"

#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Touch.h"
#include "Perception/AISense_Team.h"
#include "Perception/AISense_Prediction.h"

#include "CogAIModule.h"
#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "CogCommon.h"
#include "CogDebugDraw.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_Perception::Initialize() {
    Super::Initialize();

    bHasMenu = true;

    Config = GetConfig<UCogAIConfig_Perception>();
    if (!Config) 
    {
        UE_LOG(LogCogAI, Warning, TEXT("FCogAIWindow_Perception: Config not found, using defaults."));
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_Perception::RenderHelp() {
    ImGui::TextUnformatted(
        "Shows UAIPerceptionComponent data for the selected Pawn/AIController:\n"
        "Known perceived actors\n"
        "- Stimuli details (age, strength, locations, tags)\n"
        "Optionally draws stimulus locations in-world."
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_Perception::GameTick(float DeltaTime)
{
    Super::GameTick(DeltaTime);

#if ENABLE_COG
    AActor* Selection = GetSelection();
    if (Selection == nullptr) 
    { return; }

    UAIPerceptionComponent* Perception = ResolvePerception(Selection);
    if (Perception == nullptr) 
    { return; }

    if (!bDrawStimulusDebug) 
    { return; }

    TArray<AActor*> KnownActors;
    Perception->GetKnownPerceivedActors(nullptr, KnownActors);

    for (AActor* Actor : KnownActors) 
    {
        if (!IsValid(Actor)) 
        { continue; }

        FActorPerceptionBlueprintInfo Info;
        if (!Perception->GetActorsPerception(Actor, Info)) 
        { continue; }

        for (const FAIStimulus& Stimulus : Info.LastSensedStimuli) 
        {
            if (Stimulus.IsExpired() || !Stimulus.IsValid()) 
            { continue; }

            const FVector From = Stimulus.ReceiverLocation;
            const FVector To = Stimulus.StimulusLocation;

            const FAISenseID SenseID = Stimulus.Type;
            const auto Color = ColorFromSenseID(SenseID);
            const FLinearColor LinCol(Color.x, Color.y, Color.z, Color.w);
            const FColor LineColor = LinCol.ToFColor(true);

            FCogDebugDraw::Arrow(LogCogAI, Actor, From, To, LineColor, bDrawStimulusDebugPersist, 0);
            FCogDebugDraw::Point(LogCogAI, Actor, From, 8.0f, LineColor, bDrawStimulusDebugPersist, 0);
            FCogDebugDraw::Point(LogCogAI, Actor, To, 10.0f, LineColor, bDrawStimulusDebugPersist, 0);
        }
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_Perception::PreBegin(ImGuiWindowFlags& WindowFlags)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_Perception::PostBegin()
{
    ImGui::PopStyleVar();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_Perception::RenderContent()
{
    Super::RenderContent();

    if (Config == nullptr) 
    {
        ImGui::TextDisabled("Invalid Config");
        return;
    }

    if (ImGui::BeginMenuBar()) 
    {
        if (ImGui::BeginMenu("Options")) 
        {
            ImGui::ColorEdit4("Active Color", &Config->ActiveColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Inactive Color", &Config->InactiveColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::Separator();
            ImGui::Checkbox("Draw Stimulus Debug", &bDrawStimulusDebug);
            ImGui::SetNextItemWidth(150.f);
            ImGui::Checkbox("Persist Debug Draw", &bDrawStimulusDebugPersist);
            ImGui::Separator();
            ImGui::ColorEdit4("Sight Color", &Config->SightSenseColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Hearing Color", &Config->HearingSenseColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Damage Color", &Config->DamageSenseColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Touch Color", &Config->TouchSenseColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Team Color", &Config->TeamSenseColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Prediction Color", &Config->PredictionSenseColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Default Color", &Config->DefaultSenseColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::EndMenu();
        }

        FCogWidgets::SearchBar("##Filter", Filter);

        ImGui::EndMenuBar();
    }

    AActor* Selection = GetSelection();
    if (Selection == nullptr) 
    {
        ImGui::Indent();
        ImGui::Spacing();
        ImGui::TextDisabled("Invalid Selection");
        ImGui::Unindent();
        return;
    }

    UAIPerceptionComponent* Perception = ResolvePerception(Selection);
    if (Perception == nullptr) 
    {
        ImGui::Indent();
        ImGui::Spacing();
        ImGui::TextDisabled("No UAIPerceptionComponent on selection (or its AIController).");
        ImGui::Unindent();
        return;
    }

    TArray<AActor*> KnownActors;
    Perception->GetKnownPerceivedActors(nullptr, KnownActors);

    if (KnownActors.Num() == 0) 
    {
        ImGui::Indent();
        ImGui::Spacing();
        ImGui::TextDisabled("No known perceived actors.");
        ImGui::Unindent();
        return;
    }

    if (ImGui::BeginTable("Stimuli", 7, ImGuiTableFlags_SizingFixedFit
                                        | ImGuiTableFlags_Resizable
                                        | ImGuiTableFlags_NoBordersInBodyUntilResize
                                        | ImGuiTableFlags_ScrollY
                                        | ImGuiTableFlags_RowBg
                                        | ImGuiTableFlags_BordersV
                                        | ImGuiTableFlags_Reorderable
                                        | ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupColumn("--", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 2);
        ImGui::TableSetupColumn("Sense");
        ImGui::TableSetupColumn("Active");
        ImGui::TableSetupColumn("Age");
        ImGui::TableSetupColumn("Strength");
        ImGui::TableSetupColumn("Location");
        ImGui::TableSetupColumn("Tag");
        ImGui::TableHeadersRow();

        for (AActor* Actor : KnownActors)
        {
            if (!IsValid(Actor))
            {
                continue;
            }

            const FString ActorName = FCogHelper::GetActorName(Actor);
            auto ActorNameStr = StringCast<ANSICHAR>(*ActorName);

            if (Filter.IsActive() && !Filter.PassFilter(ActorNameStr.Get()))
            {
                continue;
            }

            ImGui::PushID(Actor);

            FActorPerceptionBlueprintInfo Info;
            const bool bHaveInfo = Perception->GetActorsPerception(Actor, Info);
            bool bAnyActive = false;
            if (bHaveInfo)
            {
                for (const FAIStimulus& Stimulus : Info.LastSensedStimuli)
                {
                    if (!Stimulus.IsExpired() && Stimulus.WasSuccessfullySensed())
                    {
                        bAnyActive = true;
                        break;
                    }
                }
            }

            //------------------------
            // Actor Tree Node
            //------------------------
            const ImVec4 NameColor = bAnyActive
                ? FCogImguiHelper::ToImVec4(Config->ActiveColor)
                : FCogImguiHelper::ToImVec4(Config->InactiveColor);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Text, NameColor);
            bool bOpenActor = ImGui::TreeNodeEx(ActorNameStr.Get(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_LabelSpanAllColumns);
            ImGui::PopStyleColor();


            //------------------------
            // Actor Stimulus
            //------------------------

            if (bOpenActor && bHaveInfo)
            {
                for (const FAIStimulus& Stimulus : Info.LastSensedStimuli)
                {
                    if (!Stimulus.IsValid())
                    { continue; }

                    ImGui::PushID(Stimulus.Type.Index);

                    ImGui::TableNextRow();

                    //------------------------
                    // --
                    //------------------------
                    ImGui::TableNextColumn();

                    //------------------------
                    // Type
                    //------------------------
                    ImGui::TableNextColumn();
                    const ImVec4 SenseColor = ColorFromSenseID(Stimulus.Type);
                    ImGui::PushStyleColor(ImGuiCol_Text, SenseColor);
                    ImGui::Selectable(SenseNameFromID(Stimulus.Type), false, ImGuiSelectableFlags_SpanAllColumns);
                    ImGui::PopStyleColor();

                    if (FCogWidgets::BeginItemTableTooltip())
                    {
                        DrawSenseTooltip(Info, Stimulus);
                        FCogWidgets::EndItemTableTooltip();
                    }

                    //------------------------
                    // Active
                    //------------------------
                    const bool bActive = (!Stimulus.IsExpired() && Stimulus.WasSuccessfullySensed());

                    ImGui::TableNextColumn();
                    {
                        FCogWidgets::PushStyleCompact();
                        ImGui::BeginDisabled();
                        bool Flag = bActive;
                        ImGui::Checkbox("##Active", &Flag);
                        ImGui::EndDisabled();
                        FCogWidgets::PopStyleCompact();
                    }

                    //------------------------
                    // Age
                    //------------------------
                    ImGui::TableNextColumn();
                    ImGui::Text("%.2f", Stimulus.GetAge());

                    //------------------------
                    // Strength
                    //------------------------
                    ImGui::TableNextColumn();
                    ImGui::Text("%.2f", Stimulus.Strength);

                    //------------------------
                    // Location
                    //-----------------------
                    ImGui::TableNextColumn();
                    {
                        const FVector Location = Stimulus.StimulusLocation;
                        ImGui::Text("(%.1f, %.1f, %.1f)", Location.X, Location.Y, Location.Z);
                    }

                    //------------------------
                    // Tag
                    //-----------------------
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", TCHAR_TO_UTF8(*Stimulus.Tag.ToString()));

                    ImGui::PopID();
                }
            }

            if (bOpenActor)
            {
                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    
        ImGui::EndTable();
    }
}

void FCogAIWindow_Perception::DrawSenseTooltip(const FActorPerceptionBlueprintInfo& Info, const FAIStimulus& Stimulus)
{
    if (ImGui::BeginTable("Extra", 2, ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupColumn("Property");
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextDisabled("Actor");
        ImGui::TableNextColumn();
        ImGui::Text("%s", COG_TCHAR_TO_CHAR(*FCogHelper::GetActorName(Info.Target)));

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextDisabled("Sense");
        ImGui::TableNextColumn();
        const ImVec4 SenseColor = ColorFromSenseID(Stimulus.Type);
        ImGui::PushStyleColor(ImGuiCol_Text, SenseColor);
        ImGui::Text("%s", SenseNameFromID(Stimulus.Type));
        ImGui::PopStyleColor();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextDisabled("Active");
        ImGui::TableNextColumn();
        bool bActive = (!Stimulus.IsExpired() && Stimulus.WasSuccessfullySensed());
        ImGui::Checkbox("##Active", &bActive);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextDisabled("Age");
        ImGui::TableNextColumn();
        ImGui::Text("%.2f", Stimulus.GetAge());

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextDisabled("Strength");
        ImGui::TableNextColumn();
        ImGui::Text("%.2f", Stimulus.Strength);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextDisabled("Location");
        ImGui::TableNextColumn();
        const FVector Location = Stimulus.StimulusLocation;
        ImGui::Text("(%.1f, %.1f, %.1f)", Location.X, Location.Y, Location.Z);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextDisabled("Tag");
        ImGui::TableNextColumn();
        ImGui::Text("%s", TCHAR_TO_UTF8(*Stimulus.Tag.ToString()));

        ImGui::EndTable();
    }
}

TObjectPtr<UAIPerceptionComponent> FCogAIWindow_Perception::ResolvePerception(const TObjectPtr<AActor> Selection) 
{
    if (!IsValid(Selection)) 
    { return nullptr; }

    if (UAIPerceptionComponent* Direct = Selection->FindComponentByClass<UAIPerceptionComponent>()) 
    { return Direct; }

    if (AAIController* AsController = Cast<AAIController>(Selection)) 
    { return AsController->GetPerceptionComponent(); }

    const auto Pawn = Cast<APawn>(Selection);
    if (!IsValid(Pawn)) 
    { return nullptr; }

    const auto Controller = Cast<AAIController>(Pawn->GetController());
    if (!IsValid(Controller)) 
    { return nullptr; }

    return Controller->GetPerceptionComponent();
}

const char* FCogAIWindow_Perception::SenseNameFromID(const FAISenseID& Id) const 
{
    if (!Id.IsValid())
    { return "None"; }

    if (Id == UAISense_Sight::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    { return "Sight"; }

    if (Id == UAISense_Hearing::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    { return "Hearing"; }

    if (Id == UAISense_Damage::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    { return "Damage"; }

    if (Id == UAISense_Touch::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    { return "Touch"; }

    if (Id == UAISense_Team::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    { return "Team"; }

    if (Id == UAISense_Prediction::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    { return "Prediction"; }

    return "Other";
}

ImVec4 FCogAIWindow_Perception::ColorFromSenseID(const FAISenseID& Id) const 
{
    if (!Id.IsValid())
    { return FCogImguiHelper::ToImVec4(Config->DefaultSenseColor); }

    if (Id == UAISense_Sight::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    { return FCogImguiHelper::ToImVec4(Config->SightSenseColor); }

    if (Id == UAISense_Hearing::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    { return FCogImguiHelper::ToImVec4(Config->HearingSenseColor); }

    if (Id == UAISense_Damage::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    { return FCogImguiHelper::ToImVec4(Config->DamageSenseColor); }

    if (Id == UAISense_Touch::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    { return FCogImguiHelper::ToImVec4(Config->TouchSenseColor); }

    if (Id == UAISense_Team::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    { return FCogImguiHelper::ToImVec4(Config->TeamSenseColor); }

    if (Id == UAISense_Prediction::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    { return FCogImguiHelper::ToImVec4(Config->PredictionSenseColor); }

    return FCogImguiHelper::ToImVec4(Config->DefaultSenseColor);
}
