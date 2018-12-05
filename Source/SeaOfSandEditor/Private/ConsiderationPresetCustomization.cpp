
#include "ConsiderationPresetCustomization.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Input/SSearchBox.h"

#define LOCTEXT_NAMESPACE "FConsiderationPresetCustomization"


TSharedPtr<FString> FConsiderationPresetCustomization::InitWidgetContent()
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

void FConsiderationPresetCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> InStructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
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

void FConsiderationPresetCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> InStructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	/** Get all the existing property handles */
	DataTablePropertyHandle = InStructPropertyHandle->GetChildHandle("DataTable");
	RowNamePropertyHandle = InStructPropertyHandle->GetChildHandle("RowName");
	ConsiderationPropertyHandle = InStructPropertyHandle->GetChildHandle("Consideration");

	if (DataTablePropertyHandle->IsValidHandle() && RowNamePropertyHandle->IsValidHandle() && ConsiderationPropertyHandle->IsValidHandle())
	{
		/** Queue up a refresh of the selected item, not safe to do from here */
		StructCustomizationUtils.GetPropertyUtilities()->EnqueueDeferredAction(FSimpleDelegate::CreateSP(this, &FConsiderationPresetCustomization::OnDataTableChanged));

		/** Setup Change callback */
		FSimpleDelegate OnDataTableChangedDelegate = FSimpleDelegate::CreateSP(this, &FConsiderationPresetCustomization::OnDataTableChanged);
		DataTablePropertyHandle->SetOnPropertyValueChanged(OnDataTableChangedDelegate);	

		/** Setup Change callback */
		FSimpleDelegate OnPresetChangeDelegate = FSimpleDelegate::CreateSP(this, &FConsiderationPresetCustomization::OnPresetChanged);
		RowNamePropertyHandle->SetOnPropertyValueChanged(OnPresetChangeDelegate);

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
			.ToolTipText(this, &FConsiderationPresetCustomization::GetRowNameComboBoxContentText)
			.OnGetMenuContent(this, &FConsiderationPresetCustomization::GetListContent)
			.OnComboBoxOpened(this, &FConsiderationPresetCustomization::HandleMenuOpen)
			.ContentPadding(FMargin(2.0f, 2.0f))
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(this, &FConsiderationPresetCustomization::GetRowNameComboBoxContentText)
			]
		];

		StructBuilder.AddProperty(ConsiderationPropertyHandle.ToSharedRef());		
	}
}

void FConsiderationPresetCustomization::HandleMenuOpen()
{
	FSlateApplication::Get().SetKeyboardFocus(SearchBox);
}

TSharedRef<SWidget> FConsiderationPresetCustomization::GetListContent()
{
	SAssignNew(RowNameComboListView, SListView<TSharedPtr<FString> >)
	.ListItemsSource(&RowNames)
	.OnSelectionChanged(this, &FConsiderationPresetCustomization::OnSelectionChanged)
	.OnGenerateRow(this, &FConsiderationPresetCustomization::HandleRowNameComboBoxGenarateWidget)
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
		.OnTextChanged(this, &FConsiderationPresetCustomization::OnFilterTextChanged)
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

void FConsiderationPresetCustomization::OnDataTableChanged()
{
	CurrentSelectedItem = InitWidgetContent();
	if (RowNameComboListView.IsValid())
	{
		RowNameComboListView->SetSelection(CurrentSelectedItem);
		RowNameComboListView->RequestListRefresh();
	}
}


TSharedPtr<IPropertyHandle> FConsiderationPresetCustomization::GetChildProperty(const TSharedPtr<IPropertyHandle>& ParentHandle, const FName Property)
{
	return ParentHandle->IsValidHandle() ? ParentHandle->GetChildHandle(Property) : nullptr;
}


void FConsiderationPresetCustomization::SetDecisionConsiderationChildProperty(const TSharedPtr<IPropertyHandle>& ChildHandle, uint8 PropertyIndex, FDecisionConsideration* StructPreset)
{
	if (StructPreset == nullptr)
	{
		return;
	}

	if (ChildHandle->IsValidHandle())
	{
		switch (PropertyIndex)
		{
		case 0:
			ChildHandle->SetValue(StructPreset->ResponseCurve.Curve);
			break;
		case 1:
			ChildHandle->SetValue(StructPreset->ResponseCurve.OffsetX);
			break;
		case 2:
			ChildHandle->SetValue(StructPreset->ResponseCurve.OffsetY);
			break;
		case 3:
			ChildHandle->SetValue((uint8)StructPreset->Parameter.ConsiderationParameter);
			break;
		case 4:
			ChildHandle->SetValue(StructPreset->Parameter.RangeMin);
			break;
		case 5:
			ChildHandle->SetValue(StructPreset->Parameter.RangeMax);
			break;
		default:
			break;
		}
	}
}

void FConsiderationPresetCustomization::OnPresetChanged()
{
	if (ConsiderationPropertyHandle.IsValid())
	{
		/** Get the properties we wish to work with */
		UDataTable* DataTable = NULL;
		DataTablePropertyHandle->GetValue((UObject*&)DataTable);

		/** Get datatable row struct */
		if (DataTable == nullptr)
		{
			return;
		}

		FName RowName;
		RowNamePropertyHandle->GetValue(RowName);
		FString ReferenceString = FString("");
		FDecisionConsideration* StructPreset = DataTable->FindRow<FDecisionConsideration>(RowName, ReferenceString);

		TSharedPtr<IPropertyHandle> ResponseCurvePropertyHandle = GetChildProperty(ConsiderationPropertyHandle, "ResponseCurve");
		TSharedPtr<IPropertyHandle> ParameterCurvePropertyHandle = GetChildProperty(ConsiderationPropertyHandle, "Parameter");


		SetDecisionConsiderationChildProperty(GetChildProperty(ResponseCurvePropertyHandle, "Curve"), 0, StructPreset);
		SetDecisionConsiderationChildProperty(GetChildProperty(ResponseCurvePropertyHandle, "OffsetX"), 1, StructPreset);
		SetDecisionConsiderationChildProperty(GetChildProperty(ResponseCurvePropertyHandle, "OffsetY"), 2, StructPreset);
		SetDecisionConsiderationChildProperty(GetChildProperty(ParameterCurvePropertyHandle, "ConsiderationParameter"), 3, StructPreset);
		SetDecisionConsiderationChildProperty(GetChildProperty(ParameterCurvePropertyHandle, "RangeMin"), 4, StructPreset);
		SetDecisionConsiderationChildProperty(GetChildProperty(ParameterCurvePropertyHandle, "RangeMax"), 5, StructPreset);
	}
}

TSharedRef<ITableRow> FConsiderationPresetCustomization::HandleRowNameComboBoxGenarateWidget(TSharedPtr<FString> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return
	SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
	[
		SNew(STextBlock).Text(FText::FromString(*InItem))
	];
}

FText FConsiderationPresetCustomization::GetRowNameComboBoxContentText() const
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

void FConsiderationPresetCustomization::OnSelectionChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
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

void FConsiderationPresetCustomization::OnFilterTextChanged(const FText& InFilterText)
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

bool FConsiderationPresetCustomization::ShouldFilterAsset(const struct FAssetData& AssetData)
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