#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "imgui.h"
#include "CogEngineWindow_Inspector.generated.h"

typedef int32 ECogEngineInspectorFlags;

enum ECogEngineInspectorFlags_
{
    ECogEngineInspectorFlags_None                   = 0,
    ECogEngineInspectorFlags_ShowDisplayName        = 1 << 0,
    ECogEngineInspectorFlags_ShowRowBackground      = 1 << 1,
    ECogEngineInspectorFlags_ShowBorders            = 1 << 2,
    ECogEngineInspectorFlags_ShowCategories         = 1 << 3,
    ECogEngineInspectorFlags_SortByName             = 1 << 4,
    ECogEngineInspectorFlags_CollapseAllCategories  = 1 << 5,
    ECogEngineInspectorFlags_ExpandAllCategories    = 1 << 6,
    ECogEngineInspectorFlags_DisplaySearch          = 1 << 7,
    ECogEngineInspectorFlags_DisplayOptions         = 1 << 8,
};

class FCogEngineInspectorHost
{
public:
    virtual void InspectObject(const UObject* Object) = 0;
};

UCLASS(Config = Cog)
class COGENGINE_API UCogEngineWindow_Inspector : public UCogWindow, public FCogEngineInspectorHost
{
    GENERATED_BODY()

public:
    
    UCogEngineWindow_Inspector();

    virtual void InspectObject(const UObject* Object) override;

    static void RenderMenu(FCogEngineInspectorHost& Host, ImGuiTextFilter& Filter, ECogEngineInspectorFlags& Flags);
    static bool RenderInspector(FCogEngineInspectorHost& Host, const UObject* Object, ImGuiTextFilter& Filter, ECogEngineInspectorFlags Flags);
    static bool RenderBegin(FCogEngineInspectorHost& Host, ECogEngineInspectorFlags Flags);
    static void RenderEnd(FCogEngineInspectorHost& Host);
    static bool HasPropertyAnyChildren(FCogEngineInspectorHost& Host, const FProperty* Property, uint8* PointerToValue);
    static bool RenderPropertyList(FCogEngineInspectorHost& Host, TArray<const FProperty*>& Properties, uint8* PointerToValue, ECogEngineInspectorFlags Flags);
    static bool RenderProperty(FCogEngineInspectorHost& Host, const FProperty* Property, uint8* PointerToValue, int IndexInArray, ECogEngineInspectorFlags Flags);
    static bool RenderBool(FCogEngineInspectorHost& Host, const FBoolProperty* BoolProperty, uint8* PointerToValue);
    static bool RenderByte(FCogEngineInspectorHost& Host, const FByteProperty* ByteProperty, uint8* PointerToValue);
    static bool RenderInt8(FCogEngineInspectorHost& Host, const FInt8Property* Int8Property, uint8* PointerToValue);
    static bool RenderInt(FCogEngineInspectorHost& Host, const FIntProperty* IntProperty, uint8* PointerToValue);
    static bool RenderInt64(FCogEngineInspectorHost& Host, const FInt64Property* Int64Property, uint8* PointerToValue);
    static bool RenderUInt32(FCogEngineInspectorHost& Host, const FUInt32Property* UInt32Property, uint8* PointerToValue);
    static bool RenderFloat(FCogEngineInspectorHost& Host, const FFloatProperty* FloatProperty, uint8* PointerToValue);
    static bool RenderDouble(FCogEngineInspectorHost& Host, const FDoubleProperty* DoubleProperty, uint8* PointerToValue);
    static bool RenderEnum(FCogEngineInspectorHost& Host, const FEnumProperty* EnumProperty, uint8* PointerToValue);
    static bool RenderString(FCogEngineInspectorHost& Host, const FStrProperty* StrProperty, uint8* PointerToValue);
    static bool RenderName(FCogEngineInspectorHost& Host, const FNameProperty* NameProperty, uint8* PointerToValue);
    static bool RenderText(FCogEngineInspectorHost& Host, const FTextProperty* TextProperty, uint8* PointerToValue);
    static bool RenderClass(FCogEngineInspectorHost& Host, const FClassProperty* ClassProperty);
    static bool RenderInterface(FCogEngineInspectorHost& Host, const FInterfaceProperty* InterfaceProperty);
    static bool RenderStruct(FCogEngineInspectorHost& Host, const FStructProperty* StructProperty, uint8* PointerToValue, bool ShowChildren, ECogEngineInspectorFlags Flags);
    static bool RenderObject(FCogEngineInspectorHost& Host, UObject* Object, bool ShowChildren, ECogEngineInspectorFlags Flags);
    static bool RenderArray(FCogEngineInspectorHost& Host, const FArrayProperty* ArrayProperty, uint8* PointerToValue, bool ShowChildren, ECogEngineInspectorFlags Flags);
    static FString GetPropertyName(const FProperty& Property, ECogEngineInspectorFlags Flags);

protected:

    virtual FString GetTitle() const override;

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    TWeakObjectPtr<const UObject> InspectedObject = nullptr;
    
    ImGuiTextFilter PropertyFilter;

    UPROPERTY(Config)
    int32 PropertyGridFlags = 0;
};
