#pragma once

#include "CoreMinimal.h"

#ifndef USE_COG
#define USE_COG (ENABLE_DRAW_DEBUG && !NO_LOGGING)
#endif

#if !USE_COG

#define COG(expr)   (0)

#else //!ENABLE_COG

#define COG(expr)   { expr; }

#endif //!ENABLE_COG
