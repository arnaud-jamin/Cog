#include "SCogDebugLogCategoryWidget.h"

#include "CogDebugLog.h"
#include "Misc/TextFilter.h"
#include "SlateOptMacros.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectHash.h"
#include "UObject/UObjectIterator.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STableViewBase.h"

#define LOCTEXT_NAMESPACE "K2Node"

//--------------------------------------------------------------------------------------------------------------------------
DECLARE_DELEGATE_OneParam(FOnLogCategoryPicked, FName);

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

//--------------------------------------------------------------------------------------------------------------------------
struct FLogCategoryViewerNode
{
public:
	FLogCategoryViewerNode(FName InName)
	{
		Name = InName;
	}

	FName Name;
};

//--------------------------------------------------------------------------------------------------------------------------
class SLogCategoryItem : public SComboRow<TSharedPtr<FLogCategoryViewerNode>>
{
public:

	SLATE_BEGIN_ARGS(SLogCategoryItem)
		: _HighlightText()
		, _TextColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
	{}

	SLATE_ARGUMENT(FText, HighlightText)
	SLATE_ARGUMENT(FSlateColor, TextColor)
	SLATE_ARGUMENT(TSharedPtr<FLogCategoryViewerNode>, AssociatedNode)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		AssociatedNode = InArgs._AssociatedNode;

		this->ChildSlot
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.Padding(0.0f, 3.0f, 6.0f, 3.0f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString(*AssociatedNode->Name.ToString()))
						.HighlightText(InArgs._HighlightText)
						.ColorAndOpacity(this, &SLogCategoryItem::GetTextColor)
						.IsEnabled(true)
					]
			];

		TextColor = InArgs._TextColor;

		STableRow< TSharedPtr<FLogCategoryViewerNode> >::ConstructInternal(
			STableRow::FArguments()
			.ShowSelection(true),
			InOwnerTableView
			);
	}

	/** Returns the text color for the item based on if it is selected or not. */
	FSlateColor GetTextColor() const
	{
		const TSharedPtr<ITypedTableView<TSharedPtr<FLogCategoryViewerNode>>> OwnerWidget = OwnerTablePtr.Pin();
		const TSharedPtr<FLogCategoryViewerNode>* MyItem = OwnerWidget->Private_ItemFromWidget(this);
		const bool bIsSelected = OwnerWidget->Private_IsItemSelected(*MyItem);

		if (bIsSelected)
		{
			return FSlateColor::UseForeground();
		}

		return TextColor;
	}

private:

	/** The text color for this item. */
	FSlateColor TextColor;

	/** The LogCategory Viewer Node this item is associated with. */
	TSharedPtr<FLogCategoryViewerNode> AssociatedNode;
};

//--------------------------------------------------------------------------------------------------------------------------
class SLogCategoryListWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLogCategoryListWidget)
	{
	}

	SLATE_ARGUMENT(FString, FilterMetaData)
	SLATE_ARGUMENT(FOnLogCategoryPicked, OnLogCategoryPickedDelegate)

	SLATE_END_ARGS()

	/**
	* Construct the widget
	*
	* @param	InArgs			A declaration from which to construct the widget
	*/
	void Construct(const FArguments& InArgs);

	virtual ~SLogCategoryListWidget() override;

private:
	typedef TTextFilter<const FName&> FLogCategoryTextFilter;

	/** Called by Slate when the filter box changes text. */
	void OnFilterTextChanged(const FText& InFilterText);

	/** Creates the row widget when called by Slate when an item appears on the list. */
	TSharedRef< ITableRow > OnGenerateRowForLogCategoryViewer(TSharedPtr<FLogCategoryViewerNode> Item, const TSharedRef< STableViewBase >& OwnerTable) const;

	/** Called by Slate when an item is selected from the tree/list. */
	void OnLogCategorySelectionChanged(TSharedPtr<FLogCategoryViewerNode> Item, ESelectInfo::Type SelectInfo) const;

	/** Updates the list of items in the dropdown menu */
	TSharedPtr<FLogCategoryViewerNode> UpdatePropertyOptions();

	/** Delegate to be called when an LogCategory is picked from the list */
	FOnLogCategoryPicked OnLogCategoryPicked;

	/** The search box */
	TSharedPtr<SSearchBox> SearchBoxPtr;

	/** Holds the Slate List widget which holds the LogCategory for the LogCategory Viewer. */
	TSharedPtr<SListView<TSharedPtr< FLogCategoryViewerNode > >> LogCategoryList;

	/** Array of items that can be selected in the dropdown menu */
	TArray<TSharedPtr<FLogCategoryViewerNode>> PropertyOptions;

	/** Filters needed for filtering the assets */
	TSharedPtr<FLogCategoryTextFilter> LogCategoryTextFilter;

	/** Filter for meta data */
	FString FilterMetaData;
};

//--------------------------------------------------------------------------------------------------------------------------
SLogCategoryListWidget::~SLogCategoryListWidget()
{
	if (OnLogCategoryPicked.IsBound())
	{
		OnLogCategoryPicked.Unbind();
	}
}

