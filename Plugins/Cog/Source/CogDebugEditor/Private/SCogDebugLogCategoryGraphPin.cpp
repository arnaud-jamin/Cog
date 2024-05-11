#include "SCogDebugLogCategoryGraphPin.h"

#include "CogCommonLog.h"
#include "ScopedTransaction.h"
#include "SCogDebugLogCategoryWidget.h"
#include "UObject/UObjectIterator.h"
#include "Widgets/SBoxPanel.h"

#define LOCTEXT_NAMESPACE "K2Node"

//--------------------------------------------------------------------------------------------------------------------------
void SCogLogCategoryGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
    SGraphPin::Construct( SGraphPin::FArguments(), InGraphPinObj );
    LastSelectedName = FName();
}

//--------------------------------------------------------------------------------------------------------------------------
TSharedRef<SWidget>	SCogLogCategoryGraphPin::GetDefaultValueWidget()
{
    // Parse out current default value	
    FString DefaultString = GraphPinObj->GetDefaultAsString();
    FCogLogCategory DefaultLogCategory;

    UScriptStruct* PinLiteralStructType = FCogLogCategory::StaticStruct();
    if (!DefaultString.IsEmpty())
    {
        PinLiteralStructType->ImportText(*DefaultString, &DefaultLogCategory, nullptr, EPropertyPortFlags::PPF_SerializedAsImportText, GError, PinLiteralStructType->GetName(), true);
    }

    //Create widget
    return SNew(SVerticalBox)
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SCogLogCategoryWidget)
            .OnLogCategoryChanged(this, &SCogLogCategoryGraphPin::OnLogCategoryChanged)
            .DefaultName(DefaultLogCategory.Name)
            .Visibility(this, &SGraphPin::GetDefaultValueVisibility)
            .IsEnabled(this, &SCogLogCategoryGraphPin::GetDefaultValueIsEnabled)
        ];
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogLogCategoryGraphPin::OnLogCategoryChanged(FName SelectedName)
{
    FString FinalValue;
    FCogLogCategory NewLogCategoryStruct;
    NewLogCategoryStruct.Name = SelectedName;

    FCogLogCategory::StaticStruct()->ExportText(FinalValue, &NewLogCategoryStruct, &NewLogCategoryStruct, nullptr, EPropertyPortFlags::PPF_SerializedAsImportText, nullptr);

    if (FinalValue != GraphPinObj->GetDefaultAsString())
    {
        const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangePinValue", "Change Pin Value"));
        GraphPinObj->Modify();
        GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, FinalValue);
    }

    LastSelectedName = SelectedName;
}

#undef LOCTEXT_NAMESPACE
