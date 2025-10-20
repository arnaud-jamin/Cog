#pragma once

#include "CoreMinimal.h"
#include "imgui.h"
#include "UObject/ReflectedTypeAccessors.h"

#include <Templates/SubclassOf.h>

#include "CogHelper.h"
#include "InputCoreTypes.h"

class AActor;
class APawn;
class FEnumProperty;
class UCollisionProfile;
class UEnum;
class UObject;
enum class ECheckBoxState : uint8;
enum ECollisionChannel : int;
struct FKeyBind;
struct FCogInputChord;

using FCogWindowActorContextMenuFunction = TFunction<void(AActor& Actor)>;

//--------------------------------------------------------------------------------------------------------------------------
class COG_API FCogWidgets
{
public:

    static bool BeginTableTooltip();

    static void EndTableTooltip();

    static bool ItemTooltipWrappedText(const char* InText);

    static bool BeginItemTooltipWrappedText();

    static void EndItemTooltipWrappedText();
    
    static bool BeginItemTableTooltip();

    static void EndItemTableTooltip();

    static void ThinSeparatorText(const char* Label);

    static bool DarkCollapsingHeader(const char* InLabel, ImGuiTreeNodeFlags InFlags);

    static void ProgressBarCentered(float Fraction, const ImVec2& Size, const char* Overlay);

    static bool ToggleMenuButton(bool* Value, const char* Text, const ImVec4& TrueColor);

    static bool ToggleButton(bool* Value, const char* Text, const ImVec4& TrueColor, const ImVec4& FalseColor, const ImVec2& Size = ImVec2(0, 0));

    static bool ToggleButton(bool* Value, const char* TextTrue, const char* TextFalse, const ImVec4& TrueColor, const ImVec4& FalseColor, const ImVec2& Size = ImVec2(0, 0));

    static bool MultiChoiceButton(const char* Label, bool IsSelected, const ImVec2& Size = ImVec2(0, 0));

    static bool MultiChoiceButtonsInt(TArray<int32>& Values, int32& Value, const ImVec2& Size = ImVec2(0, 0), bool InInline = true);

    static bool MultiChoiceButtonsFloat(TArray<float>& InValues, float& InValue, const ImVec2& InSize = ImVec2(0, 0), bool InInline = true, float InTolerance = UE_SMALL_NUMBER);
    
    static void SliderWithReset(const char* Name, float* Value, float Min, float Max, const float& ResetValue, const char* Format);

    static void HelpMarker(const char* Text);

    static void PushStyleCompact();
    
    static void PopStyleCompact();

    static void AddTextWithShadow(ImDrawList* DrawList, const ImVec2& Position, ImU32 Color, const char* TextBegin, const char* TextEnd = nullptr);

    static bool SearchBar(const char* InLabel, ImGuiTextFilter& InFilter, float InWidth = -1.0f);

    static void PushButtonBackColor(const ImVec4& Color);

    static void PopButtonBackColor();
    static void PushFrameBackColor(const ImVec4& Color);
    static void PushSliderBackColor(const ImVec4& Color);

    static void PushBackColor(const ImVec4& Color);
    static void PopSliderBackColor();
    static void PopFrameBackColor();

    static void PopBackColor();

    static float GetShortWidth();

    static void SetNextItemToShortWidth();

    static float GetFontWidth();

    template<typename EnumType>
    static bool ComboboxEnum(const char* Label, const EnumType CurrentValue, EnumType& NewValue);

    template<typename EnumType>
    static bool ComboboxEnum(const char* Label, EnumType& Value);

    static bool ComboboxEnum(const char* Label, const UEnum* Enum, int64 CurrentValue, int64& NewValue);


    static bool ComboboxEnum(const char* Label, const UObject* Object, const char* FieldName, uint8* PointerToEnumValue);
    
    static bool ComboboxEnum(const char* Label, const FEnumProperty* EnumProperty, uint8* PointerToEnumValue);

    static bool CheckBoxState(const char* Label, ECheckBoxState& State, bool ShowTooltip = true);

