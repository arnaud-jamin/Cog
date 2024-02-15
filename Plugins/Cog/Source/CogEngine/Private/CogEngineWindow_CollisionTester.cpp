#include "CogEngineWindow_CollisionTester.h"

#include "CogDebug.h"
#include "CogImguiHelper.h"
#include "CogWindowWidgets.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/World.h"
#include "imgui.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    Config = GetConfig<UCogEngineConfig_CollisionTester>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::RenderHelp()
{
    ImGui::Text("This window is used to test a collision query.");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::ResetConfig()
{
    Super::ResetConfig();

    Config->Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::RenderContent()
{
    Super::RenderContent();

    ACogEngineCollisionTester* CollisionTester = Cast<ACogEngineCollisionTester>(GetSelection());

    //-------------------------------------------------
    // Menu
    //-------------------------------------------------
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::MenuItem("Spawn"))
        {
            const FActorSpawnParameters SpawnInfo;
            const FTransform Transform = GetSelection() ? GetSelection()->GetActorTransform() : FTransform::Identity;
            ACogEngineCollisionTester* NewActor = GetWorld()->SpawnActor<ACogEngineCollisionTester>(ACogEngineCollisionTester::StaticClass(), Transform, SpawnInfo);

#if WITH_EDITOR
            NewActor->SetActorLabel(NewActor->GetName().Replace(TEXT("CogEngine"), TEXT("")));
#endif

            FCogDebug::SetSelection(GetWorld(), NewActor);
        }
        if (ImGui::BeginItemTooltip())
        {
            ImGui::SetTooltip("Spawn a new collision tester");
            ImGui::EndTooltip();
        }

        const bool Disable = CollisionTester == nullptr;
        if (Disable)
        {
            ImGui::BeginDisabled();
        }
        if (ImGui::MenuItem("Delete"))
        {
            GetWorld()->DestroyActor(CollisionTester);
            CollisionTester = nullptr;
        }
        if (ImGui::BeginItemTooltip())
        {
            ImGui::SetTooltip("Delete selected collision tester");
            ImGui::EndTooltip();
        }
        if (Disable)
        {
            ImGui::EndDisabled();
        }

        ImGui::SetNextItemWidth(-1);
        FCogWindowWidgets::MenuActorsCombo("CollisionTesters", *GetWorld(), ACogEngineCollisionTester::StaticClass());

        ImGui::EndMenuBar();
    }

    if (CollisionTester == nullptr)
    {
        ImGui::PushTextWrapPos(0.0f);
        ImGui::TextDisabled("Select or spawn a Collision Tester actor");
        ImGui::PopTextWrapPos();
        return;
    }

    const UCollisionProfile* CollisionProfile = UCollisionProfile::Get();
    if (CollisionProfile == nullptr)
    {
        ImGui::TextDisabled("No CollisionProfile");
        return;
    }

    if (const APlayerController* LocalPlayerController = GetLocalPlayerController())
    {
        StartGizmo.Draw("CollisionTesterStartGizmo", *LocalPlayerController, *CollisionTester->StartComponent);
        EndGizmo.Draw("CollisionTesterEndGizmo", *LocalPlayerController, *CollisionTester->EndComponent, ECogDebug_GizmoFlags::NoRotation | ECogDebug_GizmoFlags::NoScale);
    }

    FCogWindowWidgets::SetNextItemToShortWidth();
    FCogWindowWidgets::ComboboxEnum("Type", CollisionTester->Type);

    FCogWindowWidgets::SetNextItemToShortWidth();
    FCogWindowWidgets::ComboboxEnum("Mode", CollisionTester->Mode);

    FCogWindowWidgets::SetNextItemToShortWidth();
    FCogWindowWidgets::ComboboxEnum("By", CollisionTester->By);

    //-------------------------------------------------
    // Channel
    //-------------------------------------------------
    if (CollisionTester->By == ECogEngine_CollisionQueryBy::Channel)
    {
        FCogWindowWidgets::SetNextItemToShortWidth();
        ECollisionChannel Channel = CollisionTester->Channel.GetValue();
        if (FCogWindowWidgets::ComboCollisionChannel("Channel", Channel))
        {
            CollisionTester->Channel = Channel;
        }
    }
    //-------------------------------------------------
    // Profile
    //-------------------------------------------------
    else if (CollisionTester->By == ECogEngine_CollisionQueryBy::Profile)
    {
        const FCollisionResponseTemplate* SelectedProfile = CollisionProfile->GetProfileByIndex(CollisionTester->ProfileIndex);
        const FName SelectedProfileName = SelectedProfile != nullptr ? SelectedProfile->Name : FName("Custom");

        FCogWindowWidgets::SetNextItemToShortWidth();
        if (ImGui::BeginCombo("Profile", TCHAR_TO_ANSI(*SelectedProfileName.ToString()), ImGuiComboFlags_HeightLargest))
        {
            for (int i = 0; i < CollisionProfile->GetNumOfProfiles(); ++i)
            {
                const FCollisionResponseTemplate* Profile = CollisionProfile->GetProfileByIndex(i);
                if (ImGui::Selectable(TCHAR_TO_ANSI(*Profile->Name.ToString()), false))
                {
                    CollisionTester->ProfileIndex = i;
                    CollisionTester->ObjectTypesToQuery = 0;
                    SelectedProfile = CollisionProfile->GetProfileByIndex(CollisionTester->ProfileIndex);

                    if (Profile->CollisionEnabled != ECollisionEnabled::NoCollision)
                    {
                        for (int j = 0; j < ECC_MAX; ++j)
                        {
	                        const ECollisionResponse Response = Profile->ResponseToChannels.GetResponse((ECollisionChannel)j);
                            if (Response != ECR_Ignore)
                            {
                                CollisionTester->ObjectTypesToQuery |= ECC_TO_BITFIELD(j);
                            }
                        }
                    }
                }
            }
            ImGui::EndCombo();
        }
    }

    ImGui::Checkbox("Complex", &CollisionTester->TraceComplex);

    //-------------------------------------------------
    // Shape
    //-------------------------------------------------

    if (CollisionTester->Type != ECogEngine_CollisionQueryType::LineTrace)
    {
        FCogWindowWidgets::SetNextItemToShortWidth();
        FCogWindowWidgets::ComboboxEnum("Shape", CollisionTester->Shape);

        switch (CollisionTester->Shape)
        {
        case ECogEngine_CollisionQueryShape::Sphere:
        {
            FCogWindowWidgets::SetNextItemToShortWidth();
            FCogImguiHelper::DragDouble("Sphere Radius", &CollisionTester->ShapeExtent.X, 1.0f, 0, FLT_MAX, "%.1f");
            break;
        }

        case ECogEngine_CollisionQueryShape::Box:
        {
            FCogWindowWidgets::SetNextItemToShortWidth();
            FCogImguiHelper::DragFVector("Box Extent", CollisionTester->ShapeExtent, 1.0f, 0, FLT_MAX, "%.1f");
            break;
        }

        case ECogEngine_CollisionQueryShape::Capsule:
        {
            FCogWindowWidgets::SetNextItemToShortWidth();
            FCogImguiHelper::DragDouble("Capsule Radius", &CollisionTester->ShapeExtent.X, 1.0f, 0, FLT_MAX, "%.1f");

            FCogWindowWidgets::SetNextItemToShortWidth();
            FCogImguiHelper::DragDouble("Capsule Half Height", &CollisionTester->ShapeExtent.Z, 1.0f, 0, FLT_MAX, "%.1f");
            break;
        }
        }
    }

    //-------------------------------------------------
	// Channels
	//-------------------------------------------------
    if (CollisionTester->By == ECogEngine_CollisionQueryBy::Profile)
    {
        ImGui::Separator();
        ImGui::BeginDisabled();
        FCogWindowWidgets::CollisionProfileChannels(CollisionTester->ObjectTypesToQuery);
        ImGui::EndDisabled();
    }
    else if (CollisionTester->By == ECogEngine_CollisionQueryBy::ObjectType)
    {
        ImGui::Separator();
        FCogWindowWidgets::CollisionProfileChannels(CollisionTester->ObjectTypesToQuery);
    }
}
