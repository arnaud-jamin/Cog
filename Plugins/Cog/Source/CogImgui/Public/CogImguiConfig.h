#pragma once

#include "Misc/AssertionMacros.h"
#include "Engine/Texture2D.h"

#define IM_ASSERT(Expr) ensure(Expr)

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define IMGUI_DISABLE_OBSOLETE_KEYIO
#define IMGUI_DISABLE_WIN32_FUNCTIONS
#define IMGUI_DISABLE_DEFAULT_ALLOCATORS
#define IMGUI_DEFINE_MATH_OPERATORS

#define ImTextureID UTexture2D*
