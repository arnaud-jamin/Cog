#pragma once

#include "CoreMinimal.h"
#include "imgui.h"
#include "Layout/SlateRect.h"

struct ImGuiWindow;

using CogTextureIndex = int32;

class COGIMGUI_API FCogImguiHelper
{
public:

    static FString GetIniSaveDirectory();

    static FString GetIniFilePath(const FString& Filename);

    static ImGuiWindow* GetCurrentWindow();

    static FColor UnpackImU32Color(ImU32 Color);

    static FSlateRect ToSlateRect(const ImVec4& Value);

    static FVector2D ToVector2D(const ImVec2& Value);

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

};
