#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogEngineDataAsset.h"
#include "GameFramework/Actor.h"
#include "CogWindow.h"
#include "CogEngineWindow_Selection.generated.h"

class IConsoleObject;
class UCogEngineConfig_Selection;
enum class ECogImGuiInputMode : uint8;

//--------------------------------------------------------------------------------------------------------------------------
class COGENGINE_API FCogEngineWindow_Selection : public FCogWindow
{
    typedef FCogWindow Super;

public:

    static FString ToggleSelectionModeCommand;

    virtual void Initialize() override;

    virtual void Shutdown() override;

protected:

    virtual void TryReapplySelection() const;

    virtual void PreSaveConfig() override;

    virtual void RenderHelp() override;

    virtual void RenderTick(float DeltaTime) override;

    virtual void RenderContent() override;

    virtual void RenderMainMenuWidget() override;

    virtual bool DrawSelectionCombo();

    virtual void HackWaitInputRelease();

    virtual void SetGlobalSelection(AActor* Value) const;

    virtual void RenderPickButtonTooltip();

    virtual void RenderActorContextMenu(AActor& Actor);

    virtual const TArray<TSubclassOf<AActor>>& GetSelectionFilters() const;

    virtual ETraceTypeQuery GetSelectionTraceChannel() const;

    TSubclassOf<AActor> GetSelectedActorClass() const;

    bool TickSelectionMode();

    FVector LastSelectedActorLocation = FVector::ZeroVector;

    bool bIsInputEnabledBeforeEnteringSelectionMode = false;

    int32 WaitInputReleased = 0;

    TWeakObjectPtr<UCogEngineConfig_Selection> Config;

    TWeakObjectPtr<const UCogEngineDataAsset> Asset;

    ImGuiTextFilter Filter;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_Selection : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool bReapplySelection = true;

    UPROPERTY(Config)
    FString SelectionName;

    UPROPERTY(Config)
    int32 SelectedClassIndex = 0;

    UPROPERTY(Config)
    FInputChord Shortcut_ToggleSelection = FInputChord(EKeys::F5);
    
    virtual void Reset() override
    {
        Super::Reset();

        bReapplySelection = true;
        SelectionName.Reset();
        SelectedClassIndex = 0;
        Shortcut_ToggleSelection = FInputChord(EKeys::F5);
    }
};