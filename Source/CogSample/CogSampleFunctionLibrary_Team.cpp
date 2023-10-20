#include "CogSampleFunctionLibrary_Team.h"

#include "CogSampleTeamInterface.h"

//--------------------------------------------------------------------------------------------------------------------------
int32 UCogSampleFunctionLibrary_Team::GetTeamSafe(const AActor* Actor)
{
    if (Actor == nullptr)
    {
        return 0;
    }

    const ICogSampleTeamInterface* TeamActor = Cast<ICogSampleTeamInterface>(Actor);
    if (TeamActor == nullptr)
    {
        return 0;
    }

    const int32 Team = TeamActor->GetTeam();
    return Team;
}

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
    const int32 Team1 = GetTeamSafe(Actor1);
    const int32 Team2 = GetTeamSafe(Actor2);
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
bool UCogSampleFunctionLibrary_Team::MatchAllegianceFromTeams(int32 Team1, int32 Team2, int32 AllegianceFilter)
{
    const ECogSampleAllegiance Allegiance = GetTeamsAllegiance(Team1, Team2);
    const bool Result = MatchAllegianceFilter(Allegiance, AllegianceFilter);
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogSampleFunctionLibrary_Team::MatchAllegianceBetweenTeamAndActor(int32 Team, const AActor* Actor, int32 AllegianceFilter)
{
    const int32 ActorTeam = GetTeamSafe(Actor);
    const ECogSampleAllegiance Allegiance = GetTeamsAllegiance(Team, ActorTeam);
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