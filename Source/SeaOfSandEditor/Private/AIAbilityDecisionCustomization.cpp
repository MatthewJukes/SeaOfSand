
#include "AIAbilityDecisionCustomization.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Input/SSearchBox.h"

#define LOCTEXT_NAMESPACE "FAIAbilityDecisionCustomization"

TSharedPtr<FString> FAIAbilityDecisionCustomization::InitWidgetContent()
{
	TSharedPtr<FString> InitialValue = MakeShareable(new FString(LOCTEXT("DataTable_None", "None").ToString()));;

	FName RowName;
	const FPropertyAccess::Result RowResult = RowNamePropertyHandle->GetValue(RowName);
	RowNames.Empty();

	/** Get the properties we wish to work with */
	UDataTable* DataTable = NULL;
	DataTablePropertyHandle->GetValue((UObject*&)DataTable);

	if (DataTable != NULL)
	{
		/** Extract all the row names from the RowMap */
		for (TMap<FName, uint8*>::TConstIterator Iterator(DataTable->RowMap); Iterator; ++Iterator)
		{
			/** Create a simple array of the row names */
			TSharedRef<FString> RowNameItem = MakeShareable(new FString(Iterator.Key().ToString()));
			RowNames.Add(RowNameItem);

			/** Set the initial value to the currently selected item */
			if (Iterator.Key() == RowName)
			{
				InitialValue = RowNameItem;
			}
		}
	}

	/** Reset the initial value to ensure a valid entry is set */
	if (RowResult != FPropertyAccess::MultipleValues)
	{
		FName NewValue = FName(**InitialValue);
		RowNamePropertyHandle->SetValue(NewValue);
	}

	return InitialValue;
}

void FAIAbilityDecisionCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> InStructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	this->StructPropertyHandle = InStructPropertyHandle;

	if (StructPropertyHandle->HasMetaData(TEXT("RowType")))
	{
		const FString& RowType = StructPropertyHandle->GetMetaData(TEXT("RowType"));
		RowTypeFilter = FName(*RowType);
	}

	HeaderRow
		.NameContent()
		[
			InStructPropertyHandle->CreatePropertyNameWidget(FText::GetEmpty(), FText::GetEmpty(), false)
		];
}

void FAIAbilityDecisionCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> InStructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	/** Get all the existing property handles */
	DataTablePropertyHandle = InStructPropertyHandle->GetChildHandle("DataTable");
	RowNamePropertyHandle = InStructPropertyHandle->GetChildHandle("RowName");

	if (DataTablePropertyHandle->IsValidHandle() && RowNamePropertyHandle->IsValidHandle())
	{
		/** Queue up a refresh of the selected item, not safe to do from here */
		StructCustomizationUtils.GetPropertyUtilities()->EnqueueDeferredAction(FSimpleDelegate::CreateSP(this, &FAIAbilityDecisionCustomization::OnDataTableChanged));

		/** Setup Change callback */
		FSimpleDelegate OnDataTableChangedDelegate = FSimpleDelegate::CreateSP(this, &FAIAbilityDecisionCustomization::OnDataTableChanged);
		DataTablePropertyHandle->SetOnPropertyValueChanged(OnDataTableChangedDelegate);

		/** Construct a combo box widget to select from a list of valid options */
		StructBuilder.AddCustomRow(LOCTEXT("DataTable_RowName", "Preset"))
			.NameContent()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("DataTable_RowName", "Preset"))
			//.Font(StructCustomizationUtils.GetRegularFont())
			]
		.ValueContent()
			.MaxDesiredWidth(0.0f) // don't constrain the combo button width
			[
				SAssignNew(RowNameComboButton, SComboButton)
				.ToolTipText(this, &FAIAbilityDecisionCustomization::GetRowNameComboBoxContentText)
			.OnGetMenuContent(this, &FAIAbilityDecisionCustomization::GetListContent)
			.OnComboBoxOpened(this, &FAIAbilityDecisionCustomization::HandleMenuOpen)
			.ContentPadding(FMargin(2.0f, 2.0f))
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(this, &FAIAbilityDecisionCustomization::GetRowNameComboBoxContentText)
			]
			];
	}
}

void FAIAbilityDecisionCustomization::HandleMenuOpen()
{
	FSlateApplication::Get().SetKeyboardFocus(SearchBox);
}

