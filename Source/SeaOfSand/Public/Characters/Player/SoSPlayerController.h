// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SoSPlayerController.generated.h"

class IPlayerInputsInterface;
class ASoSPlayerCharacter;
class APlayerHUD;

UCLASS()
class SEAOFSAND_API ASoSPlayerController : public APlayerController
{
	GENERATED_BODY()
		
public:

	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

	// The player character
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	ASoSPlayerCharacter* PlayerCharacter;

	// Reference to the weapon the player has currently equipped
	void UpdateCurrentPawn();

	// Toggle Vehicle HUD
	void ToggleVehicleHud();

	// Return an OUT parameter
	FVector GetCrosshairHitLocation() const;

	// Get Pitch/Yaw from current pawn camera
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	FRotator GetAimOffsets() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Current Player pawn
	APawn* CurrentPlayerPawn;

	// Player HUD
	APlayerHUD* PlayerHUD;

	UPROPERTY(EditDefaultsOnly)
	float CrossHairXLocation = 0.5;

	UPROPERTY(EditDefaultsOnly)
	float CrosshairYLocation = 0.5;

	UPROPERTY(EditDefaultsOnly)
	float CrosshairTraceRange = 100000.f;

	void StartFiring();

	void StopFiring();
	
	void HolsterUnholster(); // Holster Weapon
	
	void Reload(); // Reload Weapon

	void NextWeapon();

	void PrevWeapon();

	// Get crosshair world hit location
	bool GetLookDirection(FVector2D ScreenLocation, FVector &LookDirection) const;

	FVector GetLookVectorHitLocation(FVector LookDirection) const;
}; 
