#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CogWindow.h"
#include "CogEngineWindow_Skeleton.generated.h"

class USkeletalMeshComponent;

struct FBoneInfo
{
    FName Name;
    TArray<int32> Children;
    int32 Index = 0;
    int32 ParentIndex = INDEX_NONE;
    FVector LastLocation = FVector::ZeroVector;
    bool IsSecondaryBone = false;

    bool ShowBone = true;
    bool ShowName = false;
    bool ShowLocalVelocity = false;
    bool ShowAxes = false;
    bool ShowTrajectory = false;
};

UCLASS()
class COGENGINE_API UCogEngineWindow_Skeleton : public UCogWindow
{
    GENERATED_BODY()

public:
    UCogEngineWindow_Skeleton();

    virtual void RenderHelp() override;

protected:

    virtual void RenderContent() override;

    virtual void RenderTick(float DeltaTime) override;

    virtual void OnSelectionChanged(AActor* OldSelection, AActor* NewSelection) override;

private:
    void DrawBoneEntry(int32 BoneIndex, bool OpenAllChildren);
    void SetChildrenVisibility(int32 BoneIndex, bool IsVisible);
    void DrawSkeleton();
    void RefreshSkeleton();

    TWeakObjectPtr<USkeletalMeshComponent> CurrentSkeleton = nullptr;
    bool ShowBones = true;
    bool ShowNames = false;
    bool ShowAxes = false;
    bool ShowVelocities = false;
    bool ShowTrajectories = false;
    bool HideSecondaryBones = true;
    int32 HoveredBoneIndex = INDEX_NONE;
    TArray<FBoneInfo> BonesInfos;
    ImGuiTextFilter Filter;
};