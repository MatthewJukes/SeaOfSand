// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class ABaseWeapon;

UCLASS()
class SEAOFSANDS_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

	//void UpdateCurrentWeapon(ABaseWeapon* NewWeapon);
	
private:
	void StartFiring();
	void StopFiring();

	//ABaseWeapon CurrentWeapon;
};