    static bool InputChord(const char* Label, FCogInputChord& InInputChord);

    static bool InputChord(FInputChord& InInputChord);
    
    static bool Key(FKey& InKey);
    
    static bool KeyBind(FKeyBind& InKeyBind);

    static bool ButtonWithTooltip(const char* Text, const char* Tooltip);

    static bool DeleteArrayItemButton();

    static bool ComboTraceChannel(const char* Label, ECollisionChannel& Channel);

    static bool CollisionProfileChannels(int32& OutChannels);

    static bool CollisionTraceChannels(int32& OutChannels);

    static bool CollisionObjectTypeChannels(int32& OutChannels);

    static bool CollisionProfileChannel(const UCollisionProfile& InCollisionProfile, int32 InChannelIndex, FColor& InChannelColor, int32& InChannels);

    static bool MenuActorsCombo(const char* StrID, AActor*& NewSelection, const UWorld& World, const TSubclassOf<AActor>& ActorClass, const FCogWindowActorContextMenuFunction& ContextMenuFunction = nullptr);

    static bool MenuActorsCombo(const char* StrID, AActor*& NewSelection, const UWorld& World, const TArray<TSubclassOf<AActor>>& ActorClasses, int32& SelectedActorClassIndex, ImGuiTextFilter* Filter, const APawn* LocalPlayerPawn, const FCogWindowActorContextMenuFunction& ContextMenuFunction = nullptr);

    static bool ActorsListWithFilters(AActor*& NewSelection, const UWorld& World, const TArray<TSubclassOf<AActor>>& ActorClasses, int32& SelectedActorClassIndex, ImGuiTextFilter* Filter, const APawn* LocalPlayerPawn, const FCogWindowActorContextMenuFunction& ContextMenuFunction = nullptr);

    static bool ActorsList(AActor*& NewSelection, const UWorld& World, const TSubclassOf<AActor>& ActorClass, const ImGuiTextFilter* Filter = nullptr, const APawn* LocalPlayerPawn = nullptr, const FCogWindowActorContextMenuFunction& ContextMenuFunction = nullptr);

    static void ActorContextMenu(AActor& Selection, const FCogWindowActorContextMenuFunction& ContextMenuFunction);

    static void ActorFrame(const AActor& Actor);

    static void SmallButton(const char* Text, const ImVec4& Color);

    static bool InputText(const char* Text, FString& Value,  ImGuiInputTextFlags InFlags = 0, ImGuiInputTextCallback InCallback = nullptr, void* InUserData = nullptr);

    static bool InputTextWithHint(const char* InText, const char* InHint, FString& InValue, ImGuiInputTextFlags InFlags = 0, ImGuiInputTextCallback InCallback = nullptr, void* InUserData = nullptr);

    static bool BeginRightAlign(const char* Id);

    static void EndRightAlign();

    static void MenuItemShortcut(const char* Id, const FString& Text);

    template <typename TCLass, typename TMember>
    static void InputChordProperty(TCLass* InConfig, TMember TCLass::* InInputChordPointerToMember);
    
    template <typename TCLass, typename TMember>
    static void TextInputChordProperty(TCLass* InConfig, TMember TCLass::* InInputChordPointerToMember);

    static bool BrowseToAssetButton(const UObject* InAsset, const ImVec2& InSize = ImVec2(0, 0));
    
    static bool BrowseToAssetButton(const FAssetData& InAssetData, const ImVec2& InSize = ImVec2(0, 0));

    static bool BrowseToObjectAssetButton(const UObject* InObject, const ImVec2& InSize = ImVec2(0, 0));

    static bool OpenAssetButton(const UObject* InAsset, const ImVec2& InSize = ImVec2(0, 0));

    static bool OpenObjectAssetButton(const UObject* InObject, const ImVec2& InSize = ImVec2(0, 0));

    static void RenderCloseButton(const ImVec2& InPos);

    static bool PickButton(const char* InLabel, const ImVec2& InSize, ImGuiButtonFlags InFlags = ImGuiButtonFlags_None);

