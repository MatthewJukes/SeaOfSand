// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class APlayerCharacter;
class ABaseWeapon;

UCLASS()
class SEAOFSAND_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
		
public:
	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

	// Reference to the weapon the player has currently equipped
	void UpdateCurrentPawn();

	// Reference to the weapon the player has currently equipped
	void UpdateCurrentWeapon(ABaseWeapon* NewWeapon);

	// Return an OUT parameter
	FVector GetCrosshairHitLocation() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:

	UPROPERTY(EditDefaultsOnly)
	float CrossHairXLocation = 0.5;

	UPROPERTY(EditDefaultsOnly)
	float CrosshairYLocation = 0.5;

	UPROPERTY(EditDefaultsOnly)
	float CrosshairTraceRange = 100000.f;

	// Player firing
	void StartFiring();
	void StopFiring();

	// Interact and mount/dismount vehicle
	void Interact();

	// Get crosshair world hit location
	bool GetLookDirection(FVector2D ScreenLocation, FVector & LookDirection) const;
	FVector GetLookVectorHitLocation(FVector LookDirection) const;

	// Current Player pawn
	APawn* CurrentPlayerPawn;

	// Currently equipped weapon
	ABaseWeapon* CurrentWeapon;
}; 
