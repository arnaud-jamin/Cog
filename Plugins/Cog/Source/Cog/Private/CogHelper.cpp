#include "CogHelper.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "CogDebug.h"
#include "GameFramework/PlayerController.h"
#include "imgui.h"

//----------------------------------------------------------------------------------------------------------------------
const UObject* FCogHelper::GetFirstAssetByClass(const TSubclassOf<UObject>& AssetClass)
{
    const IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();

    TArray<FAssetData> Assets;
    AssetRegistry.GetAssetsByClass(AssetClass->GetClassPathName(), Assets, true);
    if (Assets.Num() == 0)
    {
        return nullptr;
    }

    const UObject* Asset = Assets[0].GetAsset();
    return Asset;
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogHelper::GetActorName(const AActor* Actor)
{
	if (Actor == nullptr)
	{
		return FString("none");
	}

	return GetActorName(*Actor);
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogHelper::GetActorName(const AActor& Actor)
{
#if WITH_EDITOR

	const FCogDebugSettings& Settings = FCogDebug::Settings;
	return Settings.ActorNameUseLabel ? Actor.GetActorLabel() : Actor.GetName();

#else //WITH_EDITOR

        return Actor.GetName();

#endif //WITH_EDITOR
}


//-----------------------------------------------------------------------------------------
bool FCogHelper::ComputeBoundingBoxScreenPosition(const APlayerController* PlayerController, const FVector& Origin, const FVector& Extent, FVector2D& Min, FVector2D& Max)
{
    FVector Corners[8];
    Corners[0].Set(-Extent.X, -Extent.Y, -Extent.Z); // - - - 
    Corners[1].Set(Extent.X, -Extent.Y, -Extent.Z);  // + - - 
    Corners[2].Set(-Extent.X, Extent.Y, -Extent.Z);  // - + - 
    Corners[3].Set(-Extent.X, -Extent.Y, Extent.Z);  // - - + 
    Corners[4].Set(Extent.X, Extent.Y, -Extent.Z);   // + + - 
    Corners[5].Set(Extent.X, -Extent.Y, Extent.Z);   // + - + 
    Corners[6].Set(-Extent.X, Extent.Y, Extent.Z);   // - + + 
    Corners[7].Set(Extent.X, Extent.Y, Extent.Z);    // + + + 

    Min.X = FLT_MAX;
    Min.Y = FLT_MAX;
    Max.X = -FLT_MAX;
    Max.Y = -FLT_MAX;

    for (int i = 0; i < 8; ++i)
    {
        FVector2D ScreenLocation;
        if (PlayerController->ProjectWorldLocationToScreen(Origin + Corners[i], ScreenLocation, false) == false)
        {
            return false;
        }

        Min.X = FMath::Min(ScreenLocation.X, Min.X);
        Min.Y = FMath::Min(ScreenLocation.Y, Min.Y);
        Max.X = FMath::Max(ScreenLocation.X, Max.X);
        Max.Y = FMath::Max(ScreenLocation.Y, Max.Y);
    }

    // Prevent getting large values when the camera get close to the target
    ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;
    Min.X = FMath::Max(-DisplaySize.x, Min.X);
    Min.Y = FMath::Max(-DisplaySize.y, Min.Y);
    Max.X = FMath::Min(DisplaySize.x * 2, Max.X);
    Max.Y = FMath::Min(DisplaySize.y * 2, Max.Y);

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogHelper::IsTraceChannelHidden(const UCollisionProfile& InCollisionProfile, const ECollisionChannel InCollisionChannel)
{

	
	return false;
}
