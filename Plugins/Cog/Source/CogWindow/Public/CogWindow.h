#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "imgui.h"
#include "Templates/SubclassOf.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/WeakObjectPtrTemplates.h"

class AActor;
class APawn;
class APlayerController;
class UCogWindowManager;
class ULocalPlayer;
class UWorld;

class COGWINDOW_API FCogWindow
{
public:
    
    virtual ~FCogWindow() {}

    virtual void Initialize() {}

    virtual void Shutdown() {}

    virtual void ResetConfig() {}

    virtual void PreSaveConfig() {}

    /** Called every frame with a valid imgui context if the window is visible.  */
    virtual void Render(float DeltaTime);

    /** Called every frame with a valid imgui context even if the window is hidden. */
    virtual void RenderTick(float DeltaTime);

    /** Called every frame without a valid imgui context (outside of the imgui NewFrame/EndFrame) even if the window is hidden. */
    virtual void GameTick(float DeltaTime);

    /**  */
    virtual float GetMainMenuWidgetWidth(int32 SubWidgetIndex, float MaxWidth) { return -1.0f; }

    /**  */
    virtual void RenderMainMenuWidget(int32 SubWidgetIndex, float Width) {}

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

    bool HasWidget() const { return bHasWidget; }

    bool GetIsWidgetVisible() const { return bIsWidgetVisible; }

    void SetIsWidgetVisible(bool Value) { bIsWidgetVisible = Value; }
    
    int32 GetWidgetOrderIndex() const { return WidgetOrderIndex; }

    void SetWidgetOrderIndex(int32 Value) { WidgetOrderIndex = Value; }

    void SetOwner(UCogWindowManager* InOwner) { Owner = InOwner; }

    UCogWindowManager* GetOwner() const { return Owner; }

    template<class T>
    T* GetConfig() { return Cast<T>(GetConfig(T::StaticClass())); }

    UCogCommonConfig* GetConfig(const TSubclassOf<UCogCommonConfig> ConfigClass) const;

    template<class T>
    const T* GetAsset() { return Cast<T>(GetAsset(T::StaticClass())); }

    const UObject* GetAsset(const TSubclassOf<UObject> AssetClass) const;

protected:
    
    friend class UCogWindowManager;

    virtual const FString& GetTitle() const { return Title; }

    virtual UWorld* GetWorld() const;

    virtual void RenderHelp();

    virtual void PreRender(ImGuiWindowFlags& WindowFlags) {}

    virtual void PostRender() {}

    virtual void RenderContent() {}

    virtual bool CheckEditorVisibility();
    
    virtual void OnSelectionChanged(AActor* OldSelection, AActor* NewSelection) {}

    APawn* GetLocalPlayerPawn() const;

    APlayerController* GetLocalPlayerController() const;

    ULocalPlayer* GetLocalPlayer() const;

protected:

    bool bHideMenu = false;

    bool bNoPadding = false;

    bool bHasMenu = false;

    bool bIsVisible = false;

    bool bHasWidget = false;

    bool bIsWidgetVisible = false;

    int32 WidgetOrderIndex = -1;

    ImGuiID ID;

    FString FullName;

    FString Name;

    FString Title;

    UCogWindowManager* Owner = nullptr;

    TWeakObjectPtr<AActor> CurrentSelection;

    TWeakObjectPtr<AActor> OverridenSelection;
};

