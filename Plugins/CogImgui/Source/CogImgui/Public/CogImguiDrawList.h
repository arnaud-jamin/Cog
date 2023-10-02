#pragma once

#include "CoreMinimal.h"
#include "CogImguiHelper.h"
#include "imgui.h"
#include "Rendering/RenderingCommon.h"

//--------------------------------------------------------------------------------------------------------------------------
class FCogImguiDrawList
{
public:

    struct DrawCommand
    {
        uint32 NumElements;
        FSlateRect ClippingRect;
        CogTextureIndex TextureId;
    };

    // Get the number of draw commands in this list.
    FORCEINLINE int NumCommands() const { return ImGuiCommandBuffer.Size; }

    // Get the draw command by number.
    // @param CommandNb - Number of draw command
    // @param Transform - Transform to apply to clipping rectangle
    // @returns Draw command data
    DrawCommand GetCommand(int CommandCount, const FTransform2D& Transform) const;

    // Transform and copy vertex data to target buffer (old data in the target buffer are replaced).
    // @param OutVertexBuffer - Destination buffer
    // @param Transform - Transform to apply to all vertices
    void CopyVertexData(TArray<FSlateVertex>& OutVertexBuffer, const FTransform2D& Transform) const;

    // Transform and copy index data to target buffer (old data in the target buffer are replaced).
    // Internal index buffer contains enough data to match the sum of NumElements from all draw commands.
    // @param OutIndexBuffer - Destination buffer
    // @param StartIndex - Start copying source data starting from this index
    // @param NumElements - How many elements we want to copy
    void CopyIndexData(TArray<SlateIndex>& OutIndexBuffer, const int32 StartIndex, const int32 NumElements) const;

    // Transfers data from ImGui source list to this object. Leaves source cleared.
    void TransferDrawData(ImDrawList& Src);

private:

    ImVector<ImDrawCmd> ImGuiCommandBuffer;
    ImVector<ImDrawIdx> ImGuiIndexBuffer;
    ImVector<ImDrawVert> ImGuiVertexBuffer;
};
