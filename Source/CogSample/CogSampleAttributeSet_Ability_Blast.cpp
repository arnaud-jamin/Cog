#include "CogSampleAttributeSet_Ability_Blast.h"

#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleAttributeSet_Ability_Blast::UCogSampleAttributeSet_Ability_Blast()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Ability_Blast::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    Params.RepNotifyCondition = REPNOTIFY_Always;

    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Ability_Blast, Cooldown, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Ability_Blast, Cost, Params);
}
