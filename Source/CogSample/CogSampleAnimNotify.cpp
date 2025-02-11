#include "CogSampleAnimNotify.h"

#include "CogCommon.h"

#include "Animation/AnimSequenceBase.h"
#include "Components/SkeletalMeshComponent.h"

#if ENABLE_COG
#include "CogDebug.h"
#endif

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleAnimNotify::UCogSampleAnimNotify(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bIsNativeBranchingPoint = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

#if ENABLE_COG
    FCogDebug::InstantEvent(MeshComp->GetOwner(), "Anim Notify", GetFName())
        .AddParam("Name", GetNameSafe(this))
        .AddParam("Animation", GetNameSafe(Animation))
        .AddParam("Debug Info", GetDebugInfo());
#endif
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAnimNotify::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
    //--------------------------------------------------------------------------------------------------------
    // Replace UAnimNotify::BranchingPointNotify to fill the EventReference with the NotifyEvent for
    // UGPCoreAnimMotifyFunctionLibrary::AnimNotifyEventReferenceGetMontage to work properly
    //--------------------------------------------------------------------------------------------------------
    // Super::BranchingPointNotify(BranchingPointPayload);
    const FAnimNotifyEventReference EventReference(BranchingPointPayload.NotifyEvent,
                                                   BranchingPointPayload.SequenceAsset);

    Notify(BranchingPointPayload.SkelMeshComponent, BranchingPointPayload.SequenceAsset, EventReference);
}

//--------------------------------------------------------------------------------------------------------------------------
FString UCogSampleAnimNotify::GetDebugInfo_Implementation() const
{
    FString DebugInfo;
    return DebugInfo;
}
 