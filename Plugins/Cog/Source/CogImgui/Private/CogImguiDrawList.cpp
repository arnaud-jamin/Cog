#include "CogImguiDrawList.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiDrawList::CopyVertexData(TArray<FSlateVertex>& OutVertexBuffer, const FTransform2D& Transform) const
{
    // Reset and reserve space in destination buffer.
    OutVertexBuffer.SetNumUninitialized(ImGuiVertexBuffer.Size, false);

    // Transform and copy vertex data.
    for (int Idx = 0; Idx < ImGuiVertexBuffer.Size; Idx++)
    {
        const ImDrawVert& ImGuiVertex = ImGuiVertexBuffer[Idx];
        FSlateVertex& SlateVertex = OutVertexBuffer[Idx];

        // Final UV is calculated in shader as XY * ZW, so we need set all components.
        SlateVertex.TexCoords[0] = ImGuiVertex.uv.x;
        SlateVertex.TexCoords[1] = ImGuiVertex.uv.y;
        SlateVertex.TexCoords[2] = SlateVertex.TexCoords[3] = 1.f;

        const FVector2D VertexPosition = Transform.TransformPoint(FCogImguiHelper::ToVector2D(ImGuiVertex.pos));
        SlateVertex.Position[0] = VertexPosition.X;
        SlateVertex.Position[1] = VertexPosition.Y;

        // Unpack ImU32 color.
        SlateVertex.Color = FCogImguiHelper::UnpackImU32Color(ImGuiVertex.col);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FCogImguiDrawList::DrawCommand FCogImguiDrawList::GetCommand(int CommandCount, const FTransform2D& Transform) const
{
    const ImDrawCmd& ImGuiCommand = ImGuiCommandBuffer[CommandCount];
    return
    {
        ImGuiCommand.ElemCount,
        TransformRect(Transform, FCogImguiHelper::ToSlateRect(ImGuiCommand.ClipRect)),
        FCogImguiHelper::ToTextureIndex(ImGuiCommand.TextureId)
    };
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiDrawList::CopyIndexData(TArray<SlateIndex>& OutIndexBuffer, const int32 StartIndex, const int32 NumElements) const
{
    // Reset buffer.
    OutIndexBuffer.SetNumUninitialized(NumElements, false);

    // Copy elements (slow copy because of different sizes of ImDrawIdx and SlateIndex and because SlateIndex can
    // have different size on different platforms).
    for (int i = 0; i < NumElements; i++)
    {
        OutIndexBuffer[i] = ImGuiIndexBuffer[StartIndex + i];
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiDrawList::TransferDrawData(ImDrawList& Src)
{
    // Move data from source to this list.
    Src.CmdBuffer.swap(ImGuiCommandBuffer);
    Src.IdxBuffer.swap(ImGuiIndexBuffer);
    Src.VtxBuffer.swap(ImGuiVertexBuffer);

    // ImGui seems to clear draw lists in every frame, but since source list can contain pointers to buffers that
    // we just swapped, it is better to clear explicitly here.
    Src._ResetForNewFrame();
}
