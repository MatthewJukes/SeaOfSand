
#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "IPropertyUtilities.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Layout/Margin.h"
#include "Widgets/SWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/SListView.h"
#include "Engine/DataTable.h"
#include "SoSAIDecisionData.h"

#define LOCTEXT_NAMESPACE "FConsiderationPresetCustomization"

struct FAssetData;

/**
* Customizes a DataTable asset to use a dropdown
*/
class FConsiderationPresetCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FConsiderationPresetCustomization);
	}

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> InStructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> InStructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:

	bool ShouldFilterAsset(const FAssetData& AssetData);

	/** Init the contents the combobox sources its data off */
	TSharedPtr<FString> InitWidgetContent();

	/** Returns the ListView for the ComboButton */
	TSharedRef<SWidget> GetListContent();

	/** Delegate to refresh the drop down when the datatable changes */
	void OnDataTableChanged();

	/** Delegate to refresh the consideration struct when the preset changes */
	void OnPresetChanged();

	/** Return the representation of the the row names to display */
	TSharedRef<ITableRow> HandleRowNameComboBoxGenarateWidget(TSharedPtr<FString> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	/** Display the current selection */
	FText GetRowNameComboBoxContentText() const;

	/** Update the root data on a change of selection */
	void OnSelectionChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo);

	/** Called by Slate when the filter box changes text. */
	void OnFilterTextChanged(const FText& InFilterText);

	void HandleMenuOpen();

	/** The comboButton objects */
	TSharedPtr<SComboButton> RowNameComboButton;
	TSharedPtr<class SSearchBox> SearchBox;
	TSharedPtr<SListView<TSharedPtr<FString> > > RowNameComboListView;
	TSharedPtr<FString> CurrentSelectedItem;
	TSharedPtr<FDecisionConsideration> StructPreset;
	/** Handle to the struct properties being customized */
	TSharedPtr<IPropertyHandle> StructPropertyHandle;
	TSharedPtr<IPropertyHandle> DataTablePropertyHandle;
	TSharedPtr<IPropertyHandle> RowNamePropertyHandle;
	TSharedPtr<IPropertyHandle> ConsiderationPropertyHandle;

	/** A cached copy of strings to populate the combo box */
	TArray<TSharedPtr<FString> > RowNames;
	/** The MetaData derived filter for the row type */
	FName RowTypeFilter;
};

#undef LOCTEXT_NAMESPACE