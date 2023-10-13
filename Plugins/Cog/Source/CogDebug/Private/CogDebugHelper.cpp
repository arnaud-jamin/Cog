#include "CogDebugHelper.h"

//--------------------------------------------------------------------------------------------------------------------------
FColor FCogDebugHelper::GetAutoColor(FName Name, const FColor& UserColor)
{
    if (UserColor != FColor::Transparent)
    {
        return UserColor;
    }
    else
    {
        uint32 Hash = GetTypeHash(Name.ToString());
        FMath::RandInit(Hash);

        const uint8 Hue = (uint8)(FMath::FRand() * 255);
        const uint8 Saturation = 255;
        const uint8 Value = FMath::Rand() > 0.5f ? 200 : 255;

        return FLinearColor::MakeFromHSV8(Hue, Saturation, Value).ToFColor(true);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
const char* FCogDebugHelper::VerbosityToString(ELogVerbosity::Type Verbosity)
{
    switch (Verbosity & ELogVerbosity::VerbosityMask)
    {
        case ELogVerbosity::NoLogging:      return "No Logging";
        case ELogVerbosity::Fatal:          return "Fatal";
        case ELogVerbosity::Error:          return "Error";
        case ELogVerbosity::Warning:        return "Warning";
        case ELogVerbosity::Display:        return "Display";
        case ELogVerbosity::Log:            return "Log";
        case ELogVerbosity::Verbose:        return "Verbose";
        case ELogVerbosity::VeryVerbose:    return "Very Verbose";
    }

    return "None";
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogDebugHelper::ShortenEnumName(FString EnumNameString)
{
    int32 ScopeIndex = EnumNameString.Find(TEXT("::"), ESearchCase::CaseSensitive);
    if (ScopeIndex != INDEX_NONE)
    {
        return EnumNameString.Mid(ScopeIndex + 2);
    }
    return EnumNameString;
}
