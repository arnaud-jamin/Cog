#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogEngineWindow_CommandBindings.generated.h"

class UPlayerInput;
struct FKeyBind;

UCLASS(Config = Cog)
class COGENGINE_API UCogEngineWindow_CommandBindings : public UCogWindow
{
    GENERATED_BODY()

public:
    
    UCogEngineWindow_CommandBindings();

protected:

    virtual void PostInitProperties() override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual bool RegisterDefaultCommands();

private:
    
    UPROPERTY(Config)
    bool bRegisterDefaultCommands = true;

    void Sort(UPlayerInput* PlayerInput);

    void AddCogCommand(UPlayerInput* PlayerInput, const FString& Command, const FKey& Key);
};
