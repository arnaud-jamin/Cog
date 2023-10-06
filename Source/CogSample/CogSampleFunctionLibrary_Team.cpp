#include "CogSampleFunctionLibrary_Team.h"

#include "CogSampleTeamInterface.h"

//--------------------------------------------------------------------------------------------------------------------------
ECogSampleAllegiance UCogSampleFunctionLibrary_Team::GetTeamsAllegiance(int32 Team1, int32 Team2)
{
    if (Team1 == 0 || Team2 == 0)
    {
        return ECogSampleAllegiance::Neutral;
    }

    if (Team1 == Team2)
    {
        return ECogSampleAllegiance::Friendly;
    }

    return ECogSampleAllegiance::Enemy;
}

//--------------------------------------------------------------------------------------------------------------------------
ECogSampleAllegiance UCogSampleFunctionLibrary_Team::GetActorsAllegiance(const AActor* Actor1, const AActor* Actor2)
{
    const ICogSampleTeamInterface* TeamActor1 = Cast<ICogSampleTeamInterface>(Actor1);
    const ICogSampleTeamInterface* TeamActor2 = Cast<ICogSampleTeamInterface>(Actor2);

    if (TeamActor1 == nullptr || TeamActor2 == nullptr)
    {
        return ECogSampleAllegiance::Neutral;
    }

    const int32 Team1 = TeamActor1->GetTeam();
    const int32 Team2 = TeamActor2->GetTeam();
    const ECogSampleAllegiance Allegiance = GetTeamsAllegiance(Team1, Team2);

    return Allegiance;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleFunctionLibrary_Team::MatchAllegiance(const AActor* Actor1, const AActor* Actor2, int32 AllegianceFilter)
{
    const ECogSampleAllegiance Allegiance = GetActorsAllegiance(Actor1, Actor2);
    const bool Result = MatchAllegianceFilter(Allegiance, AllegianceFilter);
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleFunctionLibrary_Team::MatchAllegianceFilter(ECogSampleAllegiance Allegiance, int32 AllegianceFilter)
{
    const bool HasFriendly = (AllegianceFilter & (int)ECogSampleAllegianceFilter::Friendly) != 0;
    const bool HasNeutral = (AllegianceFilter & (int)ECogSampleAllegianceFilter::Neutral) != 0;
    const bool HasEnemy = (AllegianceFilter & (int)ECogSampleAllegianceFilter::Enemy) != 0;

    if (Allegiance == ECogSampleAllegiance::Friendly && HasFriendly)
    {
        return true;
    }

    if (Allegiance == ECogSampleAllegiance::Neutral && HasNeutral)
    {
        return true;
    }

    if (Allegiance == ECogSampleAllegiance::Enemy && HasEnemy)
    {
        return true;
    }

    return false;
}