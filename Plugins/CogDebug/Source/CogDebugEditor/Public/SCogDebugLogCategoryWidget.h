#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SComboButton.h"

class SCogLogCategoryWidget : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnLogCategoryChanged, FName)

	SLATE_BEGIN_ARGS(SCogLogCategoryWidget)
	: _FilterMetaData()
	, _DefaultName()
	{}
	SLATE_ARGUMENT(FString, FilterMetaData)
	SLATE_ARGUMENT(FName, DefaultName)
	SLATE_EVENT(FOnLogCategoryChanged, OnLogCategoryChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);	

private:
	TSharedRef<SWidget> GenerateLogCategoryPicker();
	FText GetSelectedValueAsText() const;
	void OnItemPicked(FName Name);

	FOnLogCategoryChanged OnLogCategoryChanged;
	FString FilterMetaData;
	FName SelectedName;
	TSharedPtr<class SComboButton> ComboButton;
};
