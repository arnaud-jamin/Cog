#include "CogEngineWindow_Inspector.h"

#include "CogCommon.h"
#include "CogWidgets.h"
#include "Containers/SortedMap.h"
#include "Engine/Engine.h"
#include "imgui_internal.h"
#include "UObject/EnumProperty.h"
#include "UObject/TextProperty.h"
#include "UObject/UnrealType.h"

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Inspector::Initialize()
{
    Super::Initialize();

    bHasMenu = true;

    Config = GetConfig<UCogEngineConfig_Inspector>();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Inspector::RenderHelp()
{
    ImGui::Text(
        "This window can be used to inspect the properties of a UObject. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Inspector::SetInspectedObject(UObject* Value)
{
    if (InspectedObject == Value)
    {
        return;
    }

    InspectedObject = Value;
    
    if (InspectedObject != GetSelection())
    {
        Config->bSyncWithSelection = false;
    }

    if (HistoryIndex != History.Num() - 1)
    {
        History.SetNum(HistoryIndex + 1);
    }
    
    if (History.Num() > 9)
    {
        History.RemoveAt(0);
    }

    HistoryIndex = History.Add(Value);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Inspector::AddFavorite(UObject* Object)
{
    FFavorite& Favorite = Favorites.AddDefaulted_GetRef();
    Favorite.Object = Object;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Inspector::AddFavorite(UObject* Object, FCogEngineInspectorApplyFunction ApplyFunction)
{
    FFavorite& Favorite = Favorites.AddDefaulted_GetRef();
    Favorite.Object = Object;
    Favorite.ApplyFunction = ApplyFunction;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Inspector::RenderContent()
{
    Super::RenderContent();

    RenderMenu();

    //--------------------------
    // Objects to inspect Combo
    //--------------------------

    if (Config->bSyncWithSelection || InspectedObject == nullptr)
    {
        SetInspectedObject(GetSelection());
    }

    if (InspectedObject == nullptr)
    {
        return;
    }

    const FCogEngineInspectorApplyFunction ApplyFunction = FindObjectApplyFunction(InspectedObject.Get());

    ImGui::BeginChild("Inspector", ImVec2(-1, ApplyFunction != nullptr ? -ImGui::GetFrameHeightWithSpacing() : -1), false);

    RenderInspector();

    ImGui::EndChild();

    if (ApplyFunction != nullptr)
    {
        if (ImGui::Button("Apply", ImVec2(-1, 0)))
        {
            ApplyFunction(InspectedObject.Get());
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
FCogEngineInspectorApplyFunction FCogEngineWindow_Inspector::FindObjectApplyFunction(const UObject* Object) const
{
    for (const FFavorite& Favorite : Favorites)
    {
        if (Favorite.Object == Object)
        {
            return Favorite.ApplyFunction;
        }
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Inspector::RenderMenu()
{
    bCollapseAllCategories = false;
    bExpandAllCategories = false;

    if (ImGui::BeginMenuBar())
    {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        int32 NewHistoryIndex = INDEX_NONE;

        //-----------------------------------
        // Backward / Forward
        //-----------------------------------
        {
            if (ImGui::ArrowButton("##Backward", ImGuiDir_Left))
            {
                NewHistoryIndex = FMath::Max(0, HistoryIndex - 1);
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
            {
                ImGui::SetTooltip("Backward");
            }

            if (ImGui::ArrowButton("##Forward", ImGuiDir_Right))
            {
                NewHistoryIndex = FMath::Min(History.Num(), HistoryIndex + 1);
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
            {
                ImGui::SetTooltip("Forward");
            }
        }

        //-----------------------------------
        // Current Inspected Object
        //-----------------------------------
        const auto InspectedObjectName = StringCast<ANSICHAR>(*GetNameSafe(InspectedObject.Get()));
        const ImVec2 Pos = ImGui::GetCursorScreenPos();
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
            ImGui::SameLine();

            if (ImGui::Button(InspectedObjectName.Get(), ImVec2(FCogWidgets::GetFontWidth() * 20, 0)))
            {
                ImGui::OpenPopup("SelectionPopup");
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("%s", InspectedObjectName.Get());
            }

            ImGui::PopStyleVar(1);
        }

        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar(1);

        //-----------------------------------
        // Popup
        //-----------------------------------
        ImGui::SetNextWindowPos(Pos + ImVec2(0, ImGui::GetFrameHeight()));
        if (ImGui::BeginPopup("SelectionPopup"))
        {
            ImGui::BeginChild("Popup", ImVec2(FCogWidgets::GetFontWidth() * 30, FCogWidgets::GetFontWidth() * 40), false);

            //-----------------------------------
            // FAVORITES
            //-----------------------------------
            ImGui::SeparatorText("FAVORITES");

            if (ImGui::MenuItem("Selection"))
            {
                SetInspectedObject(GetSelection());
                ImGui::CloseCurrentPopup();
            }

            ImGui::PushID("Favorites");
            for (FFavorite& Favorite : Favorites)
            {
                const TWeakObjectPtr<UObject>& Object = Favorite.Object;
                if (ImGui::MenuItem(COG_TCHAR_TO_CHAR(*GetNameSafe(Object.Get()))))
                {
                    SetInspectedObject(Object.Get());
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::PopID();

            //-----------------------------------
            // HISTORY
            //-----------------------------------
            ImGui::Spacing();
            ImGui::SeparatorText("HISTORY");
            ImGui::PushID("History");
            for (int32 i = History.Num() - 1; i >= 0; i--)
            {
                ImGui::PushID(i);
                const TWeakObjectPtr<const UObject>& Object = History[i];
                if (ImGui::MenuItem(COG_TCHAR_TO_CHAR(*GetNameSafe(Object.Get())), nullptr, i == HistoryIndex))
                {
                    NewHistoryIndex = i;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::PopID();
            }
            ImGui::PopID();

            ImGui::EndChild();
            ImGui::EndPopup();
        }

        //-----------------------------------
        // Apply history selection
        //-----------------------------------
        if (NewHistoryIndex != INDEX_NONE)
        {
            if (History.IsValidIndex(NewHistoryIndex))
            {
                HistoryIndex = NewHistoryIndex;
                InspectedObject = History[HistoryIndex];
                Config->bSyncWithSelection = false;
            }
        }

        //-----------------------------------
        // Search
        //-----------------------------------
        FCogWidgets::SearchBar("##Filter", Filter, -FCogWidgets::GetFontWidth() * 9);

        //-----------------------------------
        // Options
        //-----------------------------------
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Sync With Selection", &Config->bSyncWithSelection);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Should the inspector be synced with the actor selection ?");
            }

            ImGui::Separator();

            ImGui::Checkbox("Sort by Name", &Config->bSortByName);
            ImGui::Checkbox("Show Background", &Config->bShowRowBackground);
            ImGui::Checkbox("Show Borders", &Config->bShowBorders);
#if WITH_EDITORONLY_DATA
            ImGui::Checkbox("Show Display Name", &Config->bShowDisplayName);
            ImGui::Checkbox("Show Categories", &Config->bShowCategories);

            ImGui::Separator();
            if (ImGui::MenuItem("Collapse All Categories", nullptr, false, Config->bShowCategories))
            {
                bCollapseAllCategories = true;
            }
            if (ImGui::MenuItem("Expand All Categories", nullptr, false, Config->bShowCategories))
            {
                bExpandAllCategories = true;
            }
#endif  // WITH_EDITORONLY_DATA

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderInspector()
{
    const UObject* Object = GetInspectedObject();
    if (Object == nullptr)
    {
        return false;
    }

    TArray<const FProperty*> Properties;
    for (TFieldIterator<FProperty> It(Object->GetClass()); It; ++It)
    {
        const FProperty* Property = *It;

        if (Filter.IsActive() == false || Filter.PassFilter(COG_TCHAR_TO_CHAR(*Property->GetName())))
        {
            Properties.Add(Property);
        }
    }

    bool HasChanged = false;
    bool IsPropertyGridRendered = false;

#if WITH_EDITORONLY_DATA

    //----------------------------------------------------------------------
    // Render properties with categories
    //----------------------------------------------------------------------

    if ((Config->bShowCategories) != 0)
    {
        IsPropertyGridRendered = true;

        TSortedMap<FString, TArray<const FProperty*>> PropertiesByCategories;
        for (const FProperty* Property : Properties)
        {
            FString Category = Property->HasMetaData(TEXT("Category")) ? Property->GetMetaData(TEXT("Category")) : TEXT("Default");
            TArray<const FProperty*>& PropertiesInCategory = PropertiesByCategories.FindOrAdd(Category);
            PropertiesInCategory.Add(Property);
        }

        int TableIndex = 0;
        for (auto& Entry : PropertiesByCategories)
        {
            if ((bExpandAllCategories) != 0)
            {
                ImGui::SetNextItemOpen(true);
            }
            else if ((bCollapseAllCategories) != 0)
            {
                ImGui::SetNextItemOpen(false);
            }

            if (ImGui::CollapsingHeader(TCHAR_TO_UTF8(*Entry.Key), nullptr, ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (RenderBegin())
                {
                    HasChanged |= RenderPropertyList(Entry.Value, (uint8*)Object);
                    RenderEnd();
                }
            }
            TableIndex++;
        }
    }

#endif  // WITH_EDITORONLY_DATA

    //----------------------------------------------------------------------
    // Render properties without categories
    //----------------------------------------------------------------------
    if (IsPropertyGridRendered == false)
    {
        if (RenderBegin())
        {
            HasChanged = RenderPropertyList(Properties, (uint8*)Object);
            RenderEnd();
        }
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderBegin()
{
    FCogWidgets::PushStyleCompact();

    ImGuiTableFlags TableFlags = ImGuiTableFlags_Resizable;
    if ((Config->bShowBorders) != 0)
    {
        TableFlags |= ImGuiTableFlags_Borders;
    }
    else
    {
        TableFlags |= ImGuiTableFlags_NoBordersInBodyUntilResize;
    }

    if ((Config->bShowRowBackground) != 0)
    {
        TableFlags |= ImGuiTableFlags_RowBg;
    }

    if (ImGui::BeginTable("PropertyGridTable", 2, TableFlags))
    {
        ImGui::TableSetupColumn("Setting");
        ImGui::TableSetupColumn("Value");
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogEngineWindow_Inspector::RenderEnd()
{
    ImGui::EndTable();
    FCogWidgets::PopStyleCompact();
}

//--------------------------------------------------------------------------------------------------------------------------
FString FCogEngineWindow_Inspector::GetPropertyName(const FProperty& Property)
{
#if WITH_EDITORONLY_DATA

    if (Config->bShowDisplayName && Property.GetDisplayNameText().IsEmpty() == false)
    {
        return Property.GetDisplayNameText().ToString();
    }

    return Property.GetAuthoredName();

#else

    return Property.GetAuthoredName();

#endif
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderPropertyList(TArray<const FProperty*>& Properties, uint8* PointerToValue)
{
    if (Config->bSortByName)
    {
        Properties.Sort([this](const FProperty& Lhs, const FProperty& Rhs) { return GetPropertyName(Lhs) < GetPropertyName(Rhs); });
    }

    bool HasChanged = false;
    int index = 0;
    for (const FProperty* Property : Properties)
    {
        ImGui::PushID(index++);
        uint8* InnerPointerToValue = Property->ContainerPtrToValuePtr<uint8>(PointerToValue);
        HasChanged |= RenderProperty(Property, InnerPointerToValue, -1);
        ImGui::PopID();
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderProperty(const FProperty* Property, uint8* PointerToValue, int IndexInArray, const char* NameSuffix)
{
    bool HasChanged = false;

    ImGui::PushID(PointerToValue);

    //--------------------------------------------------------------------------------------
    // Render Property Name
    //--------------------------------------------------------------------------------------
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();

    FString PropertyName;
    if (IndexInArray != -1)
    {
        PropertyName = FString::Printf(TEXT("[%d]"), IndexInArray);
        if (NameSuffix != nullptr)
        {
            PropertyName.Append(" ");
            PropertyName.Append(NameSuffix);
        }
    }
    else
    {
        PropertyName = GetPropertyName(*Property);
    }

    bool ShowChildren = false;
    if (HasPropertyAnyChildren(Property, PointerToValue))
    {
        ShowChildren = ImGui::TreeNodeEx(COG_TCHAR_TO_CHAR(*PropertyName), ImGuiTreeNodeFlags_SpanFullWidth);
    }
    else
    {
        ImGui::TreeNodeEx(COG_TCHAR_TO_CHAR(*PropertyName), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
    }

    //--------------------------------------------------------------------------------------
    // Render Property Tooltip
    //--------------------------------------------------------------------------------------
    if (ImGui::IsItemHovered() && ImGui::GetIO().KeyCtrl)
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);

        if (ImGui::BeginTable("Infos", 2, ImGuiTableFlags_RowBg))
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Type:");
            ImGui::TableNextColumn();
            ImGui::Text("%s", COG_TCHAR_TO_CHAR(*Property->GetClass()->GetName()));

#if WITH_EDITORONLY_DATA
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("DisplayName:");
            ImGui::TableNextColumn();
            ImGui::Text(TCHAR_TO_UTF8(*Property->GetDisplayNameText().ToString()));
#endif  // WITH_EDITORONLY_DATA

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("FullName:");
            ImGui::TableNextColumn();
            ImGui::Text("%s", COG_TCHAR_TO_CHAR(*Property->GetFullName()));

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Address:");
            ImGui::TableNextColumn();
            ImGui::Text("#%p", PointerToValue);

#if WITH_EDITORONLY_DATA
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Tooltip:");
            ImGui::TableNextColumn();
            if (Property->HasMetaData("Tooltip"))
            {
                ImGui::Text(TCHAR_TO_UTF8(*Property->GetToolTipText(false).ToString()));
            }
#endif  // WITH_EDITORONLY_DATA

            ImGui::EndTable();
        }

        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
#if WITH_EDITORONLY_DATA
    else if (Property->HasMetaData("Tooltip"))
    {
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);

            ImGui::Text(TCHAR_TO_UTF8(*Property->GetToolTipText(false).ToString()));
            ImGui::Text("Details [CTRL]");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
#endif // WITH_EDITORONLY_DATA


    //--------------------------------------------------------------------------------------
    // Render Property Value
    //--------------------------------------------------------------------------------------

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);

    if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
    {
        HasChanged = RenderBool(BoolProperty, PointerToValue);
    }
    else if (const FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
    {
        HasChanged = RenderByte(ByteProperty, PointerToValue);
    }
    else if (const FInt8Property* Int8Property = CastField<FInt8Property>(Property))
    {
        HasChanged = RenderInt8(Int8Property, PointerToValue);
    }
    else if (const FIntProperty* IntProperty = CastField<FIntProperty>(Property))
    {
        HasChanged = RenderInt(IntProperty, PointerToValue);
    }
    else if (const FUInt32Property* UInt32Property = CastField<FUInt32Property>(Property))
    {
        HasChanged = RenderUInt32(UInt32Property, PointerToValue);
    }
    else if (const FInt64Property* Int64Property = CastField<FInt64Property>(Property))
    {
        HasChanged = RenderInt64(Int64Property, PointerToValue);
    }
    else if (const FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
    {
        HasChanged = RenderFloat(FloatProperty, PointerToValue);
    }
    else if (const FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(Property))
    {
        HasChanged = RenderDouble(DoubleProperty, PointerToValue);
    }
    else if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
    {
        HasChanged = RenderEnum(EnumProperty, PointerToValue);
    }
    else if (const FStrProperty* StrProperty = CastField<FStrProperty>(Property))
    {
        HasChanged = RenderString(StrProperty, PointerToValue);
    }
    else if (const FNameProperty* NameProperty = CastField<FNameProperty>(Property))
    {
        HasChanged = RenderName(NameProperty, PointerToValue);
    }
    else if (const FTextProperty* TextProperty = CastField<FTextProperty>(Property))
    {
        HasChanged = RenderText(TextProperty, PointerToValue);
    }
    else if (const FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
    {
        HasChanged = RenderClass(ClassProperty);
    }
    else if (const FInterfaceProperty* InterfaceProperty = CastField<FInterfaceProperty>(Property))
    {
        HasChanged = RenderInterface(InterfaceProperty);
    }
    else if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
    {
        HasChanged = RenderStruct(StructProperty, PointerToValue, ShowChildren);
    }
    else if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
    {
        UObject* ReferencedObject = ObjectProperty->GetObjectPropertyValue(PointerToValue);
        HasChanged = RenderObject(ReferencedObject, ShowChildren);
    }
    else if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
    {
        HasChanged = RenderArray(ArrayProperty, PointerToValue, ShowChildren);
    }
    else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
    {
        HasChanged = RenderSet(SetProperty, PointerToValue, ShowChildren);
    }
    else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
    {
        HasChanged = RenderMap(MapProperty, PointerToValue, ShowChildren);
    }
    else if (const FDelegateProperty* DelegateProperty = CastField<FDelegateProperty>(Property))
    {
    }
    else if (const FMulticastDelegateProperty* MulticastDelegateProperty = CastField<FMulticastDelegateProperty>(Property))
    {
    }
    else if (const FWeakObjectProperty* WeakObjectProperty = CastField<FWeakObjectProperty>(Property))
    {
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 128));
        ImGui::Text("Unmanaged Type: %s", COG_TCHAR_TO_CHAR(*Property->GetClass()->GetName()));
        ImGui::PopStyleColor();
    }

    ImGui::PopID();

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderBool(const FBoolProperty* BoolProperty, uint8* PointerToValue)
{
    bool HasChanged = false;

    bool Value = BoolProperty->GetPropertyValue(PointerToValue);
    if (ImGui::Checkbox("##Bool", &Value))
    {
        HasChanged = true;
        BoolProperty->SetPropertyValue(PointerToValue, Value);
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderByte(const FByteProperty* ByteProperty, uint8* PointerToValue)
{
    bool HasChanged = false;

    int Value = (int)ByteProperty->GetPropertyValue(PointerToValue);
    if (ImGui::InputInt("##Byte", &Value))
    {
        HasChanged = true;
        ByteProperty->SetPropertyValue(PointerToValue, static_cast<uint8>(Value));
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderInt8(const FInt8Property* Int8Property, uint8* PointerToValue)
{
    bool HasChanged = false;

    int Value = (int)Int8Property->GetPropertyValue(PointerToValue);
    if (ImGui::InputInt("##Int8", &Value))
    {
        HasChanged = true;
        Int8Property->SetPropertyValue(PointerToValue, static_cast<int8>(Value));
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderInt(const FIntProperty* IntProperty, uint8* PointerToValue)
{
    bool HasChanged = false;

    int Value = IntProperty->GetPropertyValue(PointerToValue);
    if (ImGui::InputInt("##Int", &Value))
    {
        HasChanged = true;
        IntProperty->SetPropertyValue(PointerToValue, Value);
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderInt64(const FInt64Property* Int64Property, uint8* PointerToValue)
{
    bool HasChanged = false;

    int Value = static_cast<int>(Int64Property->GetPropertyValue(PointerToValue));
    if (ImGui::InputInt("##UInt64", &Value))
    {
        HasChanged = true;
        Int64Property->SetPropertyValue(PointerToValue, static_cast<uint64>(Value));
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderUInt32(const FUInt32Property* UInt32Property, uint8* PointerToValue)
{
    bool HasChanged = false;

    int Value = static_cast<int>(UInt32Property->GetPropertyValue(PointerToValue));
    if (ImGui::InputInt("##UInt32", &Value))
    {
        HasChanged = true;
        UInt32Property->SetPropertyValue(PointerToValue, static_cast<uint32>(Value));
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderFloat(const FFloatProperty* FloatProperty, uint8* PointerToValue)
{
    bool HasChanged = false;

    float Value = FloatProperty->GetPropertyValue(PointerToValue);
    if (ImGui::InputFloat("##Float", &Value))
    {
        HasChanged = true;
        FloatProperty->SetPropertyValue(PointerToValue, Value);
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderDouble(const FDoubleProperty* DoubleProperty, uint8* PointerToValue)
{
    bool HasChanged = false;

    double Value = DoubleProperty->GetPropertyValue(PointerToValue);
    if (ImGui::InputDouble("##Double", &Value))
    {
        HasChanged = true;
        DoubleProperty->SetPropertyValue(PointerToValue, Value);
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderEnum(const FEnumProperty* EnumProperty, uint8* PointerToValue)
{
    return FCogWidgets::ComboboxEnum("##Enum", EnumProperty, PointerToValue);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderString(const FStrProperty* StrProperty, uint8* PointerToValue)
{
    FString Text;
    StrProperty->ExportTextItem_Direct(Text, PointerToValue, nullptr, nullptr, PPF_None, nullptr);

    static char Buffer[256] = "";

    const auto Str = StringCast<ANSICHAR>(*Text);
    ImStrncpy(Buffer, Str.Get(), IM_ARRAYSIZE(Buffer));

    if (ImGui::InputText("##String", Buffer, IM_ARRAYSIZE(Buffer)))
    {
        StrProperty->SetPropertyValue(PointerToValue, FString(Buffer));
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderName(const FNameProperty* NameProperty, uint8* PointerToValue)
{
    FString NameValue;
    NameProperty->ExportTextItem_Direct(NameValue, PointerToValue, nullptr, nullptr, PPF_None, nullptr);
    ImGui::BeginDisabled();
    ImGui::Text("%s", COG_TCHAR_TO_CHAR(*NameValue));
    ImGui::EndDisabled();

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderText(const FTextProperty* TextProperty, uint8* PointerToValue)
{
    FString Text;
    TextProperty->ExportTextItem_Direct(Text, PointerToValue, nullptr, nullptr, PPF_None, nullptr);
    ImGui::BeginDisabled();
    ImGui::Text("%s", TCHAR_TO_UTF8(*Text));
    ImGui::EndDisabled();

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderObject(UObject* Object, bool ShowChildren)
{
    if (Object == nullptr)
    {
        ImGui::BeginDisabled();
        ImGui::Text("NULL");
        ImGui::EndDisabled();

        return false;
    }

    ImGui::BeginDisabled();
    ImGui::Text("%s", COG_TCHAR_TO_CHAR(*Object->GetClass()->GetName()));
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("...", ImVec2(0, 0)))
    {
        SetInspectedObject(Object);
    }

    bool HasChanged = false;

    if (ShowChildren)
    {
        TArray<const FProperty*> Properties;
        for (TFieldIterator<FProperty> It(Object->GetClass()); It; ++It)
        {
            Properties.AddUnique(*It);
        }

        HasChanged |= RenderPropertyList(Properties, (uint8*)Object);

        ImGui::TreePop();
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderStruct(const FStructProperty* StructProperty, uint8* PointerToValue, bool ShowChildren)
{
    ImGui::BeginDisabled();
    ImGui::Text("%s", COG_TCHAR_TO_CHAR(*StructProperty->Struct->GetStructCPPName()));
    ImGui::EndDisabled();

    bool HasChanged = false;

    if (ShowChildren)
    {
        TArray<const FProperty*> Properties;
        for (TFieldIterator<FProperty> It(StructProperty->Struct); It; ++It)
        {
            Properties.AddUnique(*It);
        }

        HasChanged |= RenderPropertyList(Properties, PointerToValue);

        ImGui::TreePop();
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderClass(const FClassProperty* ClassProperty)
{
    if (ClassProperty->MetaClass == nullptr)
    {
        ImGui::BeginDisabled();
        ImGui::Text("NULL");
        ImGui::EndDisabled();
    }
    else
    {
        ImGui::BeginDisabled();
        ImGui::Text("%s", COG_TCHAR_TO_CHAR(*ClassProperty->MetaClass->GetName()));
        ImGui::EndDisabled();
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderInterface(const FInterfaceProperty* InterfaceProperty)
{
	const UClass* Class = InterfaceProperty->InterfaceClass;
    if (Class == nullptr)
    {
        ImGui::BeginDisabled();
        ImGui::Text("NULL");
        ImGui::EndDisabled();
    }
    else
    {
        ImGui::BeginDisabled();
        ImGui::Text("%s", COG_TCHAR_TO_CHAR(*Class->GetName()));
        ImGui::EndDisabled();
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderArray(const FArrayProperty* ArrayProperty, uint8* PointerToValue, bool ShowChildren)
{
    FScriptArrayHelper Helper(ArrayProperty, PointerToValue);
    const int32 Num = Helper.Num();

    ImGui::BeginDisabled();
    FString ElementPropertyName = ArrayProperty->Inner->GetClass()->GetName();
    if (const FStructProperty* StructProperty = CastField<FStructProperty>(ArrayProperty->Inner))
    {
        ElementPropertyName = StructProperty->Struct->GetStructCPPName();
    }
    ImGui::Text("%s [%d]", StringCast<ANSICHAR>(*ElementPropertyName).Get(), Num);
    ImGui::EndDisabled();

    bool HasChanged = false;

    if (ShowChildren)
    {
        for (int32 i = 0; i < Num; ++i)
        {
            ImGui::PushID(i);
            HasChanged |= RenderProperty(ArrayProperty->Inner, Helper.GetRawPtr(i), i);
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderSet(const FSetProperty* SetProperty, uint8* PointerToValue, bool ShowChildren)
{
    FScriptSetHelper Helper(SetProperty, PointerToValue);
    const int32 Num = Helper.Num();

    ImGui::BeginDisabled();
    FString ElementPropertyName = SetProperty->GetElementProperty()->GetClass()->GetName();
    if (const FStructProperty* StructProperty = CastField<FStructProperty>(SetProperty->GetElementProperty()))
    {
        ElementPropertyName = StructProperty->Struct->GetStructCPPName();
    }
    ImGui::Text("%s {%d}", StringCast<ANSICHAR>(*ElementPropertyName).Get(), Num);
    ImGui::EndDisabled();

    bool HasChanged = false;

    if (ShowChildren)
    {
        for (int32 i = 0; i < Num; ++i)
        {
            ImGui::PushID(i);
            HasChanged |= RenderProperty(SetProperty->GetElementProperty(), Helper.GetElementPtr(i), i);
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::RenderMap(const FMapProperty* MapProperty, uint8* PointerToValue, bool ShowChildren)
{
    FScriptMapHelper Helper(MapProperty, PointerToValue);
    const int32 Num = Helper.Num();

    ImGui::BeginDisabled();
    FString KeyPropertyName = MapProperty->GetKeyProperty()->GetClass()->GetName();
    if (const FStructProperty* StructProperty = CastField<FStructProperty>(MapProperty->GetKeyProperty()))
    {
        KeyPropertyName = StructProperty->Struct->GetStructCPPName();
    }
    FString ValuePropertyName = MapProperty->GetValueProperty()->GetClass()->GetName();
    if (const FStructProperty* StructProperty = CastField<FStructProperty>(MapProperty->GetValueProperty()))
    {
        ValuePropertyName = StructProperty->Struct->GetStructCPPName();
    }
    ImGui::Text("%s -> %s [%d]", StringCast<ANSICHAR>(*KeyPropertyName).Get(), StringCast<ANSICHAR>(*ValuePropertyName).Get(), Num);
    ImGui::EndDisabled();

    bool HasChanged = false;

    if (ShowChildren)
    {
        for (int32 i = 0; i < Num; ++i)
        {
            ImGui::PushID(i);
            // @todo: refactor this so it's better?
            HasChanged |= RenderProperty(MapProperty->GetKeyProperty(), Helper.GetKeyPtr(i), i, "Key");
            HasChanged |= RenderProperty(MapProperty->GetValueProperty(), Helper.GetValuePtr(i), i, "Value");
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogEngineWindow_Inspector::HasPropertyAnyChildren(const FProperty* Property, uint8* PointerToValue)
{
    if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
    {
        const TFieldIterator<FProperty> It(StructProperty->Struct);
        return It ? true : false;
    }

    if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
    {
	    const FScriptArrayHelper Helper(ArrayProperty, PointerToValue);
        const int32 Num = Helper.Num();
        if (Num == 0)
        { return false; }

        return true;
    }

    if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
    {
        const FScriptSetHelper Helper(SetProperty, PointerToValue);
        const int32 Num = Helper.Num();
        if (Num == 0)
        {
            return false;
        }

        return true;
    }

    if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
    {
        const FScriptMapHelper Helper(MapProperty, PointerToValue);
        const int32 Num = Helper.Num();
        if (Num == 0)
        {
            return false;
        }

        return true;
    }

    if (const FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
    { return false; }

    if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
    {
	    const UObject* ReferencedObject = ObjectProperty->GetObjectPropertyValue(PointerToValue);
        if (ReferencedObject == nullptr)
        { return false; }

        return true;
    }

    return false;
}



