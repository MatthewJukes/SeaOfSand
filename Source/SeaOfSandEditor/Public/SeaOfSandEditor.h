#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "UnrealEd.h"

DECLARE_LOG_CATEGORY_EXTERN(SeaOfSandEditor, All, All)

class FSeaOfSandEditorModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	
	virtual void ShutdownModule() override;
}; 