#pragma once

#include "CoreMinimal.h"

#ifndef USE_COG
#define USE_COG (ENABLE_DRAW_DEBUG && !NO_LOGGING)
#endif

#if USE_COG

#define IF_COG(expr)        { expr; }
#define COG_LOG_CATEGORY    FLogCategoryBase

#else //USE_COG

#define IF_COG(expr)        (0)
#define COG_LOG_CATEGORY    FNoLoggingCategory

#endif //USE_COG
