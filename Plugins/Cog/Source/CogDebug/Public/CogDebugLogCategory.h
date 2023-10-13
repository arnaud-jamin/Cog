#pragma  once

#include "CoreMinimal.h"
#include "CogDebugLogCategory.generated.h"

struct FCogLogCategory;

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct COGDEBUG_API FCogLogCategory
{
    GENERATED_USTRUCT_BODY()

    FCogLogCategory() {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Name;

    FString GetName() const { return Name.ToString(); }

    FLogCategoryBase* GetLogCategory() const;

private:

    mutable FLogCategoryBase* LogCategory = nullptr;
};
