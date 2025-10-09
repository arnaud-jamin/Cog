#pragma once

#include "CogWindow.h"
#include "Perception/AIPerceptionTypes.h"

#include "CogAIWindow_Perception.generated.h"

struct FActorPerceptionBlueprintInfo;
class UCogAIConfig_Perception;

class COGAI_API FCogAIWindow_Perception : public FCogWindow {
  typedef FCogWindow Super;

  TObjectPtr<UAIPerceptionComponent> ResolvePerception(const TObjectPtr<AActor> Selection);
  void RenderStimuliTable(const FActorPerceptionBlueprintInfo &Info);
  const char *SenseNameFromID(const FAISenseID &ID) const;
  ImVec4 ColorFromSenseID(const FAISenseID &ID) const;

  TObjectPtr<UCogAIConfig_Perception> Config = nullptr;
  ImGuiTextFilter Filter;
  bool bDrawStimulusDebug = true;
  bool bDrawStimulusDebugPersist = false;

public:
  virtual void Initialize() override;
  virtual void RenderHelp() override;
  virtual void GameTick(float DeltaTime) override;
  virtual void RenderContent() override;
};

UCLASS(Config = Cog)
class UCogAIConfig_Perception : public UCogCommonConfig {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, Config)
  FLinearColor ActiveColor = FLinearColor(0.20f, 0.85f, 0.35f, 1.f);

  UPROPERTY(EditAnywhere, Config)
  FLinearColor InactiveColor = FLinearColor(0.65f, 0.65f, 0.65f, 1.f);

  virtual void Reset() override {
    Super::Reset();
    ActiveColor = FLinearColor(0.20f, 0.85f, 0.35f, 1.f);
    InactiveColor = FLinearColor(0.65f, 0.65f, 0.65f, 1.f);
  }
};
