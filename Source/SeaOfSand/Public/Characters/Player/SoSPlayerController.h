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

	// Reference to the weapon the player has currently equipped
	void UpdateCurrentPawn();

	// Toggle Vehicle HUD
	void ToggleVehicleHud();

	// Return an OUT parameter
	FVector GetCrosshairHitLocation() const;

	// The player character
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	ASoSPlayerCharacter* PlayerCharacter;

	// Get Pitch/Yaw from current pawn camera
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	FRotator GetAimOffsets() const;

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

	UFUNCTION()
	void StartFiring();

	UFUNCTION()
	void StopFiring();
	
	UFUNCTION()
	void HolsterUnholster(); // Holster Weapon
	
	UFUNCTION()
	void Reload(); // Reload Weapon

	UFUNCTION()
	void NextWeapon();

	UFUNCTION()
	void PrevWeapon();

	// Get crosshair world hit location
	bool GetLookDirection(FVector2D ScreenLocation, FVector &LookDirection) const;
	FVector GetLookVectorHitLocation(FVector LookDirection) const;

	// Interface to handle inputs for both player and vehicles
	IPlayerInputsInterface* InputInterface;

	// Current Player pawn
	APawn* CurrentPlayerPawn;	

	// Player HUD
	APlayerHUD* PlayerHUD;
}; 
