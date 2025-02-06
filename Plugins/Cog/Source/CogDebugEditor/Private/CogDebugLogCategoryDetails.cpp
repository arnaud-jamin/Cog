#include "CogDebugLogCategoryDetails.h"

#include "CogCommonLog.h"
#include "CogDebugLog.h"
#include "DetailWidgetRow.h"
#include "Editor.h"
#include "PropertyHandle.h"
#include "SCogDebugLogCategoryWidget.h"

#define LOCTEXT_NAMESPACE "AttributeDetailsCustomization"

//--------------------------------------------------------------------------------------------------------------------------
TSharedRef<IPropertyTypeCustomization> FCogLogCategoryDetails::MakeInstance()
{
    return MakeShareable(new FCogLogCategoryDetails());
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogLogCategoryDetails::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    NameProperty = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCogLogCategory, Name));

    PropertyOptions.Empty();
    PropertyOptions.Add(MakeShareable(new FString("None")));
    for (auto& Entry : FCogDebugLog::GetLogCategories())
    {
        PropertyOptions.Add(MakeShareable(new FString(Entry.Value.LogCategory->GetCategoryName().ToString())));
    }

    const FString& FilterMetaStr = StructPropertyHandle->GetProperty()->GetMetaData(TEXT("FilterMetaTag"));

    FName Value;
    if (NameProperty.IsValid())
    {
        NameProperty->GetValue(Value);
    }

    HeaderRow.
        NameContent()
        [
            StructPropertyHandle->CreatePropertyNameWidget()
        ]
    .ValueContent()
        .MinDesiredWidth(500)
        .MaxDesiredWidth(4096)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            //.FillWidth(1.0f)
            .HAlign(HAlign_Fill)
            .Padding(0.f, 0.f, 2.f, 0.f)
            [
                SNew(SCogLogCategoryWidget)
                .OnLogCategoryChanged(this, &FCogLogCategoryDetails::OnLogCategoryChanged)
                .DefaultName(Value)
                .FilterMetaData(FilterMetaStr)
            ]
        ];
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogLogCategoryDetails::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogLogCategoryDetails::OnLogCategoryChanged(const FName SelectedName) const
{
    if (NameProperty.IsValid())
    {
        NameProperty->SetValue(SelectedName);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
