#include "CogEngineWindow_Levels.h"

#include "CogImguiHelper.h"
#include "CogWidgets.h"
#include "AssetRegistry/AssetRegistryModule.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Levels::RenderHelp()
{
    ImGui::Text("This window can be used to load levels.");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Levels::Initialize()
{
    Super::Initialize();

    bHasMenu = true;
    Config = GetConfig<UCogEngineWindowConfig_LevelLoader>();
}

//--------------------------------------------------------------------------------------------------------------------------
 void FCogEngineWindow_Levels::GetAllLevels(TArray<FAssetData>& OutLevels)
{
    const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    const IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    FARFilter AssetFilter;
    AssetFilter.ClassPaths.Add(UWorld::StaticClass()->GetClassPathName());

    AssetRegistry.GetAssets(AssetFilter, OutLevels);

    RefreshSorting();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Levels::RefreshSorting()
{
    Levels = UnsortedLevels;

    if (Config->SortByName)
    {
        if (Config->ShowPath)
        {
            Levels.Sort([](const auto& InA, const auto& InB) { return InA.PackagePath.Compare(InB.PackagePath) < 0; });
        }
        else
        {
            Levels.Sort([](const auto& InA, const auto& InB) { return InA.AssetName.Compare(InB.AssetName) < 0; });
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Levels::RenderMenu()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::Checkbox("Sort by Name", &Config->SortByName))
            {
                RefreshSorting();
            }
            
            if (ImGui::Checkbox("Show Path", &Config->ShowPath))
            {
                RefreshSorting();
            }

            ImGui::EndMenu();
        }
        
        FCogWidgets::SearchBar("##Filter", Filter);

        ImGui::EndMenuBar();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Levels::RenderContent()
{
    Super::RenderContent();

    if (HasGatheredLevels == false)
    {
        GetAllLevels(UnsortedLevels);
        RefreshSorting();
        HasGatheredLevels = true;
    }
    
    RenderMenu();

    const float FooterHeight = ImGui::GetFrameHeightWithSpacing();
    const ImVec2 Size = IsWindowRenderedInMainMenu()
        ? ImVec2(0.f, ImGui::GetFontSize() * 10 - FooterHeight)
        : ImVec2(0.f, ImGui::GetContentRegionAvail().y - FooterHeight);
    
    const bool Visible = ImGui::BeginChild("Levels", Size, 0, ImGuiWindowFlags_HorizontalScrollbar);
    
    if (Visible)
    {
        for (int32 i = 0; i < Levels.Num(); i++)
        {
            ImGui::PushID(i);

            const FAssetData& Asset = Levels[i];
            RenderLevel(i, Asset);

            ImGui::PopID();
        }
    }
    ImGui::EndChild();

    const bool CanLoad = Levels.IsValidIndex(SelectedIndex);
    if (CanLoad == false)
    {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Load", ImVec2(-1, 0)))
    {
        if (Levels.IsValidIndex(SelectedIndex))
        {
            LoadLevel(Levels[SelectedIndex]);
        }
    }
    if (CanLoad == false)
    {
        ImGui::EndDisabled();
    }
}


//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Levels::RenderLevel(int32 InIndex, const FAssetData& InAsset)
{
    const FString Label = Config->ShowPath ? InAsset.PackageName.ToString() : InAsset.AssetName.ToString();

    const auto LabelStr = StringCast<ANSICHAR>(*Label);
    if (Filter.PassFilter(LabelStr.Get()) == false)
    { return; }
    
    if (ImGui::Selectable(LabelStr.Get(), SelectedIndex == InIndex, ImGuiSelectableFlags_AllowDoubleClick))
    {
        SelectedIndex = InIndex;
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            LoadLevel(InAsset);
        }
    }

    ImGui::SetItemTooltip(StringCast<ANSICHAR>(*InAsset.PackageName.ToString()).Get());

    RenderLevelContextMenu(InIndex, InAsset);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Levels::LoadLevel(const FAssetData& InAsset)
{
    APlayerController* LocalPlayerController = GetLocalPlayerController();
    if (LocalPlayerController == nullptr)
    { return; }
    
    LocalPlayerController->ConsoleCommand(FString::Printf(TEXT("Travel %s"), *InAsset.PackageName.ToString()));
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Levels::RenderLevelContextMenu(int Index, const FAssetData& Asset)
{
    if (ImGui::BeginPopupContextItem())
    {
        FCogWidgets::BrowseToAssetButton(Asset);
        ImGui::EndPopup();
    }
}
