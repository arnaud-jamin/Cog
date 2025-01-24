#include "CogSampleFunctionLibrary_Tag.h"

UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Activation_Fail_Cooldown, "Ability.Activation.Fail.Cooldown");
UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Activation_Fail_Cost, "Ability.Activation.Fail.Cost");

UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Cooldown, "Ability.Cooldown");
UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Cooldown_1, "Ability.Cooldown.1");
UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Cooldown_2, "Ability.Cooldown.2");
UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Cooldown_3, "Ability.Cooldown.3");
UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Cooldown_4, "Ability.Cooldown.4");
UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Cooldown_5, "Ability.Cooldown.5");
UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Cooldown_6, "Ability.Cooldown.6");
UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Cooldown_7, "Ability.Cooldown.7");
UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Cooldown_8, "Ability.Cooldown.8");

UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Type_Active, "Ability.Type.Active");
UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Type_Passive, "Ability.Type.Passive");
UE_DEFINE_GAMEPLAY_TAG(Tag_Ability_Type_Hidden, "Ability.Type.Hidden");

UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Alignment_Negative, "Effect.Alignment.Negative");
UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Alignment_Positive, "Effect.Alignment.Positive");

UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Data_Amount, "Effect.Data.Amount");
UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Data_Cooldown, "Effect.Data.Cooldown");
UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Data_Cost, "Effect.Data.Cost");
UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Data_Damage, "Effect.Data.Damage");
UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Data_Heal, "Effect.Data.Heal");

UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Type_Cost_Free, "Effect.Type.Cost.Free");
UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Type_Cost_Overtime, "Effect.Type.Cost.Overtime");
UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Type_Cost_Stamina, "Effect.Type.Cost.Stamina");
UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Type_Damage_Kill, "Effect.Type.Damage.Kill");
UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Type_Heal_Full, "Effect.Type.Heal.Full");
UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Type_Heal_Revive, "Effect.Type.Heal.Revive");
UE_DEFINE_GAMEPLAY_TAG(Tag_Effect_Type_Hidden, "Effect.Type.Hidden");

UE_DEFINE_GAMEPLAY_TAG(Tag_GameplayCue_DamageReceived, "GameplayCue.DamageReceived");

UE_DEFINE_GAMEPLAY_TAG(Tag_Status_Dead, "Status.Dead");
UE_DEFINE_GAMEPLAY_TAG(Tag_Status_Ghost, "Status.Ghost");
UE_DEFINE_GAMEPLAY_TAG(Tag_Status_Immobilized, "Status.Immobilized");
UE_DEFINE_GAMEPLAY_TAG(Tag_Status_Invisible, "Status.Invisible");
UE_DEFINE_GAMEPLAY_TAG(Tag_Status_Immune_Damage, "Status.Immune.Damage");
UE_DEFINE_GAMEPLAY_TAG(Tag_Status_Revived, "Status.Revived");
UE_DEFINE_GAMEPLAY_TAG(Tag_Status_Silenced, "Status.Silenced");
UE_DEFINE_GAMEPLAY_TAG(Tag_Status_NoCooldown, "Status.NoColdown");
UE_DEFINE_GAMEPLAY_TAG(Tag_Status_NoCost, "Status.NoCost");
UE_DEFINE_GAMEPLAY_TAG(Tag_Status_Stunned, "Status.Stunned");

UE_DEFINE_GAMEPLAY_TAG(Tag_GameplayEvent_Killed, "GameplayEvent.Killed");
UE_DEFINE_GAMEPLAY_TAG(Tag_GameplayEvent_Revived, "GameplayEvent.Revived");

UE_DEFINE_GAMEPLAY_TAG(Tag_MontageEvent_Cast_Begin, "MontageEvent.Cast.Begin");
UE_DEFINE_GAMEPLAY_TAG(Tag_MontageEvent_Cast_End, "MontageEvent.Cast.End");

UE_DEFINE_GAMEPLAY_TAG(Tag_Unit_Hero, "Unit.Hero");
UE_DEFINE_GAMEPLAY_TAG(Tag_Unit_Creature, "Unit.Creature");

TArray<FGameplayTag> UCogSampleFunctionLibrary_Tag::ActiveAbilityCooldownTags =
{ 
    Tag_Ability_Cooldown_1, 
    Tag_Ability_Cooldown_2, 
    Tag_Ability_Cooldown_3, 
    Tag_Ability_Cooldown_4, 
    Tag_Ability_Cooldown_5,
    Tag_Ability_Cooldown_6,
    Tag_Ability_Cooldown_7,
    Tag_Ability_Cooldown_8,
};
