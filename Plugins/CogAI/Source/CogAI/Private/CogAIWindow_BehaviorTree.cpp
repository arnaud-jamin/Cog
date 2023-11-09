#include "CogAIWindow_BehaviorTree.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTNode.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BrainComponent.h"
#include "CogAIModule.h"
#include "CogDebugDraw.h"
#include "CogImguiHelper.h"
#include "CogWindowWidgets.h"
#include "GameFramework/Pawn.h"
#include <Navigation/PathFollowingComponent.h>
#include "imgui_internal.h"


//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_BehaviorTree::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    Config = GetConfig<UCogAIConfig_BehaviorTree>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_BehaviorTree::RenderHelp()
{
    ImGui::Text(
        "This window displays the behavior tree of the selected actor. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_BehaviorTree::GameTick(float DeltaTime)
{
    Super::GameTick(DeltaTime);

#if ENABLE_COG

    const AActor* Selection = GetSelection();
    if (Selection == nullptr)
    {
        return;
    }

    const APawn* Pawn = Cast<APawn>(Selection);
    if (Pawn == nullptr)
    {
        return;
    }

    const AAIController* AIController = Cast<AAIController>(Pawn->Controller);
    if (AIController == nullptr)
    {
        return;
    }

    const UPathFollowingComponent* PathFollowing = AIController->GetPathFollowingComponent();
    if (PathFollowing == nullptr)
    {
        return;
    }

    const FNavigationPath* CurrentPath = PathFollowing->GetPath().Get();
    if (CurrentPath == nullptr)
    {
        return;
    }

    FVector LastLocation;
    for (int32 i = 0; i < CurrentPath->GetPathPoints().Num(); i++)
    {
        const FVector& Location = CurrentPath->GetPathPoints()[i].Location;

        if (i > 0)
        {
            FCogDebugDraw::Arrow(LogCogAI, Pawn, LastLocation, Location, FColor::White, false, 0);
            FCogDebugDraw::Point(LogCogAI, Pawn, LastLocation, 10.0f, FColor::White, false, 0);
            FCogDebugDraw::Point(LogCogAI, Pawn, Location, 10.0f, FColor::White, false, 0);
        }

        LastLocation = Location;
    }
    
    //TArray<FString> Tokens;
    //TArray<EPathFollowingDebugTokens::Type> Flags;
    //PathFollowing->GetDebugStringTokens(Tokens, Flags);

    //for (int32 Idx = 0; Idx < Tokens.Num(); Idx++)
    //{
    //    switch (Flags[Idx])
    //    {
    //    case EPathFollowingDebugTokens::Description:
    //        DataPack.PathFollowingInfo += Tokens[Idx];
    //        break;

    //    case EPathFollowingDebugTokens::ParamName:
    //        DataPack.PathFollowingInfo += TEXT(", {yellow}");
    //        DataPack.PathFollowingInfo += Tokens[Idx];
    //        DataPack.PathFollowingInfo += TEXT(":");
    //        break;

    //    case EPathFollowingDebugTokens::PassedValue:
    //        DataPack.PathFollowingInfo += TEXT("{yellow}");
    //        DataPack.PathFollowingInfo += Tokens[Idx];
    //        break;

    //    case EPathFollowingDebugTokens::FailedValue:
    //        DataPack.PathFollowingInfo += TEXT("{orange}");
    //        DataPack.PathFollowingInfo += Tokens[Idx];
    //        break;

    //    default:
    //        break;
    //    }
    //}

#endif //ENABLE_COG

}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_BehaviorTree::RenderContent()
{
    Super::RenderContent();

    if (Config == nullptr)
    {
        ImGui::TextDisabled("No Config");
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::ColorEdit4("Active Color", (float*)&Config->ActiveColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Inactive Color", (float*)&Config->InactiveColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::EndMenu();
        }

        FCogWindowWidgets::MenuSearchBar(Filter);

        ImGui::EndMenuBar();
    }

    AActor* Selection = GetSelection();
    if (Selection == nullptr)
    {
        ImGui::TextDisabled("No Selection");
        return;
    }

    APawn* Pawn = Cast<APawn>(Selection);
    if (Pawn == nullptr)
    {
        ImGui::TextDisabled("Not a pawn");
        return;
    }

    AAIController* AIController = Cast<AAIController>(Pawn->Controller);
    if (AIController == nullptr)
    {
        ImGui::TextDisabled("No AIController");
        return;
    }

    UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(AIController->GetBrainComponent());
    if (BehaviorTreeComponent == nullptr)
    {
        ImGui::TextDisabled("No BrainComponent");
        return;
    }

    UBehaviorTree* CurrentTree = BehaviorTreeComponent->GetCurrentTree();
    if (CurrentTree == nullptr)
    {
        ImGui::TextDisabled("No Current Tree");
        return;
    }


    UBehaviorTree* RootTree = BehaviorTreeComponent->GetRootTree();
    if (CurrentTree != RootTree)
    {
        if (ImGui::CollapsingHeader(TCHAR_TO_ANSI(*GetNameSafe(RootTree)), nullptr, ImGuiTreeNodeFlags_DefaultOpen))
        {
            RenderNode(*BehaviorTreeComponent, RootTree->RootNode, false);

            ImGui::Spacing();
            ImGui::Spacing();
        }
    }

    //----------------------------------------------------------------------------------------
    // If we use the current tree root node it doesn't seem to be the one instanced.
    // Not sure if there is a better way to access it, but we find the root node from 
    // the current active node. Then we will be able to check if the drawn nodes is 
    // parent of the active node to display it active. 
    //----------------------------------------------------------------------------------------
    const UBTNode* RootNodeInstanced = nullptr;
    for (const UBTNode* ParentNode = BehaviorTreeComponent->GetActiveNode(); ParentNode != nullptr; ParentNode = ParentNode->GetParentNode())
    {
        RootNodeInstanced = ParentNode;
    }

    if (RootNodeInstanced != nullptr)
    {
        if (ImGui::CollapsingHeader(TCHAR_TO_ANSI(*GetNameSafe(CurrentTree)), nullptr, ImGuiTreeNodeFlags_DefaultOpen))
        {
            RenderNode(*BehaviorTreeComponent, const_cast<UBTNode*>(RootNodeInstanced), false);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogAIWindow_BehaviorTree::RenderNode(UBehaviorTreeComponent& BehaviorTreeComponent, UBTNode* Node, bool OpenAllChildren)
{
    const char* NodeName = TCHAR_TO_ANSI(*Node->GetNodeName());
    const bool ShowNode = Filter.PassFilter(NodeName);

    const UBTCompositeNode* CompositeNode = Cast<UBTCompositeNode>(Node);

    const bool IsActive = BehaviorTreeComponent.IsExecutingBranch(Node);

    bool OpenChildren = false;

    if (ShowNode)
    {
        ImGui::PushID(Node);

        if (OpenAllChildren)
        {
            ImGui::SetNextItemOpen(true, ImGuiCond_Always);
        }
        else
        {
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        }

        //------------------------
        // TreeNode
        //------------------------
        const bool HasChildren = CompositeNode != nullptr && CompositeNode->GetChildrenNum() > 0;
        if (HasChildren && Filter.IsActive() == false)
        {
            OpenChildren = ImGui::TreeNodeEx("##Node", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanFullWidth);
        }
        else
        {
            ImGui::TreeNodeEx("##Node", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanFullWidth);
        }

        const bool IsControlDown = ImGui::GetCurrentContext()->IO.KeyCtrl;
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && IsControlDown)
        {
            OpenAllChildren = true;
        }

        //------------------------
        // ContextMenu
        //------------------------
        if (ImGui::BeginPopupContextItem())
        {
            ImGui::EndPopup();
        }

        //------------------------
        // Tooltip
        //------------------------
        if (ImGui::IsItemHovered())
        {
            FCogWindowWidgets::BeginTableTooltip();

            if (ImGui::BeginTable("Effect", 2, ImGuiTableFlags_Borders))
            {
                ImGui::TableSetupColumn("Property");
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                const ImVec4 TextColor(1.0f, 1.0f, 1.0f, 0.5f);

                //------------------------
                // Name
                //------------------------
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextColored(TextColor, "Name");
                ImGui::TableNextColumn();
                ImGui::Text("%s", NodeName);

                //------------------------
                // Static Description
                //------------------------
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextColored(TextColor, "Description");
                ImGui::TableNextColumn();
                ImGui::Text("%s", TCHAR_TO_ANSI(*Node->GetStaticDescription()));

                //------------------------
                // Runtime Values
                //------------------------
                TArray<FString> RunTimeValues;
                uint8* NodeMemory = BehaviorTreeComponent.GetNodeMemory(Node, BehaviorTreeComponent.GetActiveInstanceIdx());
                Node->DescribeRuntimeValues(BehaviorTreeComponent, NodeMemory, EBTDescriptionVerbosity::Detailed, RunTimeValues);

                for (const FString& RuntimeValue : RunTimeValues)
                {
                    ImGui::TableNextRow();

                    FString Left, Right;
                    if (RuntimeValue.Split(TEXT(": "), &Left, &Right))
                    {
                        ImGui::TableNextColumn();
                        ImGui::TextColored(TextColor, TCHAR_TO_ANSI(*Left));

                        ImGui::TableNextColumn();
                        ImGui::Text("%s", TCHAR_TO_ANSI(*Right));
                    }
                    else
                    {
                        ImGui::TableNextColumn();
                        ImGui::TextColored(TextColor, "Value");
                        ImGui::TableNextColumn();
                        ImGui::Text(TCHAR_TO_ANSI(*RuntimeValue));
                    }
                }

                ImGui::EndTable();
            }

            FCogWindowWidgets::EndTableTooltip();
        }

        //------------------------
        // Checkbox
        //------------------------
        ImGui::SameLine();
        FCogWindowWidgets::PushStyleCompact();
        if (IsActive == false)
        {
            ImGui::BeginDisabled();
        }
        bool DrawActive = IsActive;
        ImGui::Checkbox("##Active", &DrawActive);

        if (IsActive == false)
        {
            ImGui::EndDisabled();
        }
        FCogWindowWidgets::PopStyleCompact();

        //------------------------
        // Name
        //------------------------
        ImGui::SameLine();
        const ImVec4 NameColor = IsActive ? FCogImguiHelper::ToImVec4(Config->ActiveColor) : FCogImguiHelper::ToImVec4(Config->InactiveColor);
        ImGui::TextColored(NameColor, "%s", NodeName);
    }

    //------------------------
    // Children
    //------------------------
    if (OpenChildren || Filter.IsActive())
    {
        if (CompositeNode != nullptr)
        {
            for (int32 i = 0; i < CompositeNode->GetChildrenNum(); ++i)
            {
                UBTNode* ChildNode = CompositeNode->GetChildNode(i);
                RenderNode(BehaviorTreeComponent, ChildNode, OpenAllChildren);
            }
        }
    }

    if (ShowNode)
    {
        if (OpenChildren)
        {
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}

