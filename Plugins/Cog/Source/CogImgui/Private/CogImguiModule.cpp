#include "CogImguiModule.h"

#include "imgui.h"
#include "HAL/LowLevelMemTracker.h"
#include "HAL/UnrealMemory.h"

#define LOCTEXT_NAMESPACE "FCogImguiModule"

//--------------------------------------------------------------------------------------------------------------------------
static void* ImGui_MemAlloc(size_t Size, void* UserData)
{
    LLM_SCOPE_BYNAME(TEXT("ImGui"));
    return FMemory::Malloc(Size);
}

//--------------------------------------------------------------------------------------------------------------------------
static void ImGui_MemFree(void* Ptr, void* UserData)
{
    FMemory::Free(Ptr);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiModule::StartupModule()
{
    ImGui::SetAllocatorFunctions(ImGui_MemAlloc, ImGui_MemFree);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiModule::ShutdownModule()
{
}

//--------------------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCogImguiModule, CogImgui)