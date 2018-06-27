// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SoSPlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class SEAOFSAND_API ASoSPlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = ToggleVehicleHud))
	void ToggleVehicleHud();	

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = ToggleCrosshair))
	void ToggleCrosshair();
};
