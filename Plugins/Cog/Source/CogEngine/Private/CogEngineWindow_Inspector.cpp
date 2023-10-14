#include "CogEngineWindow_Inspector.h"

#include "CogWindowWidgets.h"
#include "Containers/SortedMap.h"
#include "Engine/Engine.h"
#include "imgui_internal.h"
#include "UObject/EnumProperty.h"
#include "UObject/TextProperty.h"
#include "UObject/UnrealType.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Inspector::RenderHelp()
{
    ImGui::Text(
        "This window can be used to inspect the properties of a UObject. "
    );
}

//--------------------------------------------------------------------------------------------------------------------------
UCogEngineWindow_Inspector::UCogEngineWindow_Inspector()
{
    bHasMenu = true;
    PropertyGridFlags = ECogEngineInspectorFlags_ShowCategories
                      | ECogEngineInspectorFlags_SortByName
                      | ECogEngineInspectorFlags_ShowDisplayName
                      | ECogEngineInspectorFlags_DisplaySearch
                      | ECogEngineInspectorFlags_DisplayOptions;
}

//--------------------------------------------------------------------------------------------------------------------------
FString UCogEngineWindow_Inspector::GetTitle() const
{
    return FString::Printf(TEXT("%s: %s###Inspector"),
        *GetName(),
        InspectedObject != nullptr ? *InspectedObject->GetName() : TEXT("none"));
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Inspector::InspectObject(const UObject* Object)
{
    InspectedObject = Object;
    SetIsVisible(true);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Inspector::RenderContent()
{
    Super::RenderContent();

    if (InspectedObject == nullptr)
    {
        InspectedObject = GetSelection();
    }

    if (InspectedObject.IsValid() == false)
    {
        return;
    }

    RenderMenu(*this, PropertyFilter, PropertyGridFlags);
    RenderInspector(*this, InspectedObject.Get(), PropertyFilter, PropertyGridFlags);
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogEngineWindow_Inspector::RenderMenu(FCogEngineInspectorHost& Host, ImGuiTextFilter& Filter, ECogEngineInspectorFlags& Flags)
{
    bool ShowDisplayName =      (Flags & ECogEngineInspectorFlags_ShowDisplayName) != 0;
    bool ShowRowBackground =    (Flags & ECogEngineInspectorFlags_ShowRowBackground) != 0;
    bool ShowBorders =          (Flags & ECogEngineInspectorFlags_ShowBorders) != 0;
    bool ShowCategories =       (Flags & ECogEngineInspectorFlags_ShowCategories) != 0;
    bool SortByName =           (Flags & ECogEngineInspectorFlags_SortByName) != 0;

    Flags &= ~ECogEngineInspectorFlags_ShowDisplayName;
    Flags &= ~ECogEngineInspectorFlags_ShowRowBackground;
    Flags &= ~ECogEngineInspectorFlags_ShowBorders;
    Flags &= ~ECogEngineInspectorFlags_ShowCategories;
    Flags &= ~ECogEngineInspectorFlags_SortByName;
    Flags &= ~ECogEngineInspectorFlags_CollapseAllCategories;
    Flags &= ~ECogEngineInspectorFlags_ExpandAllCategories;

    if (ImGui::BeginMenuBar())
    {
        if (Flags & ECogEngineInspectorFlags_DisplayOptions)
        {
            if (ImGui::BeginMenu("Options"))
            {
                if (ImGui::MenuItem("Collapse all categories"))
                {
                    Flags |= ECogEngineInspectorFlags_CollapseAllCategories;
                }

                if (ImGui::MenuItem("Expand all categories"))
                {
                    Flags |= ECogEngineInspectorFlags_ExpandAllCategories;
                }

                ImGui::Separator();

                ImGui::Checkbox("Show background", &ShowRowBackground);
                ImGui::Checkbox("Show borders", &ShowBorders);
#if WITH_EDITORONLY_DATA
                ImGui::Checkbox("Show display name", &ShowDisplayName);
                ImGui::Checkbox("Show categories", &ShowCategories);
#endif  // WITH_EDITORONLY_DATA
                ImGui::Checkbox("Sort by name", &SortByName);

                ImGui::Separator();

                ImGui::MenuItem("Help");
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Tips:");
                    ImGui::TextUnformatted(" - Press [CTRL] over a property to see more informations");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }

                ImGui::EndMenu();
            }
        }

        if (Flags & ECogEngineInspectorFlags_DisplaySearch)
        {
            FCogWindowWidgets::MenuSearchBar(Filter);
        }

        ImGui::EndMenuBar();
    }

    Flags |= ShowDisplayName ? ECogEngineInspectorFlags_ShowDisplayName : 0;
    Flags |= ShowRowBackground ? ECogEngineInspectorFlags_ShowRowBackground : 0;
    Flags |= ShowBorders ? ECogEngineInspectorFlags_ShowBorders : 0;
    Flags |= ShowCategories ? ECogEngineInspectorFlags_ShowCategories : 0;
    Flags |= SortByName ? ECogEngineInspectorFlags_SortByName : 0;
}

//--------------------------------------------------------------------------------------------------------------------------

bool UCogEngineWindow_Inspector::RenderInspector(FCogEngineInspectorHost& Host, const UObject* Object, ImGuiTextFilter& Filter, ECogEngineInspectorFlags Flags)
{
    if (Object == nullptr)
    {
        return false;
    }

    TArray<const FProperty*> Properties;
    for (TFieldIterator<FProperty> It(Object->GetClass()); It; ++It)
    {
        const FProperty* Property = *It;

        if (Filter.IsActive() == false || Filter.PassFilter(TCHAR_TO_ANSI(*Property->GetName())))
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

    if ((Flags & ECogEngineInspectorFlags_ShowCategories) != 0)
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
            if ((Flags & ECogEngineInspectorFlags_ExpandAllCategories) != 0)
            {
                ImGui::SetNextItemOpen(true);
            }
            else if ((Flags & ECogEngineInspectorFlags_CollapseAllCategories) != 0)
            {
                ImGui::SetNextItemOpen(false);
            }

            if (ImGui::CollapsingHeader(TCHAR_TO_ANSI(*Entry.Key), nullptr, ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (RenderBegin(Host, Flags))
                {
                    HasChanged |= RenderPropertyList(Host, Entry.Value, (uint8*)Object, Flags);
                    RenderEnd(Host);
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
        if (RenderBegin(Host, Flags))
        {
            HasChanged = RenderPropertyList(Host, Properties, (uint8*)Object, Flags);
            RenderEnd(Host);
        }
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderBegin(FCogEngineInspectorHost& Host, ECogEngineInspectorFlags Flags)
{
    FCogWindowWidgets::PushStyleCompact();

    ImGuiTableFlags TableFlags = ImGuiTableFlags_Resizable;
    if ((Flags & ECogEngineInspectorFlags_ShowBorders) != 0)
    {
        TableFlags |= ImGuiTableFlags_Borders;
    }
    else
    {
        TableFlags |= ImGuiTableFlags_NoBordersInBodyUntilResize;
    }

    if ((Flags & ECogEngineInspectorFlags_ShowRowBackground) != 0)
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
void UCogEngineWindow_Inspector::RenderEnd(FCogEngineInspectorHost& Host)
{
    ImGui::EndTable();
    FCogWindowWidgets::PopStyleCompact();
}

//--------------------------------------------------------------------------------------------------------------------------
FString UCogEngineWindow_Inspector::GetPropertyName(const FProperty& Property, ECogEngineInspectorFlags Flags)
{
#if WITH_EDITORONLY_DATA

    if (((Flags & ECogEngineInspectorFlags_ShowDisplayName) != 0) && Property.GetDisplayNameText().IsEmpty() == false)
    {
        return Property.GetDisplayNameText().ToString();
    }

    return Property.GetAuthoredName();

#else

    return Property.GetAuthoredName();

#endif
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderPropertyList(FCogEngineInspectorHost& Host,
    TArray<const FProperty*>& Properties,
    uint8* PointerToValue,
    ECogEngineInspectorFlags Flags)
{
    if ((Flags & ECogEngineInspectorFlags_SortByName) != 0)
    {
        Properties.Sort([Flags](const FProperty& Lhs, const FProperty& Rhs) { return GetPropertyName(Lhs, Flags) < GetPropertyName(Rhs, Flags); });
    }

    bool HasChanged = false;
    int index = 0;
    for (const FProperty* Property : Properties)
    {
        ImGui::PushID(index++);
        uint8* InnerPointerToValue = Property->ContainerPtrToValuePtr<uint8>(PointerToValue);
        HasChanged |= RenderProperty(Host, Property, InnerPointerToValue, -1, Flags);
        ImGui::PopID();
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderProperty(FCogEngineInspectorHost& Host,
    const FProperty* Property,
    uint8* PointerToValue,
    int IndexInArray,
    ECogEngineInspectorFlags Flags)
{
    bool HasChanged = false;

    ImGui::PushID(PointerToValue);

    //--------------------------------------------------------------------------------------
    // Render Property Name
    //--------------------------------------------------------------------------------------
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();

    FString Name;
    if (IndexInArray != -1)
    {
        Name = FString::Printf(TEXT("[%d]"), IndexInArray);
    }
    else
    {
        Name = GetPropertyName(*Property, Flags);
    }

    bool ShowChildren = false;
    if (HasPropertyAnyChildren(Host, Property, PointerToValue))
    {
        ShowChildren = ImGui::TreeNodeEx(TCHAR_TO_ANSI(*Name), ImGuiTreeNodeFlags_SpanFullWidth);
    }
    else
    {
        ImGui::TreeNodeEx(TCHAR_TO_ANSI(*Name), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
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
            ImGui::Text("Name:");
            ImGui::TableNextColumn();
            ImGui::Text("%s", TCHAR_TO_ANSI(*Property->GetName()));

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Type:");
            ImGui::TableNextColumn();
            ImGui::Text("%s", TCHAR_TO_ANSI(*Property->GetClass()->GetName()));

#if WITH_EDITORONLY_DATA
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("DisplayName:");
            ImGui::TableNextColumn();
            ImGui::Text(TCHAR_TO_ANSI(*Property->GetDisplayNameText().ToString()));
#endif  // WITH_EDITORONLY_DATA

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("FullName:");
            ImGui::TableNextColumn();
            ImGui::Text("%s", TCHAR_TO_ANSI(*Property->GetFullName()));

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
                ImGui::Text(TCHAR_TO_ANSI(*Property->GetToolTipText(false).ToString()));
            }
#endif  // WITH_EDITORONLY_DATA

            ImGui::EndTable();
        }

        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
        
#if WITH_EDITORONLY_DATA
    if (Property->HasMetaData("Tooltip"))
    {
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary))
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);

            ImGui::Text(TCHAR_TO_ANSI(*GetPropertyName(*Property, Flags)));
            ImGui::Separator();
            ImGui::Text(TCHAR_TO_ANSI(*Property->GetToolTipText(false).ToString()));

            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
#endif  // WITH_EDITORONLY_DATA

    //--------------------------------------------------------------------------------------
    // Render Property Value
    //--------------------------------------------------------------------------------------

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);

    if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
    {
        HasChanged = RenderBool(Host, BoolProperty, PointerToValue);
    }
    else if (const FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
    {
        HasChanged = RenderByte(Host, ByteProperty, PointerToValue);
    }
    else if (const FInt8Property* Int8Property = CastField<FInt8Property>(Property))
    {
        HasChanged = RenderInt8(Host, Int8Property, PointerToValue);
    }
    else if (const FIntProperty* IntProperty = CastField<FIntProperty>(Property))
    {
        HasChanged = RenderInt(Host, IntProperty, PointerToValue);
    }
    else if (const FUInt32Property* UInt32Property = CastField<FUInt32Property>(Property))
    {
        HasChanged = RenderUInt32(Host, UInt32Property, PointerToValue);
    }
    else if (const FInt64Property* Int64Property = CastField<FInt64Property>(Property))
    {
        HasChanged = RenderInt64(Host, Int64Property, PointerToValue);
    }
    else if (const FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
    {
        HasChanged = RenderFloat(Host, FloatProperty, PointerToValue);
    }
    else if (const FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(Property))
    {
        HasChanged = RenderDouble(Host, DoubleProperty, PointerToValue);
    }
    else if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
    {
        HasChanged = RenderEnum(Host, EnumProperty, PointerToValue);
    }
    else if (const FStrProperty* StrProperty = CastField<FStrProperty>(Property))
    {
        HasChanged = RenderString(Host, StrProperty, PointerToValue);
    }
    else if (const FNameProperty* NameProperty = CastField<FNameProperty>(Property))
    {
        HasChanged = RenderName(Host, NameProperty, PointerToValue);
    }
    else if (const FTextProperty* TextProperty = CastField<FTextProperty>(Property))
    {
        HasChanged = RenderText(Host, TextProperty, PointerToValue);
    }
    else if (const FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
    {
        HasChanged = RenderClass(Host, ClassProperty);
    }
    else if (const FInterfaceProperty* InterfaceProperty = CastField<FInterfaceProperty>(Property))
    {
        HasChanged = RenderInterface(Host, InterfaceProperty);
    }
    else if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
    {
        HasChanged = RenderStruct(Host, StructProperty, PointerToValue, ShowChildren, Flags);
    }
    else if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
    {
        UObject* ReferencedObject = ObjectProperty->GetObjectPropertyValue(PointerToValue);
        HasChanged = RenderObject(Host, ReferencedObject, ShowChildren, Flags);
    }
    else if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
    {
        HasChanged = RenderArray(Host, ArrayProperty, PointerToValue, ShowChildren, Flags);
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
        ImGui::Text("Unmanaged Type: %s", TCHAR_TO_ANSI(*Property->GetClass()->GetName()));
        ImGui::PopStyleColor();
    }

    ImGui::PopID();

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderBool(FCogEngineInspectorHost& Host, const FBoolProperty* BoolProperty, uint8* PointerToValue)
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
bool UCogEngineWindow_Inspector::RenderByte(FCogEngineInspectorHost& Host, const FByteProperty* ByteProperty, uint8* PointerToValue)
{
    bool HasChanged = false;

    int Value = (int)ByteProperty->GetPropertyValue(PointerToValue);
    if (ImGui::InputInt("##Byte", &Value))
    {
        HasChanged = true;
        ByteProperty->SetPropertyValue(PointerToValue, (uint8)Value);
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderInt8(FCogEngineInspectorHost& Host, const FInt8Property* Int8Property, uint8* PointerToValue)
{
    bool HasChanged = false;

    int Value = (int)Int8Property->GetPropertyValue(PointerToValue);
    if (ImGui::InputInt("##Int8", &Value))
    {
        HasChanged = true;
        Int8Property->SetPropertyValue(PointerToValue, (int8)Value);
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderInt(FCogEngineInspectorHost& Host, const FIntProperty* IntProperty, uint8* PointerToValue)
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
bool UCogEngineWindow_Inspector::RenderInt64(FCogEngineInspectorHost& Host, const FInt64Property* Int64Property, uint8* PointerToValue)
{
    bool HasChanged = false;

    int Value = (int)Int64Property->GetPropertyValue(PointerToValue);
    if (ImGui::InputInt("##UInt64", &Value))
    {
        HasChanged = true;
        Int64Property->SetPropertyValue(PointerToValue, (uint64)Value);
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderUInt32(FCogEngineInspectorHost& Host, const FUInt32Property* UInt32Property, uint8* PointerToValue)
{
    bool HasChanged = false;

    int Value = (int)UInt32Property->GetPropertyValue(PointerToValue);
    if (ImGui::InputInt("##UInt32", &Value))
    {
        HasChanged = true;
        UInt32Property->SetPropertyValue(PointerToValue, (uint32)Value);
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderFloat(FCogEngineInspectorHost& Host, const FFloatProperty* FloatProperty, uint8* PointerToValue)
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
bool UCogEngineWindow_Inspector::RenderDouble(FCogEngineInspectorHost& Host, const FDoubleProperty* DoubleProperty, uint8* PointerToValue)
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
bool UCogEngineWindow_Inspector::RenderEnum(FCogEngineInspectorHost& Host, const FEnumProperty* EnumProperty, uint8* PointerToValue)
{
    return FCogWindowWidgets::ComboboxEnum("##Enum", EnumProperty, PointerToValue);
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderString(FCogEngineInspectorHost& Host, const FStrProperty* StrProperty, uint8* PointerToValue)
{
    FString Text;
    StrProperty->ExportTextItem_Direct(Text, PointerToValue, nullptr, nullptr, PPF_None, nullptr);

    static char Buffer[256] = "";

    const char* Str = TCHAR_TO_ANSI(*Text);
    ImStrncpy(Buffer, Str, IM_ARRAYSIZE(Buffer));

    if (ImGui::InputText("##String", Buffer, IM_ARRAYSIZE(Buffer)))
    {
        StrProperty->SetPropertyValue(PointerToValue, FString(Buffer));
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderName(FCogEngineInspectorHost& Host, const FNameProperty* NameProperty, uint8* PointerToValue)
{
    FString Name;
    NameProperty->ExportTextItem_Direct(Name, PointerToValue, nullptr, nullptr, PPF_None, nullptr);
    ImGui::BeginDisabled();
    ImGui::Text("%s", TCHAR_TO_ANSI(*Name));
    ImGui::EndDisabled();

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderText(FCogEngineInspectorHost& Host, const FTextProperty* TextProperty, uint8* PointerToValue)
{
    FString Text;
    TextProperty->ExportTextItem_Direct(Text, PointerToValue, nullptr, nullptr, PPF_None, nullptr);
    ImGui::BeginDisabled();
    ImGui::Text("%s", TCHAR_TO_ANSI(*Text));
    ImGui::EndDisabled();

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderObject(FCogEngineInspectorHost& Host, UObject* Object, bool ShowChildren, ECogEngineInspectorFlags Flags)
{
    if (Object == nullptr)
    {
        ImGui::BeginDisabled();
        ImGui::Text("NULL");
        ImGui::EndDisabled();

        return false;
    }

    ImGui::BeginDisabled();
    ImGui::Text("%s", TCHAR_TO_ANSI(*Object->GetClass()->GetName()));
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("...", ImVec2(0, 0)))
    {
        Host.InspectObject(Object);
    }

    bool HasChanged = false;

    if (ShowChildren)
    {
        TArray<const FProperty*> Properties;
        for (TFieldIterator<FProperty> It(Object->GetClass()); It; ++It)
        {
            Properties.AddUnique(*It);
        }

        HasChanged |= RenderPropertyList(Host, Properties, (uint8*)Object, Flags);

        ImGui::TreePop();
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderStruct(FCogEngineInspectorHost& Host,
    const FStructProperty* StructProperty,
    uint8* PointerToValue,
    bool ShowChildren,
    ECogEngineInspectorFlags Flags)
{
    ImGui::BeginDisabled();
    ImGui::Text("%s", TCHAR_TO_ANSI(*StructProperty->Struct->GetClass()->GetName()));
    ImGui::EndDisabled();

    bool HasChanged = false;

    if (ShowChildren)
    {
        TArray<const FProperty*> Properties;
        for (TFieldIterator<FProperty> It(StructProperty->Struct); It; ++It)
        {
            Properties.AddUnique(*It);
        }

        HasChanged |= RenderPropertyList(Host, Properties, PointerToValue, Flags);

        ImGui::TreePop();
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderClass(FCogEngineInspectorHost& Host, const FClassProperty* ClassProperty)
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
        ImGui::Text("%s", TCHAR_TO_ANSI(*ClassProperty->MetaClass->GetName()));
        ImGui::EndDisabled();
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderInterface(FCogEngineInspectorHost& Host, const FInterfaceProperty* InterfaceProperty)
{
    UClass* Class = InterfaceProperty->InterfaceClass;
    if (Class == nullptr)
    {
        ImGui::BeginDisabled();
        ImGui::Text("NULL");
        ImGui::EndDisabled();
    }
    else
    {
        ImGui::BeginDisabled();
        ImGui::Text("%s", TCHAR_TO_ANSI(*Class->GetName()));
        ImGui::EndDisabled();
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::RenderArray(FCogEngineInspectorHost& Host,
    const FArrayProperty* ArrayProperty,
    uint8* PointerToValue,
    bool ShowChildren,
    ECogEngineInspectorFlags Flags)
{
    FScriptArrayHelper Helper(ArrayProperty, PointerToValue);
    int32 Num = Helper.Num();

    ImGui::BeginDisabled();
    ImGui::Text("%s [%d]", TCHAR_TO_ANSI(*ArrayProperty->Inner->GetClass()->GetName()), Num);
    ImGui::EndDisabled();

    bool HasChanged = false;

    if (ShowChildren)
    {
        for (int32 i = 0; i < Num; ++i)
        {
            ImGui::PushID(i);
            HasChanged |= RenderProperty(Host, ArrayProperty->Inner, Helper.GetRawPtr(i), i, Flags);
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    return HasChanged;
}

//--------------------------------------------------------------------------------------------------------------------------
bool UCogEngineWindow_Inspector::HasPropertyAnyChildren(FCogEngineInspectorHost& Host, const FProperty* Property, uint8* PointerToValue)
{
    if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
    {
        for (TFieldIterator<FProperty> It(StructProperty->Struct); It; ++It)
        {
            return true;
        }

        return false;
    }
    else if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
    {
        FScriptArrayHelper Helper(ArrayProperty, PointerToValue);
        int32 Num = Helper.Num();
        if (Num == 0)
        {
            return false;
        }

        return true;
    }
    else if (const FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
    {
        return false;
    }
    else if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
    {
        UObject* ReferencedObject = ObjectProperty->GetObjectPropertyValue(PointerToValue);
        if (ReferencedObject == nullptr)
        {
            return false;
        }

        return true;
    }

    return false;
}
