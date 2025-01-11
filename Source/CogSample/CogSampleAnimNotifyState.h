#pragma once

#include "CoreMinimal.h"

#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "CogSampleAnimNotifyState.generated.h"

UCLASS()
class UCogSampleAnimNotifyState : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    UCogSampleAnimNotifyState(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintNativeEvent)
    FString GetDebugInfo() const;

    void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

    void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

    void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;

    void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};
