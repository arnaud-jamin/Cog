#pragma once

#include "CoreMinimal.h"

#ifndef USE_COG
#define USE_COG (ENABLE_DRAW_DEBUG && !NO_LOGGING)
#endif

#if USE_COG

#include "CogDebugLogMacros.h"
#include "CogDebugSettings.h"

#define IF_COG(expr)        { expr; }
#define COG_LOG_CATEGORY    FLogCategoryBase

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

#else //USE_COG

#define IF_COG(expr)            (0)
#define COG_LOG_CATEGORY        FNoLoggingCategory
#define COG_LOG_ABILITY(...)    (0)

#endif //USE_COG
