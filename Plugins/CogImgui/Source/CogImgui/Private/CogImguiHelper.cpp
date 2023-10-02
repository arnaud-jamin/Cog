#include "CogImguiHelper.h"

#include "InputCoreTypes.h"
#include "imgui_internal.h"

//----------------------------------------------------------------------------------------------------------------------
FString FCogImguiHelper::GetIniSaveDirectory()
{
    const FString SavedDir = FPaths::ProjectSavedDir();
    const FString Directory = FPaths::Combine(*SavedDir, TEXT("ImGui"));
    IPlatformFile::GetPlatformPhysical().CreateDirectory(*Directory);
    return Directory;
}

//----------------------------------------------------------------------------------------------------------------------
FString FCogImguiHelper::GetIniFilePath(const FString& Filename)
{
    FString SaveDirectory = GetIniSaveDirectory();
    FString FilePath = FPaths::Combine(SaveDirectory, Filename + TEXT(".ini"));
    return FilePath;
}

//--------------------------------------------------------------------------------------------------------------------------
ImGuiWindow* FCogImguiHelper::GetCurrentWindow()
{
    ImGuiContext& Context = *ImGui::GetCurrentContext();
    Context.CurrentWindow->WriteAccessed = true;
    return Context.CurrentWindow;
}

//--------------------------------------------------------------------------------------------------------------------------
FColor FCogImguiHelper::UnpackImU32Color(ImU32 Color)
{
    return FColor
    {
        (uint8)((Color >> IM_COL32_R_SHIFT) & 0xFF),
        (uint8)((Color >> IM_COL32_G_SHIFT) & 0xFF),
        (uint8)((Color >> IM_COL32_B_SHIFT) & 0xFF),
        (uint8)((Color >> IM_COL32_A_SHIFT) & 0xFF)
    };
}

//--------------------------------------------------------------------------------------------------------------------------
FSlateRect FCogImguiHelper::ToSlateRect(const ImVec4& Value)
{
    return FSlateRect{ Value.x, Value.y, Value.z, Value.w };
}

//--------------------------------------------------------------------------------------------------------------------------
FVector2D FCogImguiHelper::ToVector2D(const ImVec2& Value)
{
    return FVector2D(Value.x, Value.y);
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec2 FCogImguiHelper::ToImVec2(const FVector2D& Value)
{
    return ImVec2(Value.X, Value.Y);
}

//--------------------------------------------------------------------------------------------------------------------------
ImColor FCogImguiHelper::ToImColor(const FColor& Value)
{
    return ImColor(Value.R, Value.G, Value.B, Value.A);
}

//--------------------------------------------------------------------------------------------------------------------------
ImColor FCogImguiHelper::ToImColor(const FLinearColor& Value)
{
    return ImColor(Value.R, Value.G, Value.B, Value.A);
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec4 FCogImguiHelper::ToImVec4(const FColor& Value)
{
    return ToImColor(Value).Value;
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec4 FCogImguiHelper::ToImVec4(const FLinearColor& Value)
{
    return ImVec4(Value.R, Value.G, Value.B, Value.A);
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec4 FCogImguiHelper::ToImVec4(const FVector4f& Value)
{
    return ImVec4(Value.X, Value.Y, Value.Z, Value.W);
}


//--------------------------------------------------------------------------------------------------------------------------
ImU32 FCogImguiHelper::ToImU32(const FColor& Value)
{
    return (ImU32)ToImColor(Value);
}

//--------------------------------------------------------------------------------------------------------------------------
ImU32 FCogImguiHelper::ToImU32(const FVector4f& Value)
{
    return ImGui::GetColorU32(ToImVec4(Value));
}

//--------------------------------------------------------------------------------------------------------------------------
CogTextureIndex FCogImguiHelper::ToTextureIndex(ImTextureID Index)
{
    return static_cast<CogTextureIndex>(reinterpret_cast<intptr_t>(Index));
}

//--------------------------------------------------------------------------------------------------------------------------
ImTextureID FCogImguiHelper::ToImTextureID(CogTextureIndex Index)
{
    return reinterpret_cast<ImTextureID>(static_cast<intptr_t>(Index));
}

//--------------------------------------------------------------------------------------------------------------------------
FVector2D FCogImguiHelper::RoundVector(const FVector2D& Vector)
{
    return FVector2D(FMath::RoundToFloat(Vector.X), FMath::RoundToFloat(Vector.Y));
}

//--------------------------------------------------------------------------------------------------------------------------
FSlateRenderTransform FCogImguiHelper::RoundTranslation(const FSlateRenderTransform& Transform)
{
    return FSlateRenderTransform(Transform.GetMatrix(), RoundVector(Transform.GetTranslation()));
}
