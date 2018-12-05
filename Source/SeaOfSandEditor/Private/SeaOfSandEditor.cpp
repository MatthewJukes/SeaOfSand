// Fill out your copyright notice in the Description page of Project Settings.

#include "SeaOfSandEditor.h"
#include "ConsiderationPresetCustomization.h"
#include "AIActionDecisionCustomization.h"
#include "AIAbilityDecisionCustomization.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "PropertyEditorModule.h" 


IMPLEMENT_GAME_MODULE(FSeaOfSandEditorModule, SeaOfSandEditor);

DEFINE_LOG_CATEGORY(SeaOfSandEditor)

#define LOCTEXT_NAMESPACE "SeaOfSandEditor"

void FSeaOfSandEditorModule::StartupModule()
{
	UE_LOG(SeaOfSandEditor, Warning, TEXT("SeaOfSandEditor: Log Started"));

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("ConsiderationPreset", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FConsiderationPresetCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("AIActionDecision", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FAIActionDecisionCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("AIAbilityDecision", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FAIAbilityDecisionCustomization::MakeInstance));
}

void FSeaOfSandEditorModule::ShutdownModule()
{
	UE_LOG(SeaOfSandEditor, Warning, TEXT("SeaOfSandEditor: Log Ended"));
}

#undef LOCTEXT_NAMESPACE 