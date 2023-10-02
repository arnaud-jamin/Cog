#include "CogSampleGameState.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "CogDefines.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameState.h"
#include "Modules/ModuleManager.h"

#if USE_COG
#include "CogAbilityDataAsset_Abilities.h"
#include "CogAbilityDataAsset_Cheats.h"
#include "CogAbilityDataAsset_Pools.h"
#include "CogAbilityDataAsset_Tweaks.h"
#include "CogAbilityModule.h"
#include "CogAbilityWindow_Abilities.h"
#include "CogAbilityWindow_Attributes.h"
#include "CogAbilityWindow_Cheats.h"
#include "CogAbilityWindow_Damages.h"
#include "CogAbilityWindow_Effects.h"
#include "CogAbilityWindow_Pools.h"
#include "CogAbilityWindow_Tags.h"
#include "CogAbilityWindow_Tweaks.h"
#include "CogDebugDefines.h"
#include "CogDebugPlot.h"
#include "CogEngineDataAsset_Collisions.h"
#include "CogEngineModule.h"
#include "CogEngineWindow_Collisions.h"
#include "CogEngineWindow_DebugSettings.h"
#include "CogEngineWindow_ImGui.h"
#include "CogEngineWindow_LogCategories.h"
#include "CogEngineWindow_NetEmulation.h"
#include "CogEngineWindow_OutputLog.h"
#include "CogEngineWindow_Plots.h"
#include "CogEngineWindow_Scalability.h"
#include "CogEngineWindow_Selection.h"
#include "CogEngineWindow_Skeleton.h"
#include "CogEngineWindow_Stats.h"
#include "CogEngineWindow_Time.h"
#include "CogImguiModule.h"
#include "CogInputDataAsset_Actions.h"
#include "CogInputWindow_Actions.h"
#include "CogInputWindow_Gamepad.h"
#include "CogSampleTagLibrary.h"
#include "CogWindowManager.h"
#endif //USE_COG


//--------------------------------------------------------------------------------------------------------------------------
template<typename T>
T* GetFirstAssetByClass()
{
    IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();

    TArray<FAssetData> Assets;
    AssetRegistry.GetAssetsByClass(T::StaticClass()->GetClassPathName(), Assets, true);
    if (Assets.Num() == 0)
    {
        return nullptr;
    }

    UObject* Asset = Assets[0].GetAsset();
    T* CastedAsset = Cast<T>(Asset);
    return CastedAsset;
}

//--------------------------------------------------------------------------------------------------------------------------
ACogSampleGameState::ACogSampleGameState(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetActorTickEnabled(true);
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.SetTickFunctionEnable(true);
    PrimaryActorTick.bStartWithTickEnabled = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleGameState::BeginPlay()
{
    Super::BeginPlay();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

#if USE_COG
    if (CogWindowManager != nullptr)
    {
        CogWindowManager->Shutdown();
    }
#endif //USE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleGameState::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

#if USE_COG
    TickCog(DeltaSeconds);
#endif //USE_COG
}

#if USE_COG

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleGameState::TickCog(float DeltaSeconds)
{
    //---------------------------------------
    // Wait for the viewport to be set
    //---------------------------------------
    if (GEngine->GameViewport != nullptr)
    {
        if (CogWindowManager == nullptr)
        {
            InitializeCog();
        }
        else 
        {
            CogWindowManager->Tick(DeltaSeconds);
        }
    }

    extern ENGINE_API float GAverageFPS;
    extern ENGINE_API float GAverageMS;
    FCogDebugPlot::PlotValue(this, "Frame Rate", GAverageFPS);
    FCogDebugPlot::PlotValue(this, "Frame Time", GAverageMS);
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleGameState::InitializeCog()
{
    TSharedPtr<SCogImguiWidget> Widget = FCogImguiModule::Get().CreateImGuiViewport(GEngine->GameViewport, [this](float DeltaTime) { RenderCog(DeltaTime); });
    FCogImguiModule::Get().SetToggleInputKey(FCogImGuiKeyInfo(EKeys::Insert));

    CogWindowManager = NewObject<UCogWindowManager>(this);
    CogWindowManagerRef = CogWindowManager;
    CogWindowManager->Initialize(GetWorld(), Widget);

    //---------------------------------------
    // Engine
    //---------------------------------------
    CogWindowManager->CreateWindow<UCogEngineWindow_DebugSettings>("Engine.Debug Settings");
    CogWindowManager->CreateWindow<UCogEngineWindow_ImGui>("Engine.ImGui");
    CogWindowManager->CreateWindow<UCogEngineWindow_LogCategories>("Engine.Log Categories");
    CogWindowManager->CreateWindow<UCogEngineWindow_NetEmulation>("Engine.Net Emulation");
    CogWindowManager->CreateWindow<UCogEngineWindow_OutputLog>("Engine.Output Log");
    CogWindowManager->CreateWindow<UCogEngineWindow_Plots>("Engine.Plots");
    CogWindowManager->CreateWindow<UCogEngineWindow_Scalability>("Engine.Scalability");
    CogWindowManager->CreateWindow<UCogEngineWindow_Skeleton>("Engine.Skeleton");
    CogWindowManager->CreateWindow<UCogEngineWindow_Time>("Engine.Time");
    UCogEngineWindow_Stats* StatsWindow = CogWindowManager->CreateWindow<UCogEngineWindow_Stats>("Engine.Stats");

    //---------------------------------------
    // Selection
    //---------------------------------------
    UCogEngineWindow_Selection* SelectionWindow = CogWindowManager->CreateWindow<UCogEngineWindow_Selection>("Engine.Selection");
    TArray<TSubclassOf<AActor>> SubClasses
    {
        AActor::StaticClass(),
        AGameModeBase::StaticClass(),
        AGameStateBase::StaticClass(),
        ACharacter::StaticClass()
    };
    SelectionWindow->SetActorSubClasses(SubClasses);
    SelectionWindow->SetCurrentActorSubClass(ACharacter::StaticClass());
    SelectionWindow->SetTraceType(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn));

    //---------------------------------------
    // Collision
    //---------------------------------------
    UCogEngineWindow_Collisions* CollisionsWindow = CogWindowManager->CreateWindow<UCogEngineWindow_Collisions>("Engine.Collision");
    CollisionsWindow->SetCollisionsAsset(GetFirstAssetByClass<UCogEngineDataAsset_Collisions>());

    //---------------------------------------
    // Attributes
    //---------------------------------------
    CogWindowManager->CreateWindow<UCogAbilityWindow_Attributes>("Gameplay.Attributes");

    //---------------------------------------
    // Effects
    //---------------------------------------
    UCogAbilityWindow_Effects* EffectsWindow = CogWindowManager->CreateWindow<UCogAbilityWindow_Effects>("Gameplay.Effects");
    EffectsWindow->NegativeEffectTag = Tag_Effect_Alignment_Negative;
    EffectsWindow->PositiveEffectTag = Tag_Effect_Alignment_Positive;

    //---------------------------------------
    // Abilities
    //---------------------------------------
    UCogAbilityWindow_Abilities* AbilitiesWindow = CogWindowManager->CreateWindow<UCogAbilityWindow_Abilities>("Gameplay.Abilities");
    AbilitiesWindow->AbilitiesAsset = GetFirstAssetByClass<UCogAbilityDataAsset_Abilities>();

    //---------------------------------------
    // Cheats
    //---------------------------------------
    UCogAbilityWindow_Cheats* CheatsWindow = CogWindowManager->CreateWindow<UCogAbilityWindow_Cheats>("Gameplay.Cheats");
    CheatsWindow->CheatsAsset = GetFirstAssetByClass<UCogAbilityDataAsset_Cheats>();
    
    //---------------------------------------
    // Tweaks
    //---------------------------------------
    UCogAbilityWindow_Tweaks* TweaksWindow = CogWindowManager->CreateWindow<UCogAbilityWindow_Tweaks>("Gameplay.Tweaks");
    TweaksWindow->TweaksAsset = GetFirstAssetByClass<UCogAbilityDataAsset_Tweaks>();
    
    //---------------------------------------
    // Damages
    //---------------------------------------
    CogWindowManager->CreateWindow<UCogAbilityWindow_Damages>("Gameplay.Damages");

    //---------------------------------------
    // Tags
    //---------------------------------------
    UCogAbilityWindow_Tags* TagsWindow = CogWindowManager->CreateWindow<UCogAbilityWindow_Tags>("Gameplay.Tags");

    //---------------------------------------
    // Pools
    //---------------------------------------
    UCogAbilityWindow_Pools* PoolsWindow = CogWindowManager->CreateWindow<UCogAbilityWindow_Pools>("Gameplay.Pools");
    PoolsWindow->PoolsAsset = GetFirstAssetByClass<UCogAbilityDataAsset_Pools>();

    //---------------------------------------
    // Input Actions
    //---------------------------------------
    UCogInputWindow_Actions* ActionsWindow = CogWindowManager->CreateWindow<UCogInputWindow_Actions>("Input.Actions");
    ActionsWindow->ActionsAsset = GetFirstAssetByClass<UCogInputDataAsset_Actions>();

    //---------------------------------------
    // Gamepad
    //---------------------------------------
    UCogInputWindow_Gamepad* GamepadWindow = CogWindowManager->CreateWindow<UCogInputWindow_Gamepad>("Input.Gamepad");
    GamepadWindow->ActionsAsset = GetFirstAssetByClass<UCogInputDataAsset_Actions>();

    //---------------------------------------
    // Main Menu Widget
    //---------------------------------------
    CogWindowManager->AddMainMenuWidget(SelectionWindow);
    CogWindowManager->AddMainMenuWidget(StatsWindow);

    CogWindowManager->RebuildMenu();
}

//--------------------------------------------------------------------------------------------------------------------------
void ACogSampleGameState::RenderCog(float DeltaTime)
{
    static bool ShowImGuiDemo = false;
    if (ShowImGuiDemo)
    {
        ImGui::ShowDemoWindow(&ShowImGuiDemo);
    }
    CogWindowManager->Render(DeltaTime);
}

#endif //USE_COG
