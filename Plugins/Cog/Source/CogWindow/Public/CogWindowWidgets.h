#pragma once

#include "CoreMinimal.h"
#include "imgui.h"
#include "UObject/ReflectedTypeAccessors.h"

class FEnumProperty;
class UCollisionProfile;
class UEnum;
class UObject;
enum class ECheckBoxState : uint8;
enum ECollisionChannel : int;
struct FCogImGuiKeyInfo;
struct FKeyBind;

using FCogWindowActorContextMenuFunction = TFunction<void(AActor& Actor)>;

class COGWINDOW_API FCogWindowWidgets
{
public:

    static void BeginTableTooltip();

    static void EndTableTooltip();

    static void ThinSeparatorText(const char* Label);

    static void ProgressBarCentered(float Fraction, const ImVec2& Size, const char* Overlay);

    static bool ToggleMenuButton(bool* Value, const char* Text, const ImVec4& TrueColor);

    static bool ToggleButton(bool* Value, const char* Text, const ImVec4& TrueColor, const ImVec4& FalseColor, const ImVec2& Size = ImVec2(0, 0));

    static bool ToggleButton(bool* Value, const char* TextTrue, const char* TextFalse, const ImVec4& TrueColor, const ImVec4& FalseColor, const ImVec2& Size = ImVec2(0, 0));

    static bool MultiChoiceButton(const char* Label, bool IsSelected, const ImVec2& Size = ImVec2(0, 0));

    static bool MultiChoiceButtonsInt(TArray<int32>& Values, int32& Value, const ImVec2& Size = ImVec2(0, 0));

    static bool MultiChoiceButtonsFloat(TArray<float>& Values, float& Value, const ImVec2& Size = ImVec2(0, 0));

    static void SliderWithReset(const char* Name, float* Value, float Min, float Max, const float& ResetValue, const char* Format);

    static void HelpMarker(const char* Text);

    static void PushStyleCompact();
    
    static void PopStyleCompact();

    static void AddTextWithShadow(ImDrawList* DrawList, const ImVec2& Position, ImU32 Color, const char* TextBegin, const char* TextEnd = NULL);

    static void SearchBar(ImGuiTextFilter& Filter, float Width = -1.0f);

    static void PushBackColor(const ImVec4& Color);

    static void PopBackColor();

    static float GetShortWidth();

    static void SetNextItemToShortWidth();

    static float GetFontWidth();

    template<typename EnumType>
    static bool ComboboxEnum(const char* Label, const EnumType CurrentValue, EnumType& NewValue);

    template<typename EnumType>
    static bool ComboboxEnum(const char* Label, EnumType& Value);

    static bool ComboboxEnum(const char* Label, UEnum* Enum, int64 CurrentValue, int64& NewValue);
    
    static bool ComboboxEnum(const char* Label, UObject* Object, const char* FieldName, uint8* PointerToEnumValue);
    
    static bool ComboboxEnum(const char* Label, const FEnumProperty* EnumProperty, uint8* PointerToEnumValue);

    static bool CheckBoxState(const char* Label, ECheckBoxState& State);

    static bool InputKey(const char* Label, FCogImGuiKeyInfo& KeyInfo);

    static bool InputKey(FCogImGuiKeyInfo& KeyInfo);

    static bool KeyBind(FKeyBind& KeyBind);

    static bool ButtonWithTooltip(const char* Text, const char* Tooltip);

    static bool DeleteArrayItemButton();

    static bool ComboCollisionChannel(const char* Label, ECollisionChannel& Channel);

    static bool CollisionProfileChannel(const UCollisionProfile& CollisionProfile, int32 ChannelIndex, FColor& ChannelColor, int32& Channels);

    static bool CollisionProfileChannels(int32& Channels);

    static bool MenuActorsCombo(const char* StrID, const UWorld& World, TSubclassOf<AActor> ActorClass, const FCogWindowActorContextMenuFunction& ContextMenuFunction = nullptr);

	static bool MenuActorsCombo(const char* StrID, const UWorld& World, const TArray<TSubclassOf<AActor>>& ActorClasses, int32& SelectedActorClassIndex, ImGuiTextFilter* Filter, const APawn* LocalPlayerPawn, const FCogWindowActorContextMenuFunction& ContextMenuFunction = nullptr);

    static bool ActorsListWithFilters(const UWorld& World, const TArray<TSubclassOf<AActor>>& ActorClasses, int32& SelectedActorClassIndex, ImGuiTextFilter* Filter, const APawn* LocalPlayerPawn, const FCogWindowActorContextMenuFunction& ContextMenuFunction = nullptr);

    static bool ActorsList(const UWorld& World, const TSubclassOf<AActor> ActorClass, const ImGuiTextFilter* Filter = nullptr, const APawn* LocalPlayerPawn = nullptr, const FCogWindowActorContextMenuFunction& ContextMenuFunction = nullptr);

    static void ActorContextMenu(AActor& Selection, const FCogWindowActorContextMenuFunction& ContextMenuFunction);

    static void ActorFrame(const AActor& Actor);

    static void SmallButton(const char* Text, const ImVec4& Color);
};

template<typename EnumType>
bool FCogWindowWidgets::ComboboxEnum(const char* Label, const EnumType CurrentValue, EnumType& NewValue)
{
    int64 NewValueInt;
    if (ComboboxEnum(Label, StaticEnum<EnumType>(), (int64)CurrentValue, NewValueInt))
    {
        NewValue = (EnumType)NewValueInt;
        return true;
    }

    return false;
}

template<typename EnumType>
bool FCogWindowWidgets::ComboboxEnum(const char* Label, EnumType& Value)
{
    return ComboboxEnum(Label, Value, Value);
}