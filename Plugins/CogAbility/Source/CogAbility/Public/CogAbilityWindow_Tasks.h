#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAbilityWindow_Tasks.generated.h"

class UAbilitySystemComponent;
class UCogAbilityConfig_Tasks;
class UCogAbilityDataAsset;
class UGameplayTask;

class COGABILITY_API FCogAbilityWindow_Tasks : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void PreBegin(ImGuiWindowFlags& WindowFlags) override;

    virtual void PostBegin() override;

    virtual void RenderContent() override;

    virtual void RenderTaskMenu(AActor* Selection);

    virtual void RenderTasksTable(UAbilitySystemComponent& AbilitySystemComponent);

	virtual void RenderTaskInfo(const UGameplayTask* Task);

    virtual void RenderTaskOwner(const UGameplayTask* Task);

    virtual void RenderTaskState(const UGameplayTask* Task);

    TObjectPtr<UCogAbilityConfig_Tasks> Config = nullptr;

    ImGuiTextFilter Filter;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogAbilityConfig_Tasks : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool SortByName = false;

    UPROPERTY(Config)
    bool ShowTicking = true;

    UPROPERTY(Config)
    bool ShowSimulating = true;

    UPROPERTY(Config)
    bool ShowUninitialized = true;

    UPROPERTY(Config)
    bool ShowAwaitingActivation = true;

    UPROPERTY(Config)
    bool ShowActive = true;

    UPROPERTY(Config)
    bool ShowPaused = true;

    UPROPERTY(Config)
    bool ShowFinished = true;

    UPROPERTY(Config)
    FVector4f UninitializedColor = FVector4f(0.0f, 0.0f, 0.0f, 1.0f);

    UPROPERTY(Config)
    FVector4f AwaitingActivationColor = FVector4f(0.6f, 0.6f, 0.6f, 1.0f);

    UPROPERTY(Config)
    FVector4f ActiveColor = FVector4f(0.0f, 1.0f, 0.5f, 1.0f);

    UPROPERTY(Config)
    FVector4f PausedColor = FVector4f(0.3f, 0.3f, 0.3f, 1.0f);

    UPROPERTY(Config)
    FVector4f FinishedColor = FVector4f(1.0f, 0.5f, 0.5f, 1.0f);


    virtual void Reset() override
    {
        Super::Reset();

        SortByName = false;
        ShowTicking = true;
        ShowActive = true;
        ShowPaused = true;
        ShowFinished = true;
        ActiveColor = FVector4f(0.0f, 1.0f, 0.5f, 1.0f);
        PausedColor = FVector4f(0.3f, 0.3f, 0.3f, 1.0f);
        FinishedColor = FVector4f(0.0f, 1.0f, 0.5f, 1.0f);
    }
};