#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "CogSampleConfigurationSubsystem.generated.h"

UCLASS()
class UCogSampleConfigurationSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void PostInitialize() override;

private:
    UPROPERTY()
    TWeakObjectPtr<USubsystem> CogSubsystem;
};
