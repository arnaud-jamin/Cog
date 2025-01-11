#include "CogSampleAnimNotifyState.h"

#include "CogCommon.h"

#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifyEndDataContext.h"
#include "Animation/AnimSequenceBase.h"
#include "Components/SkeletalMeshComponent.h"

#if ENABLE_COG
#include "CogDebugPlot.h"
#endif

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleAnimNotifyState::UCogSampleAnimNotifyState(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bIsNativeBranchingPoint = false;
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
#if ENABLE_COG
    FCogDebugPlot::PlotEventStart(MeshComp->GetOwner(), "Anim Notify", GetFName())
        .AddParam("Name", GetNameSafe(this))
        .AddParam("Animation", GetNameSafe(Animation))
        .AddParam("Debug Info", GetDebugInfo());
#endif

    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
#if ENABLE_COG
    FCogDebugPlot::PlotEventStop(MeshComp->GetOwner(), "Anim Notify", GetFName());
#endif

    Super::NotifyEnd(MeshComp, Animation, EventReference);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAnimNotifyState::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
    //--------------------------------------------------------------------------------------------------------
    // Replace UAnimNotifyState::BranchingPointNotifyBegin to fill the EventReference with the NotifyEvent for
    // UGPCoreAnimMotifyFunctionLibrary::AnimNotifyEventReferenceGetMontage to work properly
    //--------------------------------------------------------------------------------------------------------
    // Super::BranchingPointNotifyBegin(BranchingPointPayload);

    const FAnimNotifyEventReference EventReference(BranchingPointPayload.NotifyEvent,
                                                   BranchingPointPayload.SequenceAsset);

    NotifyBegin(BranchingPointPayload.SkelMeshComponent,
                BranchingPointPayload.SequenceAsset,
                BranchingPointPayload.NotifyEvent
                    ? BranchingPointPayload.NotifyEvent->GetDuration()
                    : 0.f,
                EventReference);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAnimNotifyState::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
    //--------------------------------------------------------------------------------------------------------
    // Replace UAnimNotifyState::BranchingPointNotifyEnd to fill the EventReference with the NotifyEvent for
    // UGPCoreAnimMotifyFunctionLibrary::AnimNotifyEventReferenceGetMontage to work properly
    //--------------------------------------------------------------------------------------------------------
    // Super::BranchingPointNotifyEnd(BranchingPointPayload);

    FAnimNotifyEventReference EventReference(BranchingPointPayload.NotifyEvent, BranchingPointPayload.SequenceAsset);
    if (BranchingPointPayload.bReachedEnd)
    {
        EventReference.AddContextData<UE::Anim::FAnimNotifyEndDataContext>(true);
    }
    NotifyEnd(BranchingPointPayload.SkelMeshComponent, BranchingPointPayload.SequenceAsset, EventReference);
}

//--------------------------------------------------------------------------------------------------------------------------
FString UCogSampleAnimNotifyState::GetDebugInfo_Implementation() const
{
    FString DebugInfo;
    return DebugInfo;
}
