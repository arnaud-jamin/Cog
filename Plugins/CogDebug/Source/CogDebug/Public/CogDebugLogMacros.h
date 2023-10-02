#pragma  once

#include "CoreMinimal.h"
#include "CogDebugDefines.h"
#include "CogDebugSettings.h"
#include "Templates/IsArrayOrRefOfType.h"

#if !ENABLE_COG

#define COG_LOG(LogCategory, Verbosity, Format, ...)                            (0)
#define COG_LOG_FUNC(LogCategory, Verbosity, Format, ...)                       (0)
#define COG_LOG_ACTOR(LogCategory, Verbosity, Actor, Format, ...)               (0)
#define COG_LOG_ACTOR_NO_CONTEXT(LogCategory, Verbosity, Actor, Format, ...)    (0)

#else //!ENABLE_COG

//--------------------------------------------------------------------------------------------------------------------------
#define COG_LOG_ACTIVE_FOR_ACTOR(Actor)   (FCogDebugSettings::IsDebugActiveForActor(Actor))

//--------------------------------------------------------------------------------------------------------------------------
#define COG_LOG(LogCategory, Verbosity, Format, ...)                                                                        \
{                                                                                                                           \
    static_assert(TIsArrayOrRefOfType<decltype(Format), TCHAR>::Value, "Formatting string must be a TCHAR array.");         \
    if ((LogCategory).IsSuppressed(Verbosity) == false)                                                                     \
    {                                                                                                                       \
        FMsg::Logf_Internal(nullptr, 0, (LogCategory).GetCategoryName(), Verbosity, Format, ##__VA_ARGS__);                 \
    }                                                                                                                       \
}                                                                                                                           \

//--------------------------------------------------------------------------------------------------------------------------
#define COG_LOG_FUNC(LogCategory, Verbosity, Format, ...)                                                                   \
    COG_LOG(LogCategory, Verbosity, TEXT("%s - %s"), ANSI_TO_TCHAR(__FUNCTION__), *FString::Printf(Format, ##__VA_ARGS__)); \

//--------------------------------------------------------------------------------------------------------------------------
#define COG_LOG_ACTOR(LogCategory, Verbosity, Actor, Format, ...)                                                           \
    if (COG_LOG_ACTIVE_FOR_ACTOR(Actor) || (int32)Verbosity <= (int32)ELogVerbosity::Warning)                               \
    {                                                                                                                       \
        COG_LOG(LogCategory, Verbosity, TEXT("%s - %s - %s"),                                                               \
            *GetNameSafe(Actor),                                                                                            \
            ANSI_TO_TCHAR(__FUNCTION__),                                                                                    \
            *FString::Printf(Format, ##__VA_ARGS__));                                                                       \
    }                                                                                                                       \

//--------------------------------------------------------------------------------------------------------------------------
#define COG_LOG_ACTOR_NO_CONTEXT(LogCategory, Verbosity, Actor, Format, ...)                                                \
    if (COG_LOG_ACTIVE_FOR_ACTOR(Actor) || (int32)Verbosity <= (int32)ELogVerbosity::Warning)                               \
    {                                                                                                                       \
        COG_LOG(LogCategory, Verbosity, TEXT("%s - %s"), *GetNameSafe(Actor), *FString::Printf(Format, ##__VA_ARGS__));     \
    }                                                                                                                       \


#endif //!ENABLE_COG
