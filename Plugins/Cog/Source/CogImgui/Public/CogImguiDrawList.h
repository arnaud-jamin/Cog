#pragma once

#include "CoreMinimal.h"
#include "imgui.h"
#include "Rendering/RenderingCommon.h"

//--------------------------------------------------------------------------------------------------------------------------
struct FCogImguiDrawList
{
	FCogImguiDrawList() {};
	
	FCogImguiDrawList(ImDrawList* Source);

	ImVector<ImDrawVert> VtxBuffer;
	
	ImVector<ImDrawIdx> IdxBuffer;
	
	ImVector<ImDrawCmd> CmdBuffer;

	ImDrawListFlags Flags = ImDrawListFlags_None;
};

//--------------------------------------------------------------------------------------------------------------------------
struct FCogImguiDrawData
{
	FCogImguiDrawData() {};
	
	FCogImguiDrawData(const ImDrawData* Source);

	bool bValid = false;

	int32 TotalIdxCount = 0;
	
	int32 TotalVtxCount = 0;

	TArray<FCogImguiDrawList> DrawLists;

	FVector2f DisplayPos = FVector2f::ZeroVector;

	FVector2f DisplaySize = FVector2f::ZeroVector;

	FVector2f FrameBufferScale = FVector2f::ZeroVector;
};