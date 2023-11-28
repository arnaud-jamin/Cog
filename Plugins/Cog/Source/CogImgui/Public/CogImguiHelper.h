#pragma once

#include "CoreMinimal.h"
#include "imgui.h"
#include "Layout/SlateRect.h"

COGIMGUI_API DECLARE_LOG_CATEGORY_EXTERN(LogCogImGui, Warning, All);

struct ImGuiWindow;

using CogTextureIndex = int32;

constexpr bool ForwardEvent = false;
constexpr bool TerminateEvent = true;

class COGIMGUI_API FCogImguiHelper
{
public:

    static FString GetIniSaveDirectory();

    static FString GetIniFilePath(const FString& Filename);

    static ImGuiWindow* GetCurrentWindow();

    static FColor ToFColor(ImU32 Color);

    static FSlateRect ToSlateRect(const ImVec4& Value);

    static FVector2f ToFVector2f(const ImVec2& Value);

    static FVector2D ToFVector2D(const ImVec2& Value);

    static ImVec2 ToImVec2(const FVector2D& Value);

    static ImColor ToImColor(const FColor& Value);

    static ImColor ToImColor(const FLinearColor& Value);

    static ImVec4 ToImVec4(const FColor& Value);

    static ImVec4 ToImVec4(const FLinearColor& Value);

    static ImVec4 ToImVec4(const FVector4f& Value);

    static ImU32 ToImU32(const FColor& Value);

    static ImU32 ToImU32(const FVector4f& Value);

    static CogTextureIndex ToTextureIndex(ImTextureID Index);

    static ImTextureID ToImTextureID(CogTextureIndex Index);

    static FVector2D RoundVector(const FVector2D& Vector);

    static FSlateRenderTransform RoundTranslation(const FSlateRenderTransform& Transform);

    static void SetFlags(int32& Value, int32 Flags, bool EnableFlags);
};
