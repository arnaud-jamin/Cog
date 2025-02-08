#include "CogImguiHelper.h"

#include "GenericPlatform/GenericPlatformFile.h"
#include "imgui_internal.h"
#include "InputCoreTypes.h"
#include "Misc/Paths.h"

DEFINE_LOG_CATEGORY(LogCogImGui);

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
float FCogImguiHelper::GetNextItemWidth()
{
    float Width;
    const ImGuiContext& g = *GImGui;
    if (g.NextItemData.HasFlags & ImGuiNextItemDataFlags_HasWidth)
    {
        Width = g.NextItemData.Width;
    }
    else
    {
	    const ImGuiWindow* Window = GetCurrentWindow();
        Width = Window->DC.ItemWidth;
    }

    return Width;
}

//--------------------------------------------------------------------------------------------------------------------------
FColor FCogImguiHelper::ToFColor(ImU32 Color)
{
    return FColor
    {
        static_cast<uint8>((Color >> IM_COL32_R_SHIFT) & 0xFF),
        static_cast<uint8>((Color >> IM_COL32_G_SHIFT) & 0xFF),
        static_cast<uint8>((Color >> IM_COL32_B_SHIFT) & 0xFF),
        static_cast<uint8>((Color >> IM_COL32_A_SHIFT) & 0xFF)
    };
}

//--------------------------------------------------------------------------------------------------------------------------
FSlateRect FCogImguiHelper::ToSlateRect(const ImVec4& Value)
{
    return FSlateRect{ Value.x, Value.y, Value.z, Value.w };
}

//--------------------------------------------------------------------------------------------------------------------------
FVector2f FCogImguiHelper::ToFVector2f(const ImVec2& Value)
{
    return FVector2f(Value.x, Value.y);
}

//--------------------------------------------------------------------------------------------------------------------------
FVector2D FCogImguiHelper::ToFVector2D(const ImVec2& Value)
{
    return FVector2D(Value.x, Value.y);
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec2 FCogImguiHelper::ToImVec2(const FVector2D& Value)
{
    return ImVec2(Value.X, Value.Y);
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec2 FCogImguiHelper::ToImVec2(const FIntVector2& Value)
{
    return ImVec2(Value.X, Value.Y);
}
//--------------------------------------------------------------------------------------------------------------------------
ImVec2 FCogImguiHelper::ToImVec2(const FVector2f& Value)
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
ImU32 FCogImguiHelper::ToImU32(const FLinearColor& Value)
{
    return ToImColor(Value);
}

//--------------------------------------------------------------------------------------------------------------------------
ImU32 FCogImguiHelper::ToImU32(const FColor& Value)
{
    return ToImColor(Value);
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

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiHelper::SetFlags(int32& Value, int32 Flags, bool EnableFlags)
{
    if (EnableFlags)
    {
        Value |= Flags;
    }
    else
    {
        Value &= ~Flags;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiHelper::DragDouble(const char* Label, double* Value, float Speed, double Min, double Max, const char* Format, ImGuiSliderFlags Flags)
{
    return ImGui::DragScalar(Label, ImGuiDataType_Double, Value, Speed, &Min, &Max, Format, Flags);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiHelper::DragFVector(const char* Label, FVector& Vector, float Speed, double Min, double Max, const char* Format, ImGuiSliderFlags Flags)
{
    return ImGui::DragScalarN(Label, ImGuiDataType_Double, &Vector.X, 3, Speed, &Min, &Max, Format, Flags);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiHelper::DragFVector(const char* Label, FVector& Vector, const FVector& ResetVector, float Speed, double Min, double Max, const char* Format, ImGuiSliderFlags Flags)
{
    bool Result = ImGui::DragScalarN(Label, ImGuiDataType_Double, &Vector.X, 3, Speed, &Min, &Max, Format, Flags);

    if (ImGui::BeginPopupContextItem(Label))
    {
        if (ImGui::Button("Reset"))
        {
            Vector = ResetVector;
            Result = true;

            if (ImGuiWindow* Window = GetCurrentWindow())
            {
                const ImGuiID ID = Window->GetID(Label);
                ImGui::MarkItemEdited(ID);
            }
        }

        ImGui::EndPopup();
    }

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiHelper::DragFRotator(const char* Label, FRotator& Rotator, float Speed, double Min, double Max, const char* Format, ImGuiSliderFlags Flags)
{
    return ImGui::DragScalarN(Label, ImGuiDataType_Double, &Rotator.Pitch, 3, Speed, &Min, &Max, Format, Flags);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiHelper::DragFRotator(const char* Label, FRotator& Rotator, const FRotator& ResetRotator, float Speed, double Min, double Max, const char* Format, ImGuiSliderFlags Flags)
{
    bool Result = ImGui::DragScalarN(Label, ImGuiDataType_Double, &Rotator.Pitch, 3, Speed, &Min, &Max, Format, Flags);

    if (ImGui::BeginPopupContextItem(Label))
    {
        if (ImGui::Button("Reset"))
        {
            Rotator = ResetRotator;
            Result = true;

            if (ImGuiWindow* Window = GetCurrentWindow())
            {
                const ImGuiID ID = Window->GetID(Label);
                ImGui::MarkItemEdited(ID);
            }
        }

        ImGui::EndPopup();
    }

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiHelper::DragFVector2D(const char* Label, FVector2D& Vector, float Speed, double Min, double Max, const char* Format, ImGuiSliderFlags Flags)
{
    return ImGui::DragScalarN(Label, ImGuiDataType_Double, &Vector.X, 2, Speed, &Min, &Max, Format, Flags);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiHelper::ColorEdit4(const char* Label, FColor& Color, ImGuiColorEditFlags Flags)
{
    ImColor c = ToImColor(Color);
    const bool Result = ImGui::ColorEdit4(Label, &c.Value.x, Flags);
    Color = ToFColor(c);
    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiHelper::ColorEdit4(const char* Label, FLinearColor& Color, ImGuiColorEditFlags Flags)
{
    return ImGui::ColorEdit4(Label, &Color.R, Flags);
}
