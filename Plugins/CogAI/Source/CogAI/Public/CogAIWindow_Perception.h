#pragma once

#include "CogWindow.h"
#include "Perception/AIPerceptionTypes.h"

#include "CogAIWindow_Perception.generated.h"

struct FActorPerceptionBlueprintInfo;
class UCogAIConfig_Perception;

class COGAI_API FCogAIWindow_Perception : public FCogWindow 
{
    typedef FCogWindow Super;

public:
    virtual void Initialize() override;
    virtual void RenderHelp() override;
    virtual void GameTick(float DeltaTime) override;
    virtual void PreBegin(ImGuiWindowFlags& WindowFlags);
    virtual void PostBegin();
    virtual void RenderContent() override;

private:
    TObjectPtr<UAIPerceptionComponent> ResolvePerception(const TObjectPtr<AActor> Selection);
    const char* SenseNameFromID(const FAISenseID& ID) const;
    ImVec4 ColorFromSenseID(const FAISenseID& ID) const;
    void DrawSenseTooltip(const FActorPerceptionBlueprintInfo& Info, const FAIStimulus& Stimulus);

    TObjectPtr<UCogAIConfig_Perception> Config = nullptr;
    ImGuiTextFilter Filter;
    bool bDrawStimulusDebug = true;
    bool bDrawStimulusDebugPersist = false;
};

UCLASS(Config = Cog)
class UCogAIConfig_Perception : public UCogCommonConfig 
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, Config)
    FLinearColor ActiveColor = FLinearColor(0.20f, 0.85f, 0.35f, 1.f);

    UPROPERTY(EditAnywhere, Config)
    FLinearColor InactiveColor = FLinearColor(0.65f, 0.65f, 0.65f, 1.f);

    UPROPERTY(EditAnywhere, Config)
    FLinearColor SightSenseColor = FLinearColor(0.90f, 0.75f, 0.10f, 1.f);

    UPROPERTY(EditAnywhere, Config)
    FLinearColor HearingSenseColor = FLinearColor(0.20f, 0.60f, 1.00f, 1.f);

    UPROPERTY(EditAnywhere, Config)
    FLinearColor DamageSenseColor = FLinearColor(0.95f, 0.20f, 0.20f, 1.f);

    UPROPERTY(EditAnywhere, Config)
    FLinearColor TouchSenseColor = FLinearColor(0.20f, 0.90f, 0.45f, 1.f);

    UPROPERTY(EditAnywhere, Config)
    FLinearColor TeamSenseColor = FLinearColor(0.75f, 0.20f, 0.90f, 1.f);

    UPROPERTY(EditAnywhere, Config)
    FLinearColor PredictionSenseColor = FLinearColor(0.95f, 0.50f, 0.20f, 1.f);

    UPROPERTY(EditAnywhere, Config)
    FLinearColor DefaultSenseColor = FLinearColor(0.7f, 0.7f, 0.7f, 1.f);

    virtual void Reset() override 
    {
        Super::Reset();
        ActiveColor = FLinearColor(0.20f, 0.85f, 0.35f, 1.f);
        InactiveColor = FLinearColor(0.65f, 0.65f, 0.65f, 1.f);
    }
};
