#include "CogEngineWindow_Skeleton.h"

#include "CogDebugSettings.h"
#include "Components/LineBatchComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogEngineWindow_Skeleton::UCogEngineWindow_Skeleton()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Skeleton::PreRender(ImGuiWindowFlags& WindowFlags)
{
    WindowFlags = ImGuiWindowFlags_MenuBar;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Skeleton::OnSelectionChanged(AActor* OldSelection, AActor* NewSelection)
{
    RefreshSkeleton();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Skeleton::RenderTick(float DeltaTime)
{
    Super::RenderTick(DeltaTime);

    if (GetIsVisible() == false)
    {
        return;
    }

    DrawSkeleton();
}


//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Skeleton::RefreshSkeleton()
{
    CurrentSkeleton = nullptr;
    BonesInfos.Empty();


    AActor* Selection = GetSelection();
    if (Selection == nullptr)
    {
        return;
    }

    CurrentSkeleton = Selection->FindComponentByClass<USkeletalMeshComponent>();
    if (CurrentSkeleton == nullptr)
    {
        return;
    }
    
    const FTransform WorldTransform = CurrentSkeleton->GetComponentTransform();
    const TArray<FTransform>& ComponentSpaceTransforms = CurrentSkeleton->GetComponentSpaceTransforms();
    BonesInfos.SetNum(ComponentSpaceTransforms.Num());

    for (int32 BoneIndex = 0; BoneIndex < ComponentSpaceTransforms.Num(); ++BoneIndex)
    {
        const FReferenceSkeleton& ReferenceSkeleton = CurrentSkeleton->GetSkeletalMeshAsset()->GetRefSkeleton();

        FBoneInfo& CurrentBoneInfo = BonesInfos[BoneIndex];
        CurrentBoneInfo.Index = BoneIndex;
        CurrentBoneInfo.Name = ReferenceSkeleton.GetBoneName(BoneIndex);
        const FTransform Transform = ComponentSpaceTransforms[BoneIndex] * WorldTransform;
        CurrentBoneInfo.LastLocation = Transform.GetLocation();
        CurrentBoneInfo.IsSecondaryBone = FCogDebugSettings::IsSecondarySkeletonBone(CurrentBoneInfo.Name);
        CurrentBoneInfo.ShowBone = !(HideSecondaryBones && CurrentBoneInfo.IsSecondaryBone);

        const int32 ParentIndex = ReferenceSkeleton.GetParentIndex(BoneIndex);
        if (ParentIndex != INDEX_NONE)
        {
            FBoneInfo& ParentBoneInfo = BonesInfos[ParentIndex];
            ParentBoneInfo.Children.Add(BoneIndex);
            CurrentBoneInfo.ParentIndex = ParentIndex;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Skeleton::RenderContent()
{
    Super::RenderContent();

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Hide secondary bones", &HideSecondaryBones);
            ImGui::Separator();
            ImGui::Checkbox("Show bones", &ShowBones);
            ImGui::Checkbox("Show name", &ShowNames);
            ImGui::Checkbox("Show axes", &ShowAxes);
            ImGui::Checkbox("Show local velocity", &ShowVelocities);
            ImGui::Checkbox("Show trajectories", &ShowTrajectories);
            ImGui::EndMenu();
        }

        FCogWindowWidgets::MenuSearchBar(Filter);

        ImGui::EndMenuBar();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, FCogWindowWidgets::GetFontWidth());

    HoveredBoneIndex = INDEX_NONE;
    DrawBoneEntry(0, false);

    ImGui::PopStyleVar();
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Skeleton::DrawBoneEntry(int32 BoneIndex, bool OpenAllChildren)
{
    if (BonesInfos.IsValidIndex(BoneIndex) == false)
    {
        return;
    }

    FBoneInfo& BoneInfo = BonesInfos[BoneIndex];

    if (HideSecondaryBones && BoneInfo.IsSecondaryBone)
    {
        return;
    }

    const char* BoneName = TCHAR_TO_ANSI(*BoneInfo.Name.ToString());
    const bool ShowNode = Filter.PassFilter(BoneName);
    bool OpenChildren = false;

    if (ShowNode)
    {
        ImGui::PushID(BoneIndex);

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
        if (BoneInfo.Children.Num() > 0 && Filter.IsActive() == false)
        {
            OpenChildren = ImGui::TreeNodeEx("##Bone", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanFullWidth);
        }
        else
        {
            ImGui::TreeNodeEx("##Bone", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanFullWidth);
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
            ImGui::Checkbox("Show Name", &BoneInfo.ShowName);
            ImGui::Checkbox("Show Axe", &BoneInfo.ShowAxes);
            ImGui::Checkbox("Show Local Velocity", &BoneInfo.ShowLocalVelocity);
            ImGui::Checkbox("Show Trajectory", &BoneInfo.ShowTrajectory);
            ImGui::EndPopup();
            HoveredBoneIndex = BoneIndex;
        }

        //------------------------
        // Tooltip
        //------------------------
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::BeginDisabled();
            ImGui::Checkbox("Show Name", &BoneInfo.ShowName);
            ImGui::Checkbox("Show Axe", &BoneInfo.ShowAxes);
            ImGui::Checkbox("Show Local Velocity", &BoneInfo.ShowLocalVelocity);
            ImGui::Checkbox("Show Trajectory", &BoneInfo.ShowTrajectory);
            ImGui::EndDisabled();
            ImGui::EndTooltip();
            HoveredBoneIndex = BoneIndex;
        }

        //------------------------
        // Checkbox
        //------------------------
        ImGui::SameLine();
        if (ImGui::Checkbox("##Visible", &BoneInfo.ShowBone))
        {
            if (IsControlDown)
            {
                SetChildrenVisibility(BoneIndex, BoneInfo.ShowBone);
            }

            if (BoneInfo.ShowBone == false)
            {
                BoneInfo.ShowName = false;
                BoneInfo.ShowAxes = false;
                BoneInfo.ShowLocalVelocity = false;
                BoneInfo.ShowTrajectory = false;
            }
        }

        const bool HasCustomVisiblity = BoneInfo.ShowName || BoneInfo.ShowAxes || BoneInfo.ShowLocalVelocity || BoneInfo.ShowTrajectory;
        if (HasCustomVisiblity)
        {
            BoneInfo.ShowBone = true;
        }

        //------------------------
        // Name
        //------------------------
        ImGui::SameLine();
        ImVec4 NameColor = HasCustomVisiblity ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImGui::TextColored(NameColor, "%s", BoneName);
    }

    //------------------------
    // Children
    //------------------------
    if (OpenChildren || Filter.IsActive())
    {
        for (int32 ChildIndex : BoneInfo.Children)
        {
            DrawBoneEntry(ChildIndex, OpenAllChildren);
        }
    }

    if (ShowNode)
    {
        if (OpenChildren && ShowNode)
        {
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Skeleton::SetChildrenVisibility(int32 BoneIndex, bool IsVisible)
{
    if (BonesInfos.IsValidIndex(BoneIndex) == false)
    {
        return;
    }

    FBoneInfo& BoneInfo = BonesInfos[BoneIndex];
    for (int32 ChildIndex : BoneInfo.Children)
    {
        FBoneInfo& ChildBoneInfo = BonesInfos[ChildIndex];
        ChildBoneInfo.ShowBone = IsVisible && !(HideSecondaryBones && ChildBoneInfo.IsSecondaryBone);
        SetChildrenVisibility(ChildIndex, IsVisible);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Skeleton::DrawSkeleton()
{
    if (CurrentSkeleton == nullptr || CurrentSkeleton->GetWorld() == nullptr)
    {
        return;
    }

    const UWorld* World = CurrentSkeleton->GetWorld();
    const FTransform WorldTransform = CurrentSkeleton->GetComponentTransform();
    const TArray<FTransform>& ComponentSpaceTransforms = CurrentSkeleton->GetComponentSpaceTransforms();

    for (int32 BoneIndex = 0; BoneIndex < ComponentSpaceTransforms.Num(); ++BoneIndex)
    {
        FBoneInfo& BoneInfo = BonesInfos[BoneIndex];

        const FTransform Transform = ComponentSpaceTransforms[BoneIndex] * WorldTransform;
        FVector BoneLocation = Transform.GetLocation();
        FRotator BoneRotation = FRotator(Transform.GetRotation());

        FVector ParentLocation;
        if (BoneInfo.ParentIndex >= 0)
        {
            ParentLocation = (ComponentSpaceTransforms[BoneInfo.ParentIndex] * WorldTransform).GetLocation();
        }
        else
        {
            ParentLocation = WorldTransform.GetLocation();
        }

        if (BoneInfo.ShowBone)
        {
            const bool IsHovered = BoneIndex == HoveredBoneIndex;

            if (ShowBones)
            {
                ::DrawDebugLine(World, ParentLocation, BoneLocation, IsHovered ? FColor::Red : FColor::White, false, 0.0f, 1, FCogDebugSettings::GetDebugThickness(IsHovered ? 0.5f : 0.0f));
                ::DrawDebugPoint(World, BoneLocation, FCogDebugSettings::GetDebugThickness(IsHovered ? 6.0f : 4.0f), IsHovered ? FColor::Red : FColor::White, false, 0.0f, 1);
            }

            if (ShowNames || BoneInfo.ShowName || IsHovered)
            {
                ::DrawDebugString(World, BoneLocation, BoneInfo.Name.ToString(), nullptr, IsHovered ? FColor::Red : FColor::White, 0.0f, true, FCogDebugSettings::TextSize);
            }

            if (ShowAxes || BoneInfo.ShowAxes)
            {
                ::DrawDebugCoordinateSystem(
                    World,
                    BoneLocation,
                    BoneRotation,
                    10.0f * FCogDebugSettings::AxesScale,
                    false,
                    0.0f,
                    1,
                    FCogDebugSettings::GetDebugThickness(0.0f));
            }

            if (ShowVelocities || BoneInfo.ShowLocalVelocity)
            {
                if (const FBodyInstance* ParentBodyInstance = CurrentSkeleton->GetBodyInstance(BoneInfo.Name))
                {
                    DrawDebugDirectionalArrow(
                        World,
                        BoneLocation,
                        BoneLocation + ParentBodyInstance->GetUnrealWorldVelocity() * World->GetDeltaSeconds(),
                        FCogDebugSettings::ArrowSize,
                        FCogDebugSettings::ModulateDebugColor(World, FColor::Cyan),
                        FCogDebugSettings::GetDebugPersistent(true),
                        FCogDebugSettings::GetDebugDuration(true),
                        0,
                        FCogDebugSettings::GetDebugThickness(0.0f));
                }
            }

            if (ShowTrajectories || BoneInfo.ShowTrajectory)
            {
                const FColor Color = FCogDebugSettings::ModulateDebugColor(World, FColor::Yellow);
                DrawDebugLine(
                    World,
                    BoneInfo.LastLocation,
                    BoneLocation,
                    Color,
                    FCogDebugSettings::GetDebugPersistent(true),
                    FCogDebugSettings::GetDebugDuration(true),
                    0,
                    FCogDebugSettings::GetDebugThickness(0.0f));

                DrawDebugPoint(
                    World,
                    BoneLocation,
                    FCogDebugSettings::GetDebugThickness(2.0f),
                    Color,
                    FCogDebugSettings::GetDebugPersistent(true),
                    FCogDebugSettings::GetDebugDuration(true),
                    0);
            }

            BoneInfo.LastLocation = BoneLocation;
        }
    }
}

