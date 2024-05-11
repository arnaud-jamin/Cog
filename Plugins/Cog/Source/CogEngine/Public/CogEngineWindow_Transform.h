#pragma once

#include "CoreMinimal.h"
#include "CogCommonConfig.h"
#include "CogDebugGizmo.h"
#include "CogWindow.h"
#include "CogEngineWindow_Transform.generated.h"

class UCogEngineConfig_Transform;

class COGENGINE_API FCogEngineWindow_Transform : public FCogWindow
{
    typedef FCogWindow Super;

public:

    virtual void Initialize() override;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    bool RenderComponent(const char* Label, double* Value, float Speed, double Min, double Max, double Reset);

    void RenderSnap(const char* CheckboxLabel, const char* InputLabel, bool* SnapEnable, float* Snap);

    bool RenderLocation(FTransform& InOutTransform);

    bool RenderRotation(FTransform& InOutTransform);

    bool RenderScale(FTransform& InOutTransform);

    bool RenderTransform(FTransform& InOutTransform);

private:

    FCogDebug_Gizmo Gizmo;

    TWeakObjectPtr<UCogEngineConfig_Transform> Config = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogEngineConfig_Transform : public UCogCommonConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    bool ShowGizmo = true;

    UPROPERTY(Config)
    float LocationSpeed = 10.0f;

    UPROPERTY(Config)
    float RotationSpeed = 1.0f;

    UPROPERTY(Config)
    float ScaleSpeed = 0.1f;

    virtual void Reset() override
    {
        Super::Reset();
        ShowGizmo = true;
        LocationSpeed = 10.0f;
        RotationSpeed = 1.0f;
        ScaleSpeed = 0.1f;
    }
};