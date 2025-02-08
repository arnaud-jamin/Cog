#pragma once

#include "CoreMinimal.h"
#include "Templates/IsArrayOrRefOfType.h"
#include "CogCommonLogCategory.h"

#ifndef ENABLE_COG
#define ENABLE_COG !UE_BUILD_SHIPPING
#endif

#if ENABLE_COG

#include "CogDebug.h"

#define IF_COG(expr)            { expr; }
#define COG_LOG_CATEGORY        FLogCategoryBase

//--------------------------------------------------------------------------------------------------------------------------
#define COG_LOG_ACTIVE_FOR_OBJECT(Object)   (FCogDebug::IsDebugActiveForObject(Object))

//--------------------------------------------------------------------------------------------------------------------------
#define COG_NOTIFY(Format, ...)                                                                                             \
{                                                                                                                           \
    static_assert(TIsArrayOrRefOfType<decltype(Format), TCHAR>::Value, "Formatting string must be a TCHAR array.");         \
    FMsg::Logf_Internal(nullptr, 0, LogCogNotify.GetCategoryName(), ELogVerbosity::Log, Format, ##__VA_ARGS__);             \
}

//--------------------------------------------------------------------------------------------------------------------------
#define COG_NOTIFY_WARNING(Format, ...)                                                                                     \
{                                                                                                                           \
    static_assert(TIsArrayOrRefOfType<decltype(Format), TCHAR>::Value, "Formatting string must be a TCHAR array.");         \
    FMsg::Logf_Internal(nullptr, 0, LogCogNotify.GetCategoryName(), ELogVerbosity::Warning, Format, ##__VA_ARGS__);         \
}

//--------------------------------------------------------------------------------------------------------------------------
#define COG_NOTIFY_ERROR(Format, ...)                                                                                       \
{                                                                                                                           \
    static_assert(TIsArrayOrRefOfType<decltype(Format), TCHAR>::Value, "Formatting string must be a TCHAR array.");         \
    FMsg::Logf_Internal(nullptr, 0, LogCogNotify.GetCategoryName(), ELogVerbosity::Error, Format, ##__VA_ARGS__);           \
}

//--------------------------------------------------------------------------------------------------------------------------
#define COG_NOTIFY_VERBOSITY(Verbosity, Format, ...)                                                                        \
{                                                                                                                           \
    static_assert(TIsArrayOrRefOfType<decltype(Format), TCHAR>::Value, "Formatting string must be a TCHAR array.");         \
    if (LogCogNotify.IsSuppressed(Verbosity) == false)                                                                      \
    {                                                                                                                       \
        FMsg::Logf_Internal(nullptr, 0, LogCogNotify.GetCategoryName(), Verbosity, Format, ##__VA_ARGS__);                  \
    }                                                                                                                       \
}

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
    COG_LOG(LogCategory, Verbosity, TEXT("%s | %s"), ANSI_TO_TCHAR(__FUNCTION__), *FString::Printf(Format, ##__VA_ARGS__)); \

//--------------------------------------------------------------------------------------------------------------------------
#define COG_LOG_OBJECT(LogCategory, Verbosity, Object, Format, ...)                                                         \
    if (COG_LOG_ACTIVE_FOR_OBJECT(Object) || (int32)Verbosity <= (int32)ELogVerbosity::Warning)                             \
    {                                                                                                                       \
        COG_LOG(LogCategory, Verbosity, TEXT("%s | %s | %s"),                                                               \
            *GetNameSafe(Object),                                                                                           \
            ANSI_TO_TCHAR(__FUNCTION__),                                                                                    \
            *FString::Printf(Format, ##__VA_ARGS__));                                                                       \
    }                                                                                                                       \

//--------------------------------------------------------------------------------------------------------------------------
#define COG_LOG_OBJECT_NO_CONTEXT(LogCategory, Verbosity, Object, Format, ...)                                              \
    if (COG_LOG_ACTIVE_FOR_OBJECT(Object) || (int32)Verbosity <= (int32)ELogVerbosity::Warning)                             \
    {                                                                                                                       \
        COG_LOG(LogCategory, Verbosity, TEXT("%s | %s"), *GetNameSafe(Object), *FString::Printf(Format, ##__VA_ARGS__));    \
    }                                                                                                                       \

#else //ENABLE_COG

#define IF_COG(expr)                                                            (0)
#define COG_LOG_CATEGORY                                                        FNoLoggingCategory
#define COG_LOG_ABILITY(...)                                                    (0)
#define COG_NOTIFY(Format, ...)                                                 (0)
#define COG_NOTIFY_WARNING(Format, ...)                                         (0)
#define COG_NOTIFY_ERROR(Format, ...)                                           (0)
#define COG_NOTIFY_VERBOSITY(Verbosity, Format, ...)                            (0)
#define COG_LOG_ACTIVE_FOR_OBJECT(Object)                                       (0)
#define COG_LOG(LogCategory, Verbosity, Format, ...)                            (0)
#define COG_LOG_FUNC(LogCategory, Verbosity, Format, ...)                       (0)
#define COG_LOG_OBJECT(LogCategory, Verbosity, Actor, Format, ...)              (0)
#define COG_LOG_OBJECT_NO_CONTEXT(LogCategory, Verbosity, Actor, Format, ...)   (0)

#endif //ENABLE_COG

