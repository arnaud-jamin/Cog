#pragma once

#include "CoreMinimal.h"
#include "imgui.h"
#include "imgui_internal.h"

class COGDEBUG_API FCogDebugDrawImGui
{
public:
    static void AddLine(const ImVec2& P1, const ImVec2& P2, ImU32 Color, float Thickness = 1.0f, float Duration = 0.0f, bool FadeColor = false);
    static void AddRect(const ImVec2& Min, const ImVec2& Max, ImU32 Color, float Rounding = 0.0f, float Thickness = 1.0f, float Duration = 0.0f, bool FadeColor = false);
    static void AddRectFilled(const ImVec2& Min, const ImVec2& Max, ImU32 Color, float Rounding = 0.0f, float Duration = 0.0f, bool FadeColor = false);
    static void AddQuad(const ImVec2& P1, const ImVec2& P2, const ImVec2& P3, const ImVec2& P4, ImU32 Color, float Thickness = 1.0f, float Duration = 0.0f, bool FadeColor = false);
    static void AddQuadFilled(const ImVec2& P1, const ImVec2& P2, const ImVec2& P3, const ImVec2& P4, ImU32 Color, float Duration = 0.0f, bool FadeColor = false);
    static void AddTriangle(const ImVec2& P1, const ImVec2& P2, const ImVec2& P3, ImU32 Color, float Thickness = 1.0f, float Duration = 0.0f, bool FadeColor = false);
    static void AddTriangleFilled(const ImVec2& P1, const ImVec2& P2, const ImVec2& P3, ImU32 Color, float Duration = 0.0f, bool FadeColor = false);
    static void AddCircle(const ImVec2& Center, float Radius, ImU32 Color, int Segments = 0, float Thickness = 1.0f, float Duration = 0.0f, bool FadeColor = false);
    static void AddCircleFilled(const ImVec2& Center, float Radius, ImU32 Color, int Segments = 0, float Duration = 0.0f, bool FadeColor = false);
    static void AddText(const ImVec2& Pos, const FString& Text, ImU32 Color, bool AddShadow = false, float Duration = 0.0f, bool FadeColor = false);

    static void Draw();

private:
    struct FShape
    {
        ImU32 Color = 0;
        float Duration = 0.0f;
        double Time = 0.0f;
        bool FadeColor = false;
    };

    struct FLine : FShape
    {
        ImVec2 P1 = ImVec2(0, 0);
        ImVec2 P2 = ImVec2(0, 0);
        float Thickness = 0.0f;
    };

    struct FRectangle : FShape
    {
        ImVec2 Min = ImVec2(0, 0);
        ImVec2 Max = ImVec2(0, 0);
        float Rounding = 0.0f;
        float Thickness = 0.0f;
    };

    struct FQuad : FShape
    {
        ImVec2 P1 = ImVec2(0, 0);
        ImVec2 P2 = ImVec2(0, 0);
        ImVec2 P3 = ImVec2(0, 0);
        ImVec2 P4 = ImVec2(0, 0);
        float Thickness = 0.0f;
    };

    struct FTriangle : FShape
    {
        ImVec2 P1 = ImVec2(0, 0);
        ImVec2 P2 = ImVec2(0, 0);
        ImVec2 P3 = ImVec2(0, 0);
        float Thickness = 0.0f;
    };

    struct FCircle : FShape
    {
        ImVec2 Center = ImVec2(0, 0);
        float Radius = 0.0f;
        int Segments = 12;
        float Thickness = 0.0f;
    };

    struct FText : FShape
    {
        ImVec2 Pos = ImVec2(0, 0);
        FString Text;
        ImU32 Color = 0;
    };

    //----------------------------------------------------------------------------------------------------------------------
    static float Time;
    static TArray<FLine> Lines;
    static TArray<FTriangle> Triangles;
    static TArray<FTriangle> TrianglesFilled;
    static TArray<FRectangle> Rectangles;
    static TArray<FRectangle> RectanglesFilled;
    static TArray<FQuad> Quads;
    static TArray<FQuad> QuadsFilled;
    static TArray<FCircle> Circles;
    static TArray<FCircle> CirclesFilled;
    static TArray<FText> Texts;

    //----------------------------------------------------------------------------------------------------------------------
    template<typename TShape, typename TDrawFunction>
    static void DrawShapes(TArray<TShape>& Shapes, TDrawFunction DrawFunction)
    {
        for (int32 i = 0; i < Shapes.Num(); i++)
        {
            const TShape& Shape = Shapes[i];
            const double ElapsedTime = Time - Shape.Time;
            const float Fade = Shape.FadeColor && Shape.Duration > 0.0f ? 1.0f - (ElapsedTime / Shape.Duration) : 1.0f;
            ImColor Color(Shape.Color);
            Color.Value.w = Fade * Color.Value.w;
            DrawFunction(Shape, Color);

            if (ElapsedTime < 0 || ElapsedTime > Shape.Duration)
            {
                Shapes.RemoveAtSwap(i--);
            }
        }
    }
};