TSharedRef<SWidget> FAIAbilityDecisionCustomization::GetListContent()
{
	SAssignNew(RowNameComboListView, SListView<TSharedPtr<FString> >)
		.ListItemsSource(&RowNames)
		.OnSelectionChanged(this, &FAIAbilityDecisionCustomization::OnSelectionChanged)
		.OnGenerateRow(this, &FAIAbilityDecisionCustomization::HandleRowNameComboBoxGenarateWidget)
		.SelectionMode(ESelectionMode::Single);

	// Ensure no filter is applied at the time the menu opens
	OnFilterTextChanged(FText::GetEmpty());

	if (CurrentSelectedItem.IsValid())
	{
		RowNameComboListView->SetSelection(CurrentSelectedItem);
	}

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(SearchBox, SSearchBox)
			.OnTextChanged(this, &FAIAbilityDecisionCustomization::OnFilterTextChanged)
		]

	+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SNew(SBox)
			.MaxDesiredHeight(600)
		[
			RowNameComboListView.ToSharedRef()
		]
		];
}

void FAIAbilityDecisionCustomization::OnDataTableChanged()
{
	CurrentSelectedItem = InitWidgetContent();
	if (RowNameComboListView.IsValid())
	{
		RowNameComboListView->SetSelection(CurrentSelectedItem);
		RowNameComboListView->RequestListRefresh();
	}
}

TSharedRef<ITableRow> FAIAbilityDecisionCustomization::HandleRowNameComboBoxGenarateWidget(TSharedPtr<FString> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return
		SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			SNew(STextBlock).Text(FText::FromString(*InItem))
		];
}

FText FAIAbilityDecisionCustomization::GetRowNameComboBoxContentText() const
{
	FString RowNameValue;
	const FPropertyAccess::Result RowResult = RowNamePropertyHandle->GetValue(RowNameValue);
	if (RowResult != FPropertyAccess::MultipleValues)
	{
		TSharedPtr<FString> SelectedRowName = CurrentSelectedItem;
		if (SelectedRowName.IsValid())
		{
			return FText::FromString(*SelectedRowName);
		}
		else
		{
			return LOCTEXT("DataTable_None", "None");
		}
	}

	return LOCTEXT("MultipleValues", "Multiple Values");
}

void FAIAbilityDecisionCustomization::OnSelectionChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
{
	if (SelectedItem.IsValid())
	{
		CurrentSelectedItem = SelectedItem;
		FName NewValue = FName(**SelectedItem);
		RowNamePropertyHandle->SetValue(NewValue);

		// Close the combo
		RowNameComboButton->SetIsOpen(false);
	}
}

void FAIAbilityDecisionCustomization::OnFilterTextChanged(const FText& InFilterText)
{
	FString CurrentFilterText = InFilterText.ToString();

	FName RowName;
	const FPropertyAccess::Result RowResult = RowNamePropertyHandle->GetValue(RowName);
	RowNames.Empty();

	/** Get the properties we wish to work with */
	UDataTable* DataTable = nullptr;
	DataTablePropertyHandle->GetValue((UObject*&)DataTable);

	TArray<FString> AllRowNames;
	if (DataTable != nullptr)
	{
		for (TMap<FName, uint8*>::TConstIterator Iterator(DataTable->RowMap); Iterator; ++Iterator)
		{
			FString RowString = Iterator.Key().ToString();
			AllRowNames.Add(RowString);
		}

		// Sort the names alphabetically.
		AllRowNames.Sort();
	}

	for (const FString& RowString : AllRowNames)
	{
		if (CurrentFilterText == TEXT("") || RowString.Contains(CurrentFilterText))
		{
			TSharedRef<FString> RowNameItem = MakeShareable(new FString(RowString));
			RowNames.Add(RowNameItem);
		}
	}

	RowNameComboListView->RequestListRefresh();
}

bool FAIAbilityDecisionCustomization::ShouldFilterAsset(const struct FAssetData& AssetData)
{
	if (!RowTypeFilter.IsNone())
	{
		const UDataTable* DataTable = Cast<UDataTable>(AssetData.GetAsset());
		if (DataTable->RowStruct && DataTable->RowStruct->GetFName() == RowTypeFilter)
		{
			return false;
		}
		return true;
	}
	return false;
}

#undef LOCTEXT_NAMESPACE