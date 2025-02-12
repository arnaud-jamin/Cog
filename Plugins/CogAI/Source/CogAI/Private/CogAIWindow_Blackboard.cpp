#include "CogAIWindow_Blackboard.h"

#include "AIController.h"
#include "CogWidgets.h"
#include "BrainComponent.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_Blackboard::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    Config = GetConfig<UCogAIConfig_Blackboard>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_Blackboard::RenderHelp()
{
    ImGui::Text(
        "This window displays the blackboard of the selected actor. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_Blackboard::PreBegin(ImGuiWindowFlags& WindowFlags)
{
    WindowFlags |= ImGuiWindowFlags_NoScrollbar;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_Blackboard::PostBegin()
{
    ImGui::PopStyleVar();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_Blackboard::RenderContent()
{
    Super::RenderContent();

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Sort by Name", &Config->bSortByName);
            ImGui::EndMenu();
        }

        FCogWidgets::SearchBar("##Filter", Filter);

        ImGui::EndMenuBar();
    }

    AActor* Selection = GetSelection();
    if (Selection == nullptr)
    {
        ImGui::TextDisabled("Invalid Selection");
        return;
    }

    APawn* Pawn = Cast<APawn>(Selection);
    if (Pawn == nullptr)
    {
        ImGui::TextDisabled("Selection is not a pawn");
        return;
    }

    AAIController* AIController = Cast<AAIController>(Pawn->Controller);
    if (AIController == nullptr)
    {
        ImGui::TextDisabled("Selection has no AIController");
        return;
    }

    UBrainComponent* BehaviorTree = AIController->GetBrainComponent();
    if (BehaviorTree == nullptr)
    {
        ImGui::TextDisabled("Selection has no BrainComponent");
        return;
    }

    UBlackboardComponent* Blackboard = BehaviorTree->GetBlackboardComponent();
    if (Blackboard == nullptr)
    {
        ImGui::TextDisabled("Selection has no BlackboardComponent");
        return;
    }

    UBlackboardData* BlackboardAsset = Blackboard->GetBlackboardAsset();
    if (BlackboardAsset == nullptr)
    {
        ImGui::TextDisabled("Selection has no BlackboardAsset");
        return;
    }

    TArray<const FBlackboardEntry*> Keys;
    uint8 Offset = 0;
    for (UBlackboardData* It = BlackboardAsset; It; It = It->Parent)
    {
        for (int32 KeyID = 0; KeyID < It->Keys.Num(); KeyID++)
        {
            if (const FBlackboardEntry* Key = BlackboardAsset->GetKey(KeyID))
            {
                Keys.Add(Key);
            }
        }
        Offset += It->Keys.Num();
    }
    
    if (Config->bSortByName)
    {
        Keys.Sort([](const FBlackboardEntry& Key1, const FBlackboardEntry& Key2)
        {
            return Key1.EntryName.Compare(Key2.EntryName) < 0;
        });
    }

    if (ImGui::BeginTable("Blackboard", 3, ImGuiTableFlags_SizingFixedFit 
                                         | ImGuiTableFlags_Resizable 
                                         | ImGuiTableFlags_NoBordersInBodyUntilResize 
                                         | ImGuiTableFlags_ScrollY 
                                         | ImGuiTableFlags_RowBg 
                                         | ImGuiTableFlags_BordersV 
                                         | ImGuiTableFlags_Reorderable 
                                         | ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupScrollFreeze(0, 1);

        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Key");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        const FString CommonTypePrefix = UBlackboardKeyType::StaticClass()->GetName().AppendChar(TEXT('_'));

        for (int32 KeyID = 0; KeyID < Keys.Num(); ++KeyID)
        {
            const FBlackboardEntry* Key = Keys[KeyID];
            if (Key == nullptr)
            {
                continue;
            }
                
            const auto KeyName = StringCast<ANSICHAR>(*Key->EntryName.ToString());
            if (Filter.PassFilter(KeyName.Get()) == false)
            {
                continue;
            }

            ImGui::TableNextRow();

            //------------------------
            // Type
            //------------------------
            ImGui::TableNextColumn();
            const FString FullKeyType = Key->KeyType ? GetNameSafe(Key->KeyType->GetClass()) : FString();
            const FString DescKeyType = FullKeyType.StartsWith(CommonTypePrefix) ? FullKeyType.RightChop(CommonTypePrefix.Len()) : FullKeyType;
            ImGui::Text("%s", TCHAR_TO_ANSI(*DescKeyType));

            //------------------------
            // Name
            //------------------------
            ImGui::TableNextColumn();
            ImGui::Text("%s", KeyName.Get());

            //------------------------
            // Value
            //------------------------
            ImGui::TableNextColumn();
            const uint8* ValueData = Blackboard->GetKeyRawData(KeyID);
            FString ValueDesc = Key->KeyType && ValueData ? *(Key->KeyType->WrappedDescribeValue(*Blackboard, ValueData)) : TEXT("Empty");
            ImGui::Text("%s", TCHAR_TO_ANSI(*ValueDesc));
        }

        ImGui::EndTable();
    }
}

