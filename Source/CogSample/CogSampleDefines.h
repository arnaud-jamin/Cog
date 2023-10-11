#pragma once

#include "CoreMinimal.h"
#include "CogCommon.h"

#if ENABLE_COG

#include "CogDebugSettings.h"

#define COG_LOG_ABILITY(Verbosity, Ability, Format, ...)                                                                    \
    if (Ability != nullptr)                                                                                                 \
    {                                                                                                                       \
        AActor* Actor = Ability->GetAvatarActorFromActorInfo();                                                             \
        if (FCogDebugSettings::IsDebugActiveForObject(Actor) || (int32)Verbosity <= (int32)ELogVerbosity::Warning)          \
        {                                                                                                                   \
            COG_LOG(LogCogAbility, Verbosity, TEXT("%s - %s - %s - %s"),                                                    \
                *GetNameSafe(Actor),                                                                                        \
                *GetNameSafe(Ability),                                                                                      \
                ANSI_TO_TCHAR(__FUNCTION__),                                                                                \
                *FString::Printf(Format, ##__VA_ARGS__));                                                                   \
        }                                                                                                                   \
    }                                                                                                                       \

#else //ENABLE_COG

#define COG_LOG_ABILITY(...)    (0)

#endif //ENABLE_COG
