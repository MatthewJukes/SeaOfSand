// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SoSPlayerController.generated.h"

class IPlayerInputsInterface;
class ASoSPlayerCharacter;
class USoSInventoryComponent;
class ASoSPlayerHUD;

UCLASS()
class SEAOFSAND_API ASoSPlayerController : public APlayerController
{
	GENERATED_BODY()
		
public:

	ASoSPlayerController();

	virtual void SetupInputComponent() override;

protected:

	virtual void BeginPlay() override;

public:

	void UpdateCurrentPawn();

	void ToggleVehicleHud();

	bool GetCrosshairHitResult(FHitResult &OutHitResult, bool bOffsetFromCamera = false, FVector OffsetTarget = FVector(0, 0, 0)) const;

	// Get Pitch/Yaw from current pawn camera
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	FRotator GetAimOffsets() const;

protected:

	APawn* CurrentPlayerPawn;

	ASoSPlayerCharacter* PlayerCharacter;

	USoSInventoryComponent* PlayerInventory;

	ASoSPlayerHUD* PlayerHUD;

	UPROPERTY(EditDefaultsOnly)
	float CrosshairXLocation;

	UPROPERTY(EditDefaultsOnly)
	float CrosshairYLocation;

	UPROPERTY(EditDefaultsOnly)
	float CrosshairTraceRange;

	void PrimaryAttackStart();

	void PrimaryAttackEnd();

	void AlternateAttackStart();

	void AlternateAttackEnd();
	
	void HolsterUnholster();
	
	void Reload();

	void NextWeapon();

	void PrevWeapon();

	// Get crosshair world hit location
	bool GetLookDirection(FVector2D ScreenLocation, FVector &LookDirection) const;

	bool GetLookVectorHitResult(FHitResult &OutHitResult, FVector LookDirection, bool bOffsetFromCamera, FVector OffsetTarget) const;

public:

	ASoSPlayerHUD* GetPlayerHUD() const;
}; 
