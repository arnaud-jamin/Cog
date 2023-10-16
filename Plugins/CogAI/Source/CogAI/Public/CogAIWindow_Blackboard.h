#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogAIWindow_Blackboard.generated.h"

class UCogAIDataAsset;

UCLASS(Config = Cog)
class COGAI_API UCogAIWindow_Blackboard : public UCogWindow
{
    GENERATED_BODY()

public:

    UCogAIWindow_Blackboard();

    //const UCogAIDataAsset* GetAsset() const { return Asset.Get(); }

    //void SetAsset(const UCogAIDataAsset* Value) { Asset = Value; }

protected:

    void RenderHelp();

    virtual void RenderContent() override;

private:


    //UPROPERTY()
    //TWeakObjectPtr<const UCogAIDataAsset> Asset = nullptr;
};
