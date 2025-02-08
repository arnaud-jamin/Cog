#include "CogDebugDrawImGui.h"

#include "imgui_internal.h"

//--------------------------------------------------------------------------------------------------------------------------
float FCogDebugDrawImGui::Time = 0;
TArray<FCogDebugDrawImGui::FLine>       FCogDebugDrawImGui::Lines;
TArray<FCogDebugDrawImGui::FTriangle>   FCogDebugDrawImGui::Triangles;
TArray<FCogDebugDrawImGui::FTriangle>   FCogDebugDrawImGui::TrianglesFilled;
TArray<FCogDebugDrawImGui::FRectangle>  FCogDebugDrawImGui::Rectangles;
TArray<FCogDebugDrawImGui::FRectangle>  FCogDebugDrawImGui::RectanglesFilled;
TArray<FCogDebugDrawImGui::FQuad>       FCogDebugDrawImGui::Quads;
TArray<FCogDebugDrawImGui::FQuad>       FCogDebugDrawImGui::QuadsFilled;
TArray<FCogDebugDrawImGui::FCircle>     FCogDebugDrawImGui::Circles;
TArray<FCogDebugDrawImGui::FCircle>     FCogDebugDrawImGui::CirclesFilled;
TArray<FCogDebugDrawImGui::FText>       FCogDebugDrawImGui::Texts;

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawImGui::AddLine(const ImVec2& P1, const ImVec2& P2, ImU32 Color, float Thickness /*= 1.0f*/, float Duration /*= 0.0f*/, bool FadeColor /*= false*/)
{
    FLine Line;
    Line.P1 = P1;
    Line.P2 = P2;
    Line.Color = Color;
    Line.Thickness = Thickness;
    Line.Time = Time;
    Line.Duration = Duration;
    Line.FadeColor = FadeColor;
    Lines.Add_GetRef(Line);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawImGui::AddRect(const ImVec2& Min, const ImVec2& Max, ImU32 Color, float Rounding /*= 0.0f*/, float Thickness /*= 1.0f*/, float Duration /*= 0.0f*/, bool FadeColor /*= false*/)
{
    FRectangle Rectangle;
    Rectangle.Min = Min;
    Rectangle.Max = Max;
    Rectangle.Color = Color;
    Rectangle.Rounding = Rounding;
    Rectangle.Thickness = Thickness;
    Rectangle.Time = Time;
    Rectangle.Duration = Duration;
    Rectangle.FadeColor = FadeColor;
    Rectangles.Add_GetRef(Rectangle);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawImGui::AddRectFilled(const ImVec2& Min, const ImVec2& Max, ImU32 Color, float Rounding /*= 0.0f*/, float Duration /*= 0.0f*/, bool FadeColor /*= false*/)
{
    FRectangle Rectangle;
    Rectangle.Min = Min;
    Rectangle.Max = Max;
    Rectangle.Color = Color;
    Rectangle.Rounding = Rounding;
    Rectangle.Thickness = 0.0f;
    Rectangle.Time = Time;
    Rectangle.Duration = Duration;
    Rectangle.FadeColor = FadeColor;
    RectanglesFilled.Add_GetRef(Rectangle);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawImGui::AddQuad(const ImVec2& P1, const ImVec2& P2, const ImVec2& P3, const ImVec2& P4, ImU32 Color, float Thickness/* = 1.0f*/, float Duration /*= 0.0f*/, bool FadeColor /*= false*/)
{
    FQuad Quad;
    Quad.P1 = P1;
    Quad.P2 = P2;
    Quad.P3 = P3;
    Quad.P4 = P4;
    Quad.Color = Color;
    Quad.Thickness = Thickness;
    Quad.Time = Time;
    Quad.Duration = Duration;
    Quad.FadeColor = FadeColor;
    Quads.Add_GetRef(Quad);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawImGui::AddQuadFilled(const ImVec2& P1, const ImVec2& P2, const ImVec2& P3, const ImVec2& P4, ImU32 Color, float Duration /*= 0.0f*/, bool FadeColor /*= false*/)
{
    FQuad Quad;
    Quad.P1 = P1;
    Quad.P2 = P2;
    Quad.P3 = P3;
    Quad.P4 = P4;
    Quad.Color = Color;
    Quad.Thickness = 0.0f;
    Quad.Time = Time;
    Quad.Duration = Duration;
    Quad.FadeColor = FadeColor;
    QuadsFilled.Add_GetRef(Quad);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawImGui::AddTriangle(const ImVec2& P1, const ImVec2& P2, const ImVec2& P3, ImU32 Color, float Thickness/* = 1.0f*/, float Duration /*= 0.0f*/, bool FadeColor /*= false*/)
{
    FTriangle Triangle;
    Triangle.P1 = P1;
    Triangle.P2 = P2;
    Triangle.P3 = P3;
    Triangle.Color = Color;
    Triangle.Thickness = Thickness;
    Triangle.Time = Time;
    Triangle.Duration = Duration;
    Triangle.FadeColor = FadeColor;
    Triangles.Add_GetRef(Triangle);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawImGui::AddTriangleFilled(const ImVec2& P1, const ImVec2& P2, const ImVec2& P3, ImU32 Color, float Duration /*= 0.0f*/, bool FadeColor /*= false*/)
{
    FTriangle Triangle;
    Triangle.P1 = P1;
    Triangle.P2 = P2;
    Triangle.P3 = P3;
    Triangle.Color = Color;
    Triangle.Thickness = 0.0f;
    Triangle.Time = Time;
    Triangle.Duration = Duration;
    Triangle.FadeColor = FadeColor;
    TrianglesFilled.Add_GetRef(Triangle);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawImGui::AddCircle(const ImVec2& Center, float Radius, ImU32 Color, int Segments /*= 0*/, float Thickness /*= 1.0f*/, float Duration /*= 0.0f*/, bool FadeColor /*= false*/)
{
    
    FCircle Circle;
    Circle.Center = Center;
    Circle.Radius = Radius > 0.0f ? Radius : 1.0f;
    Circle.Color = Color;
    Circle.Segments = Segments;
    Circle.Thickness = Thickness;
    Circle.Time = Time;
    Circle.Duration = Duration;
    Circle.FadeColor = FadeColor;
    Circles.Add_GetRef(Circle);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawImGui::AddCircleFilled(const ImVec2& Center, float Radius, ImU32 Color, int Segments /*= 0*/, float Duration /*= 0.0f*/, bool FadeColor /*= false*/)
{
    FCircle Circle;
    Circle.Center = Center;
    Circle.Radius = Radius > 0.0f ? Radius : 1.0f;
    Circle.Color = Color;
    Circle.Segments = Segments;
    Circle.Thickness = 0.0f;
    Circle.Time = Time;
    Circle.Duration = Duration;
    Circle.FadeColor = FadeColor;
    CirclesFilled.Add_GetRef(Circle);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawImGui::AddText(const ImVec2& Pos, const FString& Text, ImU32 Color, bool AddShadow /*= false*/, float Duration/* = 0.0f*/, bool FadeColor /*= false*/)
{
    FText TextElement;
    TextElement.Pos = Pos;
    TextElement.Text = Text;
    TextElement.Color = Color;
    TextElement.Time = Time;
    TextElement.Duration = Duration;
    TextElement.FadeColor = FadeColor;
    Texts.Add_GetRef(TextElement);

    if (AddShadow)
    {
        FText ShadowTextElement;
        ShadowTextElement.Pos = Pos + ImVec2(1, 1);
        ShadowTextElement.Text = Text;
        const float Alpha = ImGui::ColorConvertU32ToFloat4(Color).w; // Keep original Alpha and set to black
        ShadowTextElement.Color = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, Alpha));
        ShadowTextElement.Time = Time;
        ShadowTextElement.Duration = Duration;
        ShadowTextElement.FadeColor = FadeColor;
        Texts.Add_GetRef(ShadowTextElement);
    }
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogDebugDrawImGui::Draw()
{
    ImDrawList* DrawList = ImGui::GetBackgroundDrawList();
    Time = ImGui::GetCurrentContext()->Time;

    DrawShapes(Lines, [DrawList](const FLine& Line, const ImColor Color) { DrawList->AddLine(Line.P1, Line.P2, Color, Line.Thickness); });
    DrawShapes(Rectangles, [DrawList](const FRectangle& Rectangle, const ImColor Color) { DrawList->AddRect(Rectangle.Min, Rectangle.Max, Color, Rectangle.Rounding, Rectangle.Thickness); });
    DrawShapes(RectanglesFilled, [DrawList](const FRectangle& Rectangle, const ImColor Color) { DrawList->AddRectFilled(Rectangle.Min, Rectangle.Max, Color, Rectangle.Rounding); });
    DrawShapes(Quads, [DrawList](const FQuad& Quad, const ImColor Color) { DrawList->AddQuad(Quad.P1, Quad.P2, Quad.P3, Quad.P4, Color, Quad.Thickness); });
    DrawShapes(QuadsFilled, [DrawList](const FQuad& Quad, const ImColor Color) { DrawList->AddQuadFilled(Quad.P1, Quad.P2, Quad.P3, Quad.P4, Color); });
    DrawShapes(Triangles, [DrawList](const FTriangle& Triangle, const ImColor Color) { DrawList->AddTriangle(Triangle.P1, Triangle.P2, Triangle.P3, Color, Triangle.Thickness); });
    DrawShapes(TrianglesFilled, [DrawList](const FTriangle& Triangle, const ImColor Color) { DrawList->AddTriangleFilled(Triangle.P1, Triangle.P2, Triangle.P3, Color); });
    DrawShapes(Circles, [DrawList](const FCircle& Circle, const ImColor Color) { DrawList->AddCircle(Circle.Center, Circle.Radius, Color, Circle.Segments, Circle.Thickness); });
    DrawShapes(CirclesFilled, [DrawList](const FCircle& Circle, const ImColor Color) { DrawList->AddCircleFilled(Circle.Center, Circle.Radius, Color, Circle.Segments); });
    DrawShapes(Texts, [DrawList](const FText& Text, const ImColor Color) { DrawList->AddText(Text.Pos, Color, TCHAR_TO_ANSI(*Text.Text)); });
}
