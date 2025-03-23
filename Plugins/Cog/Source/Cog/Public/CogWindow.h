#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "imgui.h"
#include "Templates/SubclassOf.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/WeakObjectPtrTemplates.h"

struct FCogDebugContext;
class AActor;
class APawn;
class APlayerController;
class UCogSubsystem;
class ULocalPlayer;
class UWorld;

class COG_API FCogWindow
{
public:
    
    virtual ~FCogWindow() {}

    virtual void Initialize();

    virtual void Shutdown();

    virtual void ResetConfig();

    virtual void PreSaveConfig() {}

    /** Called every frame with a valid imgui context if the window is visible.  */
    virtual void Render(float DeltaTime);

    /** Called every frame with a valid imgui context even if the window is hidden. */
    virtual void RenderTick(float DeltaTime);

    /** Called every frame without a valid imgui context (outside the imgui NewFrame/EndFrame) even if the window is hidden. */
    virtual void GameTick(float DeltaTime);

    /**  */
    virtual void RenderMainMenuWidget();

    virtual void RenderSettings();

    virtual void BindInputs(UInputComponent* InputComponent) {}
    
    ImGuiID GetID() const { return ID; }

    /** The full name of the window, that contains the path in the main menu. For example "Gameplay.Character.Effect" */
    const FString& GetFullName() const { return FullName; }

    void SetFullName(const FString& InFullName);

    /** The short name of the window. "Effect" if the window full name is "Gameplay.Character.Effect" */
    const FString& GetName() const { return Name; }

    AActor* GetSelection() const;

    void SetSelection(AActor* Actor);

    bool GetIsVisible() const { return bIsVisible; }

    void SetIsVisible(bool Value);

    bool HasWidget() const { return bHasWidget; }

    bool GetIsWidgetVisible() const { return bIsWidgetVisible; }

    void SetIsWidgetVisible(bool Value) { bIsWidgetVisible = Value; }
    
    int32 GetWidgetOrderIndex() const { return WidgetOrderIndex; }

    void SetWidgetOrderIndex(int32 Value) { WidgetOrderIndex = Value; }

    void SetOwner(UCogSubsystem* InOwner) { Owner = InOwner; }

    UCogSubsystem* GetOwner() const { return Owner; }

    float GetDpiScale() const;
    

    template<class T>
    T* GetConfig(bool InResetConfigOnRequest = true) const { return Cast<T>(GetConfig(T::StaticClass(), InResetConfigOnRequest)); }

    UCogCommonConfig* GetConfig(const TSubclassOf<UCogCommonConfig>& InConfigClass, bool InResetConfigOnRequest = true) const;

    template<class T>
    const T* GetAsset() const { return Cast<T>(GetAsset(T::StaticClass())); }

    const UObject* GetAsset(const TSubclassOf<UObject>& AssetClass) const;


protected:
    
    friend class UCogSubsystem;

    virtual const FString& GetTitle() const { return Title; }

    virtual UWorld* GetWorld() const;

    virtual void RenderHelp();

    virtual void PreBegin(ImGuiWindowFlags& WindowFlags) {}

    virtual void PostBegin() {}

    virtual void PostEnd() {}

    virtual void RenderContent() {}

    virtual bool CheckEditorVisibility();
    
    virtual void RenderContextMenu();

    virtual void OnWindowVisibilityChanged(bool NewVisibility) { }

    virtual void OnSelectionChanged(AActor* OldSelection, AActor* NewSelection) {}

    virtual bool IsWindowRenderedInMainMenu();

    virtual void RenderConfigShortcuts(UCogCommonConfig& InConfig) const;

    APawn* GetLocalPlayerPawn() const;

    APlayerController* GetLocalPlayerController() const;

    ULocalPlayer* GetLocalPlayer() const;

    bool bIsInitialized = false;
    
    bool bShowMenu = true;

    bool bHasMenu = false;

    bool bIsVisible = false;

    bool bHasWidget = false;

    bool bIsWidgetVisible = false;

    bool bShowInMainMenu = true;

    bool bUseCustomContextMenu = false;

    int32 WidgetOrderIndex = -1;

    ImGuiID ID = 0;

    FString FullName;

    FString Name;

    FString Title;
    
    UCogSubsystem* Owner = nullptr;

    mutable TArray<TWeakObjectPtr<UCogCommonConfig>> ConfigsToResetOnRequest;
};

