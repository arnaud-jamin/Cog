#include "CogAIWindow_BehaviorTree.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTNode.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BrainComponent.h"
#include "CogWindowWidgets.h"
#include "GameFramework/Pawn.h"
#include "imgui_internal.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogAIWindow_BehaviorTree::RenderHelp()
{
    ImGui::Text(
        "This window displays the behavior tree of the selected actor. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
UCogAIWindow_BehaviorTree::UCogAIWindow_BehaviorTree()
{
    bHasMenu = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAIWindow_BehaviorTree::RenderContent()
{
    Super::RenderContent();


    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
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
        RenderNode(BehaviorTreeComponent, RootNodeInstanced, false);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogAIWindow_BehaviorTree::RenderNode(UBehaviorTreeComponent* BehaviorTreeComponent, const UBTNode* Node, bool OpenAllChildren)
{
    const char* NodeName = TCHAR_TO_ANSI(*Node->GetNodeName());
    const bool ShowNode = Filter.PassFilter(NodeName);

    const UBTCompositeNode* CompositeNode = Cast<UBTCompositeNode>(Node);

    bool IsActive = false;
    for (const UBTNode* ActiveParentNode = BehaviorTreeComponent->GetActiveNode(); ActiveParentNode != nullptr; ActiveParentNode = ActiveParentNode->GetParentNode())
    {
        if (Node == ActiveParentNode)
        {
            IsActive = true;
            break;
        }
    }

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
            ImGui::BeginTooltip();
            ImGui::BeginDisabled();
            ImGui::Text(TCHAR_TO_ANSI(*Node->GetStaticDescription()));

            if (const UBTTask_Wait* Wait = Cast<UBTTask_Wait>(Node))
            {
                ImGui::Text("Wait: %0.2fs", Wait->WaitTime);
            }

            ImGui::EndDisabled();
            ImGui::EndTooltip();
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
        ImVec4 NameColor = IsActive ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 0.6f);
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
                const UBTNode* ChildNode = CompositeNode->GetChildNode(i);
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

