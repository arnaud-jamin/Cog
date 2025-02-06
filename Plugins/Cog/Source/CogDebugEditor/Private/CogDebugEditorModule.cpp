#include "CogDebugEditorModule.h"

#include "CogDebugGraphPanelPinFactory.h"
#include "CogDebugLogCategoryDetails.h"
#include "IAssetTools.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "GameplayCoreEditorModule"

//----------------------------------------------------------------------------------------------------------------------
class FCogDebugEditorModule : public ICogDebugEditorModule
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    /** Pin factory for abilities graph; Cached so it can be unregistered */
    TSharedPtr<FCogGraphPanelPinFactory> GraphPanelPinFactory;

    EAssetTypeCategories::Type AssetCategory = EAssetTypeCategories::None;
};

IMPLEMENT_MODULE(FCogDebugEditorModule, CogDebugEditor);

//----------------------------------------------------------------------------------------------------------------------
void FCogDebugEditorModule::StartupModule()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomPropertyTypeLayout("CogLogCategory", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCogLogCategoryDetails::MakeInstance));

    // Register factories for pins and nodes
    GraphPanelPinFactory = MakeShareable(new FCogGraphPanelPinFactory());
    FEdGraphUtilities::RegisterVisualPinFactory(GraphPanelPinFactory);
}

//----------------------------------------------------------------------------------------------------------------------
void FCogDebugEditorModule::ShutdownModule()
{
    if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
        PropertyModule.UnregisterCustomPropertyTypeLayout("CogLogCategory");
    }

    // Unregister graph factories
    if (GraphPanelPinFactory.IsValid())
    {
        FEdGraphUtilities::UnregisterVisualPinFactory(GraphPanelPinFactory);
        GraphPanelPinFactory.Reset();
    }
}

#undef LOCTEXT_NAMESPACE
