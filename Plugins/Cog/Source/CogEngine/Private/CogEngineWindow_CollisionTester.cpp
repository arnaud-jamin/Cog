#include "CogEngineWindow_CollisionTester.h"

#include "CogDebug.h"
#include "CogEngineDataAsset.h"
#include "CogImGuiHelper.h"
#include "CogWindowWidgets.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "imgui.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    SetAsset(GetAsset<UCogEngineDataAsset>());

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
bool RenderCollisionProfileChannels(const UCollisionProfile& CollisionProfileChannels, int32& Channels, const TArray<FCogCollisionChannel>& ChannelsConfig)
{
    bool Result = false;

    for (const FCogCollisionChannel& ChannelConfig : ChannelsConfig)
    {
	    const ECollisionChannel Channel = ChannelConfig.Channel.GetValue();
        ImGui::PushID(Channel);

        ImColor Color = FCogImguiHelper::ToImColor(ChannelConfig.Color);
        ImGui::ColorEdit4("Color", (float*)&Color.Value, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();

        bool IsCollisionActive = (Channels & ECC_TO_BITFIELD(Channel)) > 0;
        const FName ChannelName = CollisionProfileChannels.ReturnChannelNameFromContainerIndex(Channel);
        if (ImGui::Checkbox(TCHAR_TO_ANSI(*ChannelName.ToString()), &IsCollisionActive))
        {
            Result = true;

            if (IsCollisionActive)
            {
                Channels |= ECC_TO_BITFIELD(Channel);
            }
            else
            {
                Channels &= ~ECC_TO_BITFIELD(Channel);
            }
        }

        ImGui::PopID();
    }

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
bool RenderComboCollisionChannel(const char* Label, const UCollisionProfile& CollisionProfile, ECollisionChannel& SelectedChannel, const TArray<FCogCollisionChannel>& ChannelsConfig)
{
    const FName SelectedChannelName = CollisionProfile.ReturnChannelNameFromContainerIndex(SelectedChannel);

    bool Result = false;
    if (ImGui::BeginCombo(Label, TCHAR_TO_ANSI(*SelectedChannelName.ToString()), ImGuiComboFlags_HeightLarge))
    {
        for (const FCogCollisionChannel& ChannelConfig : ChannelsConfig)
        {
            const ECollisionChannel Channel = ChannelConfig.Channel.GetValue();
            ImGui::PushID(Channel);

            const FName ChannelName = CollisionProfile.ReturnChannelNameFromContainerIndex(Channel);

            ImColor Color = FCogImguiHelper::ToImColor(ChannelConfig.Color);
            ImGui::ColorEdit4("Color", (float*)&Color.Value, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();

            if (ImGui::Selectable(TCHAR_TO_ANSI(*ChannelName.ToString())))
            {
                SelectedChannel = Channel;
                Result = true;
            }

            ImGui::PopID();
        }
        ImGui::EndCombo();
    }

    return Result;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::RenderContent()
{
    Super::RenderContent();

    //-------------------------------------------------
    // Menu
    //-------------------------------------------------
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Draw Hit Locations", &Config->DrawHitLocations);
            ImGui::Checkbox("Draw Hit Impact Points", &Config->DrawHitImpactPoints);
            ImGui::Checkbox("Draw Hit Shapes", &Config->DrawHitShapes);
            ImGui::Checkbox("Draw Hit Normal", &Config->DrawHitNormals);
            ImGui::Checkbox("Draw Hit Impact Normal", &Config->DrawHitImpactNormals);
            ImGui::Checkbox("Draw Hit Primitives", &Config->DrawHitPrimitives);
            ImGui::Checkbox("Draw Hit Actors Names", &Config->DrawHitActorsNames);

            FCogWindowWidgets::SetNextItemToShortWidth();
            ImGui::SliderFloat("Hit Point Size", &Config->HitPointSize, 0.0f, 20.0f, "%0.f");

            ImGui::ColorEdit4("No Hit Color", (float*)&Config->NoHitColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Hit Color", (float*)&Config->HitColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Normal Color", (float*)&Config->NormalColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);
            ImGui::ColorEdit4("Impact Normal Color", (float*)&Config->ImpactNormalColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (ImGui::Button("Spawn Collision Tester", ImVec2(-1, 0)))
    {
	    const FActorSpawnParameters SpawnInfo;
        ACogEngineCollisionTester* Actor = GetWorld()->SpawnActor<ACogEngineCollisionTester>(SpawnInfo);
        FCogDebug::SetSelection(GetWorld(), Actor);
    }

    ACogEngineCollisionTester* CollisionTester = Cast<ACogEngineCollisionTester>(GetSelection());
    if (CollisionTester == nullptr)
    {
        ImGui::TextDisabled("Spawn or select a Collision Tester actor");
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
        FTransform Transform = CollisionTester->EndComponent->GetComponentTransform();
        if (EndGizmo.Draw("CollisionTesterEndGizmo", *LocalPlayerController, Transform, ECogDebug_GizmoFlags::NoRotation | ECogDebug_GizmoFlags::NoScale))
        {
            CollisionTester->EndComponent->SetWorldTransform(Transform);
        }
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
        if (RenderComboCollisionChannel("Channel", *CollisionProfile, Channel, Asset->Channels))
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
            FCogImguiHelper::DragDouble("Sphere Radius", &CollisionTester->ShapeExtent.X, 0.1f, 0, FLT_MAX, "%.1f");
            break;
        }

        case ECogEngine_CollisionQueryShape::Box:
        {
            FCogWindowWidgets::SetNextItemToShortWidth();
            FCogImguiHelper::DragFVector("Box Extent", CollisionTester->ShapeExtent, 0.1f, 0, FLT_MAX, "%.1f");
            break;
        }

        case ECogEngine_CollisionQueryShape::Capsule:
        {
            FCogWindowWidgets::SetNextItemToShortWidth();
            FCogImguiHelper::DragDouble("Capsule Radius", &CollisionTester->ShapeExtent.X, 0.1f, 0, FLT_MAX, "%.1f");

            FCogWindowWidgets::SetNextItemToShortWidth();
            FCogImguiHelper::DragDouble("Capsule Half Height", &CollisionTester->ShapeExtent.Z, 0.1f, 0, FLT_MAX, "%.1f");
            break;
        }
        }
    }

    ImGui::Separator();

    //-------------------------------------------------
	// Channels
	//-------------------------------------------------
    if (CollisionTester->By == ECogEngine_CollisionQueryBy::Profile)
    {
        ImGui::BeginDisabled();
        RenderCollisionProfileChannels(*CollisionProfile, CollisionTester->ObjectTypesToQuery, Asset->Channels);
        ImGui::EndDisabled();
    }
    else if (CollisionTester->By == ECogEngine_CollisionQueryBy::ObjectType)
    {
        RenderCollisionProfileChannels(*CollisionProfile, CollisionTester->ObjectTypesToQuery, Asset->Channels);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_CollisionTester::SetAsset(const UCogEngineDataAsset* Value)
{
    Asset = Value;

    if (Asset == nullptr)
    {
        return;
    }

    for (FChannel& Channel : Channels)
    {
        Channel.IsValid = false;
    }

    for (const FCogCollisionChannel& AssetChannel : Asset->Channels)
    {
        FChannel& Channel = Channels[(uint8)AssetChannel.Channel];
        Channel.IsValid = true;
        Channel.Color = AssetChannel.Color.ToFColor(true);
    }
}
