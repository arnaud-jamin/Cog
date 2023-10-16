#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "imgui.h"
#include "CogEngineWindow_Inspector.generated.h"

using FCogEngineInspectorApplyFunction = TFunction<void(UObject*)>;

UCLASS(Config = Cog)
class COGENGINE_API UCogEngineWindow_Inspector : public UCogWindow
{
    GENERATED_BODY()

public:
    
    UCogEngineWindow_Inspector();

    virtual UObject* GetInspectedObject() const { return InspectedObject.Get(); }

    virtual void SetInspectedObject(UObject* Object);

    virtual void AddFavorite(UObject* Object);

    virtual void AddFavorite(UObject* Object, FCogEngineInspectorApplyFunction ApplyFunction);

    UPROPERTY(Config)
    bool bSyncWithSelection = true;

    UPROPERTY(Config)
    bool bShowDisplayName = true;

    UPROPERTY(Config)
    bool bShowRowBackground = true;

    UPROPERTY(Config)
    bool bShowBorders = false;

    UPROPERTY(Config)
    bool bShowCategories = true;

    UPROPERTY(Config)
    bool bSortByName = true;

protected:

    virtual void RenderHelp() override;

    virtual void RenderContent() override;

    virtual void RenderMenu();

    virtual bool RenderInspector();

    virtual bool RenderBegin();

    virtual void RenderEnd();

    virtual bool HasPropertyAnyChildren(const FProperty* Property, uint8* PointerToValue);

    virtual bool RenderPropertyList(TArray<const FProperty*>& Properties, uint8* PointerToValue);

    virtual bool RenderProperty(const FProperty* Property, uint8* PointerToValue, int IndexInArray);

    virtual bool RenderBool(const FBoolProperty* BoolProperty, uint8* PointerToValue);

    virtual bool RenderByte(const FByteProperty* ByteProperty, uint8* PointerToValue);

    virtual bool RenderInt8(const FInt8Property* Int8Property, uint8* PointerToValue);

    virtual bool RenderInt(const FIntProperty* IntProperty, uint8* PointerToValue);

    virtual bool RenderInt64(const FInt64Property* Int64Property, uint8* PointerToValue);

    virtual bool RenderUInt32(const FUInt32Property* UInt32Property, uint8* PointerToValue);

    virtual bool RenderFloat(const FFloatProperty* FloatProperty, uint8* PointerToValue);

    virtual bool RenderDouble(const FDoubleProperty* DoubleProperty, uint8* PointerToValue);

    virtual bool RenderEnum(const FEnumProperty* EnumProperty, uint8* PointerToValue);

    virtual bool RenderString(const FStrProperty* StrProperty, uint8* PointerToValue);

    virtual bool RenderName(const FNameProperty* NameProperty, uint8* PointerToValue);

    virtual bool RenderText(const FTextProperty* TextProperty, uint8* PointerToValue);

    virtual bool RenderClass(const FClassProperty* ClassProperty);

    virtual bool RenderInterface(const FInterfaceProperty* InterfaceProperty);

    virtual bool RenderStruct(const FStructProperty* StructProperty, uint8* PointerToValue, bool ShowChildren);

    virtual bool RenderObject(UObject* Object, bool ShowChildren);

    virtual bool RenderArray(const FArrayProperty* ArrayProperty, uint8* PointerToValue, bool ShowChildren);

    virtual FString GetPropertyName(const FProperty& Property);

    FCogEngineInspectorApplyFunction FindObjectApplyFunction(const UObject* Object) const;

    struct Favorite
    {
        TWeakObjectPtr<UObject> Object = nullptr;

        FCogEngineInspectorApplyFunction ApplyFunction = nullptr;
    };

    TWeakObjectPtr<UObject> InspectedObject = nullptr;
    
    ImGuiTextFilter Filter;

    bool bExpandAllCategories = false;

    bool bCollapseAllCategories = false;

    TArray<Favorite> Favorites;

    TArray<TWeakObjectPtr<UObject>> History;

    int32 HistoryIndex = INDEX_NONE;
};
