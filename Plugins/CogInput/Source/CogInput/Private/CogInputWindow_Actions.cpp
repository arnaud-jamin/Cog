#include "CogInputWindow_Actions.h"

#include "CogWidgets.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "imgui_internal.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Misc/EngineVersionComparison.h"
#include "Styling/SlateTypes.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Actions::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    Config = GetConfig<UCogInputConfig_Actions>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Actions::RenderHelp()
{
    ImGui::Text(
        "This window displays the current state of each Input Action. "
        "It can also be used to inject inputs to help debugging.");
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Actions::RenderContent()
{
    Super::RenderContent();

    ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (LocalPlayer == nullptr)
    {
        ImGui::TextDisabled("No Local Player");
        return;
    }

    UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    if (EnhancedInputSubsystem == nullptr)
    {
        ImGui::TextDisabled("No Enhanced Input Subsystem");
        return;
    }

    UEnhancedPlayerInput* PlayerInput = EnhancedInputSubsystem->GetPlayerInput();
    if(PlayerInput == nullptr)
    {
        ImGui::TextDisabled("No Player Input");
        return;
    }

    const bool IsControlDown = (ImGui::GetCurrentContext()->IO.KeyMods & ImGuiMod_Ctrl) != 0;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Show Header", &Config->ShowHeader);
            ImGui::SliderFloat("##Repeat", &Config->RepeatPeriod, 0.1f, 10.0f, "Repeat: %0.1fs");
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Reset"))
        {
            for (FCogInputMappingContextInfo& Mapping : Mappings)
            {
                for (FCogInputActionInfo& Action : Mapping.Actions)
                {
                    Action.Reset();
                }
            }
        }

        ImGui::EndMenuBar();
    }

    //--------------------------------------------------------------------------
    // Get all the existing mapping. This is not publicly exposed so we rob it.
	//--------------------------------------------------------------------------
    TArray<FCogInputMappingContextInfo, TInlineAllocator<8>> AllAppliedMappings;
    if (const CogInputMappingContextMap* AppliedMappingContexts = &PRIVATE_ACCESS_PTR(PlayerInput, UEnhancedPlayerInput_AppliedInputContexts))
    {
        for (auto& kv : *AppliedMappingContexts)
        {
            FCogInputMappingContextInfo& MappingInfo = AllAppliedMappings.AddDefaulted_GetRef();
            MappingInfo.MappingContext = kv.Key;

#if UE_VERSION_OLDER_THAN(5, 6, 0)
            MappingInfo.Priority = kv.Value;
#else //UE_VERSION_OLDER_THAN(5, 6, 0)
            MappingInfo.AppliedInputContextData = kv.Value;
#endif //UE_VERSION_OLDER_THAN(5, 6, 0)
        }
    }

    //--------------------------------------------------------------------------
	// Remove mappings that are not present anymore.
    //--------------------------------------------------------------------------
    for (int32 i = Mappings.Num() - 1; i >= 0; i--)
    {
        const FCogInputMappingContextInfo& MappingInfo = Mappings[i];
        if (AllAppliedMappings.ContainsByPredicate([&MappingInfo](const FCogInputMappingContextInfo& m)
            {
                return m.MappingContext == MappingInfo.MappingContext;
            }))
        {
            continue;
        }

        Mappings.RemoveAt(i);
    }

    //--------------------------------------------------------------------------
    // Add mappings that were not yet added.
    //--------------------------------------------------------------------------
    for (FCogInputMappingContextInfo& MappingInfo : AllAppliedMappings)
    {
        //----------------------------------------------------
		// The mapping was already added
		//----------------------------------------------------
        if (Mappings.ContainsByPredicate([&MappingInfo](const FCogInputMappingContextInfo& m) { return m.MappingContext == MappingInfo.MappingContext; }))
        {
            continue;
        }

        //----------------------------------------------------
		// Add the mapping
		//----------------------------------------------------
        FCogInputMappingContextInfo& NewMappingInfo = Mappings.AddDefaulted_GetRef();
        NewMappingInfo.MappingContext = MappingInfo.MappingContext;

#if UE_VERSION_OLDER_THAN(5, 6, 0)
        MappingInfo.Priority = MappingInfo.Priority;
#else
        NewMappingInfo.AppliedInputContextData = MappingInfo.AppliedInputContextData;
#endif

        //----------------------------------------------------
		// Add all the mapping actions
		//----------------------------------------------------
        for (const FEnhancedActionKeyMapping& Mapping : NewMappingInfo.MappingContext->GetMappings())
        {
            TObjectPtr<const UInputAction> Action = Mapping.Action;
            if (Action == nullptr)
            {
                continue;
            }

            //----------------------------------------------------
			// Actions are present multiple time, as many times
			// as they are mapped to key. We only want to display
            // the same action once per mapping
			//----------------------------------------------------
            if (NewMappingInfo.Actions.ContainsByPredicate([&Action](const FCogInputActionInfo& m){ return m.Action == Action; }))
            {
                continue;
            }

            FCogInputActionInfo& ActionInfo = NewMappingInfo.Actions.AddDefaulted_GetRef();
            ActionInfo.Action = Mapping.Action;
        }

        Mappings.Sort([](const FCogInputMappingContextInfo& Lhs, const FCogInputMappingContextInfo& Rhs)
            {
#if UE_VERSION_OLDER_THAN(5, 6, 0)
                return Lhs.Priority < Rhs.Priority;
#else
                return Lhs.AppliedInputContextData.Priority < Rhs.AppliedInputContextData.Priority;
#endif
            });
    }

    int Index = 0;

    for (FCogInputMappingContextInfo& Mapping : Mappings)
    {
        const auto MappingName = StringCast<ANSICHAR>(*Mapping.MappingContext.GetName());
        const bool Open = ImGui::CollapsingHeader(MappingName.Get(), ImGuiTreeNodeFlags_DefaultOpen);
        if (Open && ImGui::BeginTable("Actions", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize))
        {
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Current", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Inject", ImGuiTableColumnFlags_WidthStretch);

            if (Config->ShowHeader)
            {
                ImGui::TableHeadersRow();
            }

            for (FCogInputActionInfo& ActionInfo : Mapping.Actions)
            {
                ImGui::PushID(Index);

                const auto ActionName = StringCast<ANSICHAR>(*ActionInfo.Action->GetName());

                FInputActionValue ActionValue = PlayerInput->GetActionValue(ActionInfo.Action);

                switch (ActionInfo.Action->ValueType)
                {
	                case EInputActionValueType::Boolean:
	                {
	                    ImGui::TableNextRow();
	                    ImGui::TableNextColumn();
	                    ImGui::Text("%s", ActionName.Get());

	                    ImGui::TableNextColumn();
	                    ImGui::BeginDisabled();
	                    bool Value = ActionValue.Get<bool>();
                        FCogWidgets::PushBackColor(ImVec4(0.8f, 0.8f, 0.8f, 1));
                        ImGui::Checkbox("##Current", &Value);
                        FCogWidgets::PopBackColor();
                        ImGui::EndDisabled();

	                    ImGui::TableNextColumn();

                        ECheckBoxState State = ActionInfo.bRepeat ? ECheckBoxState::Undetermined : ActionInfo.bPressed ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;

                        if (FCogWidgets::CheckBoxState("##Inject", State, false))
                        {
                            if (IsControlDown)
                            {
                                ActionInfo.bRepeat = !ActionInfo.bRepeat;
                                ActionInfo.bPressed = false;
                            }
                            else
                            {
                                ActionInfo.bPressed = !ActionInfo.bPressed;
                                ActionInfo.bRepeat = false;
                            }
                        }

                        if (ImGui::BeginPopupContextItem())
                        {
                            ImGui::Checkbox("Repeat", &ActionInfo.bRepeat);
                            ImGui::EndPopup();
                        }

                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayShort))
                        {
                            ImGui::BeginTooltip();
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, IsControlDown ? 1.0f : 0.5f), "Repeat [CTRL]");
                            ImGui::EndTooltip();
                        }

	                    break;
	                }

	                case EInputActionValueType::Axis1D:
	                {
	                    const float Value = ActionValue.Get<float>();
	                    DrawAxis("%s", ActionName.Get(), Value, &ActionInfo.X);
	                    break;
	                }

	                case EInputActionValueType::Axis2D:
	                {
	                    const FVector2D Value = ActionValue.Get<FVector2D>();
	                    DrawAxis("%s X", ActionName.Get(), Value.X, &ActionInfo.X);
	                    DrawAxis("%s Y", ActionName.Get(), Value.Y, &ActionInfo.Y);
	                    break;
	                }

	                case EInputActionValueType::Axis3D:
	                {
	                    const FVector Value = ActionValue.Get<FVector>();
	                    DrawAxis("%s X", ActionName.Get(), Value.X, &ActionInfo.X);
	                    DrawAxis("%s Y", ActionName.Get(), Value.Y, &ActionInfo.Y);
	                    DrawAxis("%s Z", ActionName.Get(), Value.Z, &ActionInfo.Z);
	                    break;
	                }
				}

				ImGui::PopID();
                Index++;
            }

            ImGui::EndTable();
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Actions::RenderTick(float DeltaSeconds)
{
    Super::RenderTick(DeltaSeconds);

    if (GetWorld() == nullptr)
    {
        return;
    }

    ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (LocalPlayer == nullptr)
    {
        return;
    }

    UEnhancedInputLocalPlayerSubsystem* EnhancedInput = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    if (EnhancedInput == nullptr)
    {
        return;
    }

    bool IsTimeToRepeat = false;
    float WorldTime = GetWorld()->GetTimeSeconds();
    if (RepeatTime < WorldTime)
    {
        RepeatTime = WorldTime + Config->RepeatPeriod;
        IsTimeToRepeat = true;
    }

    for (FCogInputMappingContextInfo Mapping : Mappings)
    {
        for (FCogInputActionInfo& ActionInfo : Mapping.Actions)
        {
            ActionInfo.Inject(*EnhancedInput, IsTimeToRepeat);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogInputWindow_Actions::DrawAxis(const char* Format, const char* ActionName, float CurrentValue, float* InjectValue)
{
    ImGui::PushID(Format);
    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::Text(Format, ActionName);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    ImGui::BeginDisabled();
    FCogWidgets::PushBackColor(ImVec4(0.8f, 0.8f, 0.8f, 1));
    ImGui::SliderFloat("##Value", &CurrentValue, -1.0f, 1.0f, "%0.2f");
    FCogWidgets::PopBackColor();
    ImGui::EndDisabled();

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    FCogWidgets::SliderWithReset("##Inject", InjectValue, -1.0f, 1.0f, 0.0f, "%0.2f");
    ImGui::PopID();
}
