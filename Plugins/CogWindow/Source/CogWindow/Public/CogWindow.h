#pragma once

#include "CoreMinimal.h"
#include "imgui.h"
#include "CogWindow.generated.h"

class UCogWindowManager;
class APawn;
class APlayerController;

UCLASS(Config = Cog)
class COGWINDOW_API UCogWindow : public UObject
{
    GENERATED_BODY()

public:
    
    virtual void Initialize() {}

    virtual void PreSaveConfig() {}

    /** Called every frame with a valid imgui context if the window is visible.  */
    virtual void Render(float DeltaTime);

    /** Called every frame with a valid imgui context even if the window is hidden. */
    virtual void RenderTick(float DeltaTime);

    /** Called every frame without a valid imgui context (outside of the imgui NewFrame/EndFrame) even if the window is hidden. */
    virtual void GameTick(float DeltaTime);

    /** Draw the window menu item. Called when the menu item visible */
    virtual void DrawMenuItem(const FString& MenuItemName);

    virtual void DrawMainMenuWidget(bool Draw, float& Width) {}

    ImGuiID GetID() const { return ID; }

    /** The full name of the window, that contains the path in the main menu. For example "Gameplay.Character.Effect" */
    const FString& GetFullName() const { return FullName; }

    void SetFullName(const FString& InFullName);

    /** The short name of the window. "Effect" if the window full name is "Gameplay.Character.Effect" */
    const FString& GetName() const { return Name; }

    AActor* GetSelection() { return CurrentSelection.Get(); }

    void SetSelection(AActor* Actor);

    bool GetIsVisible() const { return bIsVisible; }

    void SetIsVisible(bool Value) { bIsVisible = Value; }

    void SetOwner(UCogWindowManager* InOwner) { Owner = InOwner; }

    UCogWindowManager* GetOwner() const { return Owner; }

protected:

    virtual const FString& GetTitle() const { return Name; }

    virtual void RenderHelp();

    virtual void PreRender(ImGuiWindowFlags& WindowFlags) {}

    virtual void PostRender() {}

    virtual void RenderContent() {}

    virtual bool CheckEditorVisibility();
    
    virtual void OnSelectionChanged(AActor* OldSelection, AActor* NewSelection) {}

    APawn* GetLocalPlayerPawn();

    APlayerController* GetLocalPlayerController();

    ULocalPlayer* GetLocalPlayer();


private:
    bool bIsVisible = false;

    bool bShowInsideMenu = true;

    ImGuiID ID;

    FString FullName;

    FString Name;

    UCogWindowManager* Owner = nullptr;

    TWeakObjectPtr<AActor> CurrentSelection;

    TWeakObjectPtr<AActor> OverridenSelection;
};
