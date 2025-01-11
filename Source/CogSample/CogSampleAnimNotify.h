#pragma once

#include "CoreMinimal.h"

#include "Animation/AnimNotifies/AnimNotify.h"

#include "CogSampleAnimNotify.generated.h"

UCLASS()
class UCogSampleAnimNotify : public UAnimNotify
{
    GENERATED_BODY()

public:
    UCogSampleAnimNotify(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintNativeEvent)
    FString GetDebugInfo() const;

    void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

    void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};
