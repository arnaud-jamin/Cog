#include "CogImguiDrawList.h"

#include "CogImguiHelper.h"

//--------------------------------------------------------------------------------------------------------------------------
FCogImguiDrawList::FCogImguiDrawList(ImDrawList* Source)
{
    VtxBuffer.swap(Source->VtxBuffer);
    IdxBuffer.swap(Source->IdxBuffer);
    CmdBuffer.swap(Source->CmdBuffer);
    Flags = Source->Flags;
}

//--------------------------------------------------------------------------------------------------------------------------
FCogImguiDrawData::FCogImguiDrawData(const ImDrawData* Source)
{
    bValid = Source->Valid;

    TotalIdxCount = Source->TotalIdxCount;
    TotalVtxCount = Source->TotalVtxCount;

    DrawLists.SetNumUninitialized(Source->CmdListsCount);
    ConstructItems<FCogImguiDrawList>(DrawLists.GetData(), Source->CmdLists.Data, Source->CmdListsCount);

    DisplayPos = FCogImguiHelper::ToFVector2f(Source->DisplayPos);
    DisplaySize = FCogImguiHelper::ToFVector2f(Source->DisplaySize);
    FrameBufferScale = FCogImguiHelper::ToFVector2f(Source->FramebufferScale);
}