//--------------------------------------------------------------------------------------------------------------------------
void SLogCategoryListWidget::Construct(const FArguments& InArgs)
{
	struct Local
	{
		static void LogCategoryToStringArray(const FName& Name, OUT TArray< FString >& StringArray)
		{
		    StringArray.Add(Name.ToString());
		}
	};

	FilterMetaData = InArgs._FilterMetaData;
	OnLogCategoryPicked = InArgs._OnLogCategoryPickedDelegate;

	// Setup text filtering
	LogCategoryTextFilter = MakeShareable(new FLogCategoryTextFilter(FLogCategoryTextFilter::FItemToStringArray::CreateStatic(&Local::LogCategoryToStringArray)));

	UpdatePropertyOptions();

	TSharedPtr< SWidget > ClassViewerContent;

	SAssignNew(ClassViewerContent, SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(SearchBoxPtr, SSearchBox)
			.HintText(NSLOCTEXT("Log", "SearchBoxHint", "Search LogCategories"))
			.OnTextChanged(this, &SLogCategoryListWidget::OnFilterTextChanged)
			.DelayChangeNotificationsWhileTyping(true)
		]

	+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
			.Visibility(EVisibility::Collapsed)
		]
	
	+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SAssignNew(LogCategoryList, SListView<TSharedPtr<FLogCategoryViewerNode>>)
			.Visibility(EVisibility::Visible)
			.SelectionMode(ESelectionMode::Single)
			.ListItemsSource(&PropertyOptions)

 			// Generates the actual widget for a tree item
			.OnGenerateRow(this, &SLogCategoryListWidget::OnGenerateRowForLogCategoryViewer)

 			// Find out when the user selects something in the tree
			.OnSelectionChanged(this, &SLogCategoryListWidget::OnLogCategorySelectionChanged)
		];


	ChildSlot
		[
			ClassViewerContent.ToSharedRef()
		];
}

//--------------------------------------------------------------------------------------------------------------------------
TSharedRef<ITableRow> SLogCategoryListWidget::OnGenerateRowForLogCategoryViewer(TSharedPtr<FLogCategoryViewerNode> Item, const TSharedRef< STableViewBase >& OwnerTable) const
{
	TSharedRef< SLogCategoryItem > ReturnRow = SNew(SLogCategoryItem, OwnerTable)
		.HighlightText(SearchBoxPtr->GetText())
		.TextColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.f))
		.AssociatedNode(Item);

	return ReturnRow;
}

//--------------------------------------------------------------------------------------------------------------------------
TSharedPtr<FLogCategoryViewerNode> SLogCategoryListWidget::UpdatePropertyOptions()
{
	PropertyOptions.Empty();
	TSharedPtr<FLogCategoryViewerNode> InitiallySelected = MakeShareable(new FLogCategoryViewerNode(FName()));

	PropertyOptions.Add(InitiallySelected);

	// Gather all ULogCategory classes
    for (auto& Entry : FCogDebugLog::GetLogCategories())
	{
        // if we have a search string and this doesn't match, don't show it
        if (LogCategoryTextFilter.IsValid() && !LogCategoryTextFilter->PassesFilter(Entry.Value.LogCategory->GetCategoryName()))
        {
            continue;
        }

		TSharedPtr<FLogCategoryViewerNode> SelectableProperty = MakeShareable(new FLogCategoryViewerNode(Entry.Value.LogCategory->GetCategoryName()));
		PropertyOptions.Add(SelectableProperty);
	}

	return InitiallySelected;
}

//--------------------------------------------------------------------------------------------------------------------------
void SLogCategoryListWidget::OnFilterTextChanged(const FText& InFilterText)
{
	LogCategoryTextFilter->SetRawFilterText(InFilterText);
	SearchBoxPtr->SetError(LogCategoryTextFilter->GetFilterErrorText());

	UpdatePropertyOptions();
}

//--------------------------------------------------------------------------------------------------------------------------
void SLogCategoryListWidget::OnLogCategorySelectionChanged(TSharedPtr<FLogCategoryViewerNode> Item, ESelectInfo::Type SelectInfo) const
{
	OnLogCategoryPicked.ExecuteIfBound(Item->Name);
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogLogCategoryWidget::Construct(const FArguments& InArgs)
{
	FilterMetaData = InArgs._FilterMetaData;
	OnLogCategoryChanged = InArgs._OnLogCategoryChanged;
	SelectedName = InArgs._DefaultName;

	// set up the combo button
	SAssignNew(ComboButton, SComboButton)
		.OnGetMenuContent(this, &SCogLogCategoryWidget::GenerateLogCategoryPicker)
		.ContentPadding(FMargin(2.0f, 2.0f))
		.ToolTipText(this, &SCogLogCategoryWidget::GetSelectedValueAsText)
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(this, &SCogLogCategoryWidget::GetSelectedValueAsText)
		];

	ChildSlot
	[
		ComboButton.ToSharedRef()
	];
}

//--------------------------------------------------------------------------------------------------------------------------
void SCogLogCategoryWidget::OnItemPicked(FName Name)
{
	if (OnLogCategoryChanged.IsBound())
	{
		OnLogCategoryChanged.Execute(Name);
	}

	// Update the selected item for displaying
	SelectedName = Name;

	// close the list
	ComboButton->SetIsOpen(false);
}

//--------------------------------------------------------------------------------------------------------------------------
TSharedRef<SWidget> SCogLogCategoryWidget::GenerateLogCategoryPicker()
{
	FOnLogCategoryPicked OnPicked(FOnLogCategoryPicked::CreateRaw(this, &SCogLogCategoryWidget::OnItemPicked));

	return SNew(SBox)
		.WidthOverride(280)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.MaxHeight(500)
			[
				SNew(SLogCategoryListWidget)
				.OnLogCategoryPickedDelegate(OnPicked)
				.FilterMetaData(FilterMetaData)
			]
		];
}

//--------------------------------------------------------------------------------------------------------------------------
FText SCogLogCategoryWidget::GetSelectedValueAsText() const
{
    return FText::FromName(SelectedName);
}

//--------------------------------------------------------------------------------------------------------------------------
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
