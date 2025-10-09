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

void FCogAIWindow_Perception::Initialize() {
  Super::Initialize();

  bHasMenu = true;

  Config = GetConfig<UCogAIConfig_Perception>();
  if (!Config) {
    UE_LOG(LogCogAI, Warning, TEXT("FCogAIWindow_Perception: Config not found, using defaults."));
  }
}

void FCogAIWindow_Perception::RenderHelp() {
  ImGui::TextUnformatted(
    "Shows UAIPerceptionComponent data for the selected Pawn/AIController:\n"
    "Known perceived actors\n"
    "- Stimuli details (age, strength, locations, tags)\n"
    "Optionally draws stimulus locations in-world."
    );
}

void FCogAIWindow_Perception::GameTick(float DeltaTime) {
  Super::GameTick(DeltaTime);

#if ENABLE_COG
  AActor *Selection = GetSelection();
  if (Selection == nullptr) {
    return;
  }

  UAIPerceptionComponent *Perception = ResolvePerception(Selection);
  if (Perception == nullptr) {
    return;
  }

  if (!bDrawStimulusDebug) {
    return;
  }

  TArray<AActor *> KnownActors;
  Perception->GetKnownPerceivedActors(nullptr, KnownActors);

  for (AActor *Actor : KnownActors) {
    if (!IsValid(Actor)) {
      continue;
    }

    FActorPerceptionBlueprintInfo Info;
    if (!Perception->GetActorsPerception(Actor, Info)) {
      continue;
    }

    for (const FAIStimulus &Stimulus : Info.LastSensedStimuli) {
      if (Stimulus.IsExpired() || !Stimulus.IsValid()) {
        continue;
      }

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
#endif
}

void FCogAIWindow_Perception::RenderContent() {
  Super::RenderContent();

  if (Config == nullptr) {
    ImGui::TextDisabled("Invalid Config");
    return;
  }

  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("Options")) {
      ImGui::ColorEdit4("Active Color", &Config->ActiveColor.R,
                        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
      ImGui::ColorEdit4("Inactive Color", &Config->InactiveColor.R,
                        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
      ImGui::Separator();
      ImGui::Checkbox("Draw Stimulus Debug", &bDrawStimulusDebug);
      ImGui::SetNextItemWidth(150.f);
      ImGui::Checkbox("Persist Debug Draw", &bDrawStimulusDebugPersist);
      ImGui::EndMenu();
    }

    FCogWidgets::SearchBar("##Filter", Filter);

    ImGui::EndMenuBar();
  }

  AActor *Selection = GetSelection();
  if (Selection == nullptr) {
    ImGui::TextDisabled("Invalid Selection");
    return;
  }

  UAIPerceptionComponent *Perception = ResolvePerception(Selection);
  if (Perception == nullptr) {
    ImGui::TextDisabled("No UAIPerceptionComponent on selection (or its AIController).");
    return;
  }

  ImGui::Text("Selection: %s", TCHAR_TO_UTF8(*GetNameSafe(Selection)));
  ImGui::Spacing();

  TArray<AActor *> KnownActors;
  Perception->GetKnownPerceivedActors(nullptr, KnownActors);

  if (KnownActors.Num() == 0) {
    ImGui::TextDisabled("No known perceived actors.");
    return;
  }

  for (AActor *Actor : KnownActors) {
    if (!IsValid(Actor)) {
      continue;
    }

    const FString ActorName = GetNameSafe(Actor);
    if (Filter.IsActive() && !Filter.PassFilter(TCHAR_TO_UTF8(*ActorName))) {
      continue;
    }

    ImGui::PushID(Actor);

    FActorPerceptionBlueprintInfo Info;
    const bool bHaveInfo = Perception->GetActorsPerception(Actor, Info);
    bool bAnyActive = false;
    if (bHaveInfo) {
      for (const FAIStimulus &S : Info.LastSensedStimuli) {
        if (!S.IsExpired() && S.WasSuccessfullySensed()) {
          bAnyActive = true;
          break;
        }
      }
    }

    const ImVec4 NameColor = bAnyActive
                               ? FCogImguiHelper::ToImVec4(Config->ActiveColor)
                               : FCogImguiHelper::ToImVec4(Config->InactiveColor);
    ImGui::PushStyleColor(ImGuiCol_Text, NameColor);
    const bool Open = ImGui::CollapsingHeader(TCHAR_TO_UTF8(*ActorName), ImGuiTreeNodeFlags_DefaultOpen);
    ImGui::PopStyleColor();

    ImGui::SameLine();
    FCogWidgets::PushStyleCompact();
    if (!bAnyActive)
      ImGui::BeginDisabled();
    bool DrawActive = bAnyActive;
    ImGui::Checkbox("##Active", &DrawActive);
    if (!bAnyActive)
      ImGui::EndDisabled();
    FCogWidgets::PopStyleCompact();

    if (Open && bHaveInfo) {
      RenderStimuliTable(Info);
    }

    ImGui::PopID();
  }
}

TObjectPtr<UAIPerceptionComponent> FCogAIWindow_Perception::ResolvePerception(const TObjectPtr<AActor> Selection) {
  if (!IsValid(Selection)) {
    return nullptr;
  }

  if (UAIPerceptionComponent *Direct = Selection->FindComponentByClass<UAIPerceptionComponent>()) {
    return Direct;
  }

  if (AAIController *AsController = Cast<AAIController>(Selection)) {
    return AsController->GetPerceptionComponent();
  }

  const auto Pawn = Cast<APawn>(Selection);
  if (!IsValid(Pawn)) {
    return nullptr;
  }

  const auto Controller = Cast<AAIController>(Pawn->GetController());
  if (!IsValid(Controller)) {
    return nullptr;
  }

  return Controller->GetPerceptionComponent();
}

const char *FCogAIWindow_Perception::SenseNameFromID(const FAISenseID &Id) const {
  if (!Id.IsValid())
    return "None";
  if (Id == UAISense_Sight::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    return "Sight";
  if (Id == UAISense_Hearing::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    return "Hearing";
  if (Id == UAISense_Damage::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    return "Damage";
  if (Id == UAISense_Touch::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    return "Touch";
  if (Id == UAISense_Team::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    return "Team";
  if (Id == UAISense_Prediction::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    return "Prediction";
  return "Other";
}

ImVec4 FCogAIWindow_Perception::ColorFromSenseID(const FAISenseID &Id) const {
  if (!Id.IsValid())
    return ImVec4(0.6f, 0.6f, 0.6f, 1.f);

  if (Id == UAISense_Sight::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    return ImVec4(0.90f, 0.75f, 0.10f, 1.f);
  if (Id == UAISense_Hearing::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    return ImVec4(0.20f, 0.60f, 1.00f, 1.f);
  if (Id == UAISense_Damage::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    return ImVec4(0.95f, 0.20f, 0.20f, 1.f);
  if (Id == UAISense_Touch::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    return ImVec4(0.20f, 0.90f, 0.45f, 1.f);
  if (Id == UAISense_Team::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    return ImVec4(0.75f, 0.20f, 0.90f, 1.f);
  if (Id == UAISense_Prediction::StaticClass()->GetDefaultObject<UAISense>()->GetSenseID())
    return ImVec4(0.95f, 0.50f, 0.20f, 1.f);

  return ImVec4(0.7f, 0.7f, 0.7f, 1.f);
}

void FCogAIWindow_Perception::RenderStimuliTable(const FActorPerceptionBlueprintInfo &Info) {
  if (ImGui::BeginTable("Stimuli", 6, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
    ImGui::TableSetupColumn("Sense");
    ImGui::TableSetupColumn("Active");
    ImGui::TableSetupColumn("Age");
    ImGui::TableSetupColumn("Strength");
    ImGui::TableSetupColumn("Stimulus Loc");
    ImGui::TableSetupColumn("Tag");
    ImGui::TableHeadersRow();

    for (const FAIStimulus &Stimulus : Info.LastSensedStimuli) {
      if (!Stimulus.IsValid()) {
        continue;
      }

      ImGui::PushID(Stimulus.Type.Index);

      ImGui::TableNextRow();

      ImGui::TableNextColumn();
      const ImVec4 SenseColor = ColorFromSenseID(Stimulus.Type);
      ImGui::TextColored(SenseColor, "%s", SenseNameFromID(Stimulus.Type));

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

      ImGui::TableNextColumn();
      ImGui::Text("%.2f", Stimulus.GetAge());

      ImGui::TableNextColumn();
      ImGui::Text("%.2f", Stimulus.Strength);

      ImGui::TableNextColumn();
      {
        const FVector L = Stimulus.StimulusLocation;
        ImGui::Text("X %.1f Y %.1f Z %.1f", L.X, L.Y, L.Z);
      }

      ImGui::TableNextColumn();
      ImGui::Text("%s", TCHAR_TO_UTF8(*Stimulus.Tag.ToString()));

      ImGui::PopID();
    }

    ImGui::EndTable();
  }

  if (FCogWidgets::BeginItemTableTooltip()) {
    if (ImGui::BeginTable("Extra", 2, ImGuiTableFlags_Borders)) {
      ImGui::TableSetupColumn("Property");
      ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::TextDisabled("Actor");
      ImGui::TableNextColumn();
      ImGui::Text("%s", TCHAR_TO_UTF8(*GetNameSafe(Info.Target)));

      ImGui::EndTable();
    }
    FCogWidgets::EndItemTableTooltip();
  }
}
