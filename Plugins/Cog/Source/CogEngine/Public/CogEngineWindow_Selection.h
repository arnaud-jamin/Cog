#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CogWindow.h"
#include "CogEngineWindow_Selection.generated.h"

UCLASS(Config = Cog)
class COGENGINE_API UCogEngineWindow_Selection : public UCogWindow
{
    GENERATED_BODY()

public:

    UCogEngineWindow_Selection();

    bool GetIsSelecting() const { return bSelectionModeActive; }

    const TArray<TSubclassOf<AActor>>& GetActorClasses() const { return ActorClasses; }

    void SetActorClasses(const TArray<TSubclassOf<AActor>>& Value) { ActorClasses = Value; }

    ETraceTypeQuery GetTraceType() const { return TraceType; }

    void SetTraceType(ETraceTypeQuery Value) { TraceType = Value; }

    void TryReapplySelection() const;

protected:

    virtual void ResetConfig() override;

    virtual void PreSaveConfig() override;

    virtual void PostInitProperties() override;

    virtual void RenderHelp() override;

    virtual void RenderTick(float DeltaTime) override;

    virtual void RenderContent() override;

    virtual void RenderMainMenuWidget(bool Draw, float& Width) override;

    bool DrawSelectionCombo();

    void DrawActorContextMenu(AActor* Actor);

    void ActivateSelectionMode();

    void HackWaitInputRelease();

private:

    TSubclassOf<AActor> GetSelectedActorClass() const;

    void TickSelectionMode();

    void ToggleSelectionMode();

    void DeactivateSelectionMode();

    void DrawActorFrame(const AActor* Actor);

    bool ComputeBoundingBoxScreenPosition(const APlayerController* PlayerController, const FVector& Origin, const FVector& Extent, FVector2D& Min, FVector2D& Max);

    UPROPERTY(Config)
    bool bReapplySelection = true;

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
};
