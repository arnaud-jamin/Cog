#pragma  once

#include "CoreMinimal.h"
#include "CogCommonLogCategory.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct COGCOMMON_API FCogLogCategory
{
    GENERATED_USTRUCT_BODY()

    FCogLogCategory() {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Name;

    FString GetName() const { return Name.ToString(); }

    mutable FLogCategoryBase* LogCategory = nullptr;
};