    static FString RemoveFirstZero(const FString& InText);

    static FString FormatSmallFloat(float InValue);

    static void FloatArray(const char* InLabel, TArray<float>& InArray, int32 InMaxEntries = 0, const ImVec2& Size = ImVec2(0, 0));
    
    static void IntArray(const char* InLabel, TArray<int>& InArray, int32 InMaxEntries = 0, const ImVec2& Size = ImVec2(0, 0));

    template<typename T>
    static bool ScalarArray(const char* InLabel, ImGuiDataType InDataType, TArray<T>& InArray, int32 InMaxEntries = 0, const ImVec2& Size = ImVec2(0, 0));

    static ImVec2 ComputeScreenCornerLocation(const FVector2f& InAlignment, const FIntVector2& InPadding);

    static ImVec2 ComputeScreenCornerLocation(const ImVec2& InAlignment, const ImVec2& InPadding);

    static FString GetStringAfterCharacter(const FString& InString, TCHAR InChar);

    static FString FormatConfigName(const FString& InConfigName);

    static FString FormatShortcutName(const FString& InShortcutName);

    static void TextInputChordProperty(UObject& InConfig, const FProperty& InInputChordProperty);

    static bool InputChordProperty(UObject& InConfig, const FProperty& InInputChordProperty);

    static bool IsConfigContainingInputChords(const UObject& InConfig);

    static bool AllInputChordsOfConfig(UObject& InConfig, FProperty** InModifiedProperty = nullptr);

    static void TextOfAllInputChordsOfConfig(UObject& InConfig);
};

template<typename EnumType>
bool FCogWidgets::ComboboxEnum(const char* Label, const EnumType CurrentValue, EnumType& NewValue)
{
    int64 NewValueInt;
    if (ComboboxEnum(Label, StaticEnum<EnumType>(), static_cast<int64>(CurrentValue), NewValueInt))
    {
        NewValue = static_cast<EnumType>(NewValueInt);
        return true;
    }

    return false;
}

template<typename EnumType>
bool FCogWidgets::ComboboxEnum(const char* Label, EnumType& Value)
{
    return ComboboxEnum(Label, Value, Value);
}

template<typename T>
 bool FCogWidgets::ScalarArray(const char* InLabel, ImGuiDataType InDataType, TArray<T>& InArray, int32 InMaxEntries, const ImVec2& Size)
{
    bool Result = false;
    ImGui::PushID(InLabel);

    if (ImGui::BeginChild("##Entries", Size, ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::TextUnformatted(InLabel);

            int32 NumEntries = InArray.Num();
            SetNextItemToShortWidth();
            if (ImGui::SliderInt("##Size", &NumEntries, 0, InMaxEntries))
            {
                InArray.SetNum(NumEntries);
                Result = true;
            }
            ImGui::EndMenuBar();
        }
            
        for (int32 i = 0; i < InArray.Num(); i++)
        {
            ImGui::PushID(i);
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputScalar("##Entry", InDataType, &InArray[i]))
            {
                Result = true;
            }
            ImGui::PopID();
        }
    }
    ImGui::EndChild();

    ImGui::PopID();

    return Result;
}

template <typename TCLass, typename TMember>
void FCogWidgets::InputChordProperty(TCLass* InConfig, TMember TCLass::* InInputChordPointerToMember)
{
    if (InConfig == nullptr)
    { return; }
    
    FProperty* Property = FCogHelper::FindProperty(InConfig, InInputChordPointerToMember);
    if (Property == nullptr)
    { return; }

    InputChordProperty(*InConfig, *Property);
}

template <typename TCLass, typename TMember>
void FCogWidgets::TextInputChordProperty(TCLass* InConfig, TMember TCLass::* InInputChordPointerToMember)
{
    if (InConfig == nullptr)
    { return; }
    
    FProperty* Property = FCogHelper::FindProperty(InConfig, InInputChordPointerToMember);
    if (Property == nullptr)
    { return; }

    TextInputChordProperty(*InConfig, *Property);
}