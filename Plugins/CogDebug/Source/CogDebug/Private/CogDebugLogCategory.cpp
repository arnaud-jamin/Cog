#include "CogDebugLogCategory.h"

#include "CogDebugLogCategoryManager.h"

//--------------------------------------------------------------------------------------------------------------------------
FLogCategoryBase* FCogLogCategory::GetLogCategory() const
{
#if NO_LOGGING

    return nullptr;

#else

    if (Name.IsNone() || Name.IsValid() == false)
    {
        return nullptr;
    }

    if (LogCategory == nullptr)
    {
        if (FCogDebugLogCategoryInfo* CategoryInfo = FCogDebugLogCategoryManager::GetLogCategories().Find(Name))
        {
            LogCategory = CategoryInfo->LogCategory;
        }
    }

    return LogCategory;

#endif //NO_LOGGING
}

