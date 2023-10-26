#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CogWindow.h"
#include "CogEngineWindow_Selection.generated.h"

class IConsoleObject;

UCLASS(Config = Cog)
class COGENGINE_API UCogEngineWindow_Selection : public UCogWindow
{
    GENERATED_BODY()

public:

    static FString ToggleSelectionModeCommand;

    UCogEngineWindow_Selection();

    bool GetIsSelecting() const { return bSelectionModeActive; }

    const TArray<TSubclassOf<AActor>>& GetActorClasses() const { return ActorClasses; }

    void SetActorClasses(const TArray<TSubclassOf<AActor>>& Value) { ActorClasses = Value; }

    ETraceTypeQuery GetTraceType() const { return TraceType; }

    void SetTraceType(ETraceTypeQuery Value) { TraceType = Value; }

    virtual void ActivateSelectionMode();

    virtual void DeactivateSelectionMode();

    virtual void ToggleSelectionMode();

protected:

    virtual void TryReapplySelection() const;

    virtual void ResetConfig() override;

    virtual void PreSaveConfig() override;

    virtual void PostInitProperties() override;

    virtual void RenderHelp() override;

    virtual void RenderTick(float DeltaTime) override;

    virtual void RenderContent() override;

    virtual float GetMainMenuWidgetWidth(int32 SubWidgetIndex, float MaxWidth) override;

    virtual void RenderMainMenuWidget(int32 SubWidgetIndex, float Width) override;

    virtual bool DrawSelectionCombo();

    virtual void DrawActorContextMenu(AActor* Actor);

    virtual void HackWaitInputRelease();

    virtual void SetGlobalSelection(AActor* Value) const;

    virtual void RenderPickButtonTooltip();

    TSubclassOf<AActor> GetSelectedActorClass() const;

    FString GetActorName(const AActor* Actor) const;

    FString GetActorName(const AActor& Actor) const;

    void TickSelectionMode();

    void DrawActorFrame(const AActor& Actor);

    bool ComputeBoundingBoxScreenPosition(const APlayerController* PlayerController, const FVector& Origin, const FVector& Extent, FVector2D& Min, FVector2D& Max);

    UPROPERTY(Config)
    bool bReapplySelection = true;

    UPROPERTY(Config)
    bool bDisplayActorLabel = true;

    UPROPERTY(Config)
    FString SelectionName;

    UPROPERTY(Config)
    int32 SelectedClassIndex = 0;

    FVector LastSelectedActorLocation = FVector::ZeroVector;

    bool bSelectionModeActive = false;

    bool bImGuiHadInputBeforeEnteringSelectionMode = false;

    int32 WaitInputReleased = 0;

    TArray<TSubclassOf<AActor>> ActorClasses;

    ETraceTypeQuery TraceType = TraceTypeQuery1;

    TArray<IConsoleObject*> ConsoleCommands;
};
