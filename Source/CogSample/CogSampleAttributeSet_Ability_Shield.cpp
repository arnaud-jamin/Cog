#include "CogSampleAttributeSet_Ability_Shield.h"

#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

//--------------------------------------------------------------------------------------------------------------------------
UCogSampleAttributeSet_Ability_Shield::UCogSampleAttributeSet_Ability_Shield()
{
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogSampleAttributeSet_Ability_Shield::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = true;
    Params.RepNotifyCondition = REPNOTIFY_Always;

    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Ability_Shield, Cooldown, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(UCogSampleAttributeSet_Ability_Shield, Cost, Params);
}
