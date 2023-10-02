#pragma once

#include "CoreMinimal.h"
#include "CogImguiWidget.h"
#include "CogImguiKeyInfo.h"
#include "CogImguiTextureManager.h"
#include "imgui.h"
#include "Modules/ModuleManager.h"

class COGIMGUI_API FCogImguiModule : public IModuleInterface
{
public:

    static inline FCogImguiModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FCogImguiModule>("CogImgui");
    }

    //----------------------------------------------------------------------------------------------------------------------
    // IModuleInterface implementation
    //----------------------------------------------------------------------------------------------------------------------
    virtual void StartupModule() override;
	virtual void ShutdownModule() override;

    //----------------------------------------------------------------------------------------------------------------------
    TSharedPtr<SCogImguiWidget> CreateImGuiViewport(UGameViewportClient* GameViewport,  FCogImguiRender Render, ImFontAtlas* FontAtlas = nullptr);

    FCogImguiTextureManager& GetTextureManager() { return TextureManager; }
    ImFontAtlas& GetDefaultFontAtlas() { return DefaultFontAtlas; }

    bool GetEnableInput() const { return bEnabledInput; }
    void SetEnableInput(bool Value) { bEnabledInput = Value; }
    void ToggleEnableInput() { bEnabledInput = !bEnabledInput; }
    
    const FCogImGuiKeyInfo& GetToggleInputKey() const { return ToggleInputKey; }
    void SetToggleInputKey(const FCogImGuiKeyInfo& Value) { ToggleInputKey = Value; }

private:

    void Initialize();

    FCogImguiTextureManager TextureManager;
    ImFontAtlas DefaultFontAtlas;
    bool bEnabledInput = true;
    bool bIsInitialized = false;
    FCogImGuiKeyInfo ToggleInputKey;
};
