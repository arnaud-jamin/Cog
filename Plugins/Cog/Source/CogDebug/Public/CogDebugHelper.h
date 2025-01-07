#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "UObject/Class.h"

class COGDEBUG_API FCogDebugHelper
{
public:

    static FColor GetAutoColor(FName Name, const FColor& UserColor);

    static const char* VerbosityToString(ELogVerbosity::Type Verbosity);

    static FString ShortenEnumName(FString EnumNameString);

    template<typename EnumType>
    static FString GetValueAsStringShort(const EnumType EnumeratorValue)
    {
        FString EnumNameString = UEnum::GetValueAsString(EnumeratorValue);
        return ShortenEnumName(EnumNameString);
    }

    template<typename EnumType>
    static FName GetValueAsNameShort(const EnumType EnumeratorValue)
    {
        return FName(GetValueAsStringShort(EnumeratorValue));
    }
};
