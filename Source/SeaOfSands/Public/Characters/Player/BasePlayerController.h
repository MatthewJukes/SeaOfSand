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

	// Reference to the weapon the player has currently equipped
	void UpdateCurrentWeapon(ABaseWeapon* NewWeapon);

	// Return an OUT parameter
	void GetCrosshairHitLocation(FVector& OutHitLocation);
	
private:

	UPROPERTY(EditDefaultsOnly)
	float CrossHairXLocation = 0.5;

	UPROPERTY(EditDefaultsOnly)
	float CrosshairYLocation = 0.5;

	UPROPERTY(EditDefaultsOnly)
	float LineTraceRange = 100000.f;

	// Player firing
	void StartFiring();
	void StopFiring();

	// Get crosshair world hit location
	bool GetLookDirection(FVector2D ScreenLocation, FVector & LookDirection) const;
	bool GetLookVectorHitLocation(FVector & HitLocation, FVector LookDirection) const;	

	// Currently equipped weapon
	ABaseWeapon* CurrentWeapon;
}; 
