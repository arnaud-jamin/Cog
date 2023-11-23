#include "CogImguiModule.h"

#include "CogImguiWidget.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "HAL/LowLevelMemTracker.h"
#include "HAL/UnrealMemory.h"
#include "Widgets/Layout/SScaleBox.h"

#define LOCTEXT_NAMESPACE "FCogImguiModule"

//--------------------------------------------------------------------------------------------------------------------------

constexpr int32 Cog_ZOrder = 10000;

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
void FCogImguiModule::Initialize()
{
    TextureManager.InitializeErrorTexture();
    TextureManager.CreatePlainTexture("ImGuiModule_Plain", 2, 2, FColor::White);

    unsigned char* Pixels;
    int Width, Height, Bpp;

    DefaultFontAtlas.Clear();
    DefaultFontAtlas.GetTexDataAsRGBA32(&Pixels, &Width, &Height, &Bpp);
    const CogTextureIndex FontsTexureIndex = TextureManager.CreateTexture("ImGuiModule_FontAtlas", Width, Height, Bpp, Pixels);
    DefaultFontAtlas.TexID = FCogImguiHelper::ToImTextureID(FontsTexureIndex);
}

//--------------------------------------------------------------------------------------------------------------------------
TSharedPtr<SCogImguiWidget> FCogImguiModule::CreateImGuiWidget(UGameViewportClient* GameViewport, FCogImguiRenderFunction Render, ImFontAtlas* FontAtlas /*= nullptr*/)
{
    if (bIsInitialized == false)
    {
        Initialize();
        bIsInitialized = true;
    }

    if (FontAtlas == nullptr)
    {
        FontAtlas = &DefaultFontAtlas;
    }

    TSharedPtr<SCogImguiWidget> ImguiWidget;
    SAssignNew(ImguiWidget, SCogImguiWidget)
        .GameViewport(GameViewport)
        .FontAtlas(FontAtlas)
        .Render(Render)
        .Clipping(EWidgetClipping::ClipToBounds);

    TSharedPtr<SScaleBox> ScaleWidget;
    SAssignNew(ScaleWidget, SScaleBox)
        .IgnoreInheritedScale(true)
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        .Visibility(EVisibility::SelfHitTestInvisible)
        [
            ImguiWidget.ToSharedRef()
        ];

    GameViewport->AddViewportWidgetContent(ScaleWidget.ToSharedRef(), Cog_ZOrder);

    return ImguiWidget;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiModule::DestroyImGuiWidget(TSharedPtr<SCogImguiWidget> ImGuiWidget)
{
    UGameViewportClient* Viewport = ImGuiWidget->GetGameViewport().Get();
    if (Viewport == nullptr)
    {
        return;
    }

    TSharedPtr<SWidget> ParentWidget = ImGuiWidget->GetParentWidget();
    if (ParentWidget.IsValid() == false)
    {
        return;
    }

    Viewport->RemoveViewportWidgetContent(ParentWidget.ToSharedRef());
}

//--------------------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCogImguiModule, CogImGui)