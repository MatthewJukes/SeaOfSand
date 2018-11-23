// Fill out your copyright notice in the Description page of Project Settings.

#include "SeaOfSandEditor.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "PropertyEditorModule.h" 
#include "Editor/DetailCustomizations/Private/DetailCustomizationsPrivatePCH.h"
#include "DetailCustomizations/FConsiderationPresetCustomization.h"

IMPLEMENT_GAME_MODULE(FSeaOfSandEditorModule, SeaOfSandEditor);

DEFINE_LOG_CATEGORY(SeaOfSandEditor)

#define LOCTEXT_NAMESPACE "SeaOfSandEditor"

void FSeaOfSandEditorModule::StartupModule()
{
	UE_LOG(SeaOfSandEditor, Warning, TEXT("SeaOfSandEditor: Log Started"));

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//Custom properties 
	PropertyModule.RegisterCustomPropertyTypeLayout("ConsiderationPreset", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FConsiderationPresetCustomization::MakeInstance));
}

void FSeaOfSandEditorModule::ShutdownModule()
{
	UE_LOG(SeaOfSandEditor, Warning, TEXT("SeaOfSandEditor: Log Ended"));
}

#undef LOCTEXT_NAMESPACE 