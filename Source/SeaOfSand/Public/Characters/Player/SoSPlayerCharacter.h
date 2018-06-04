// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SoSPlayerCharacter.generated.h"

class UCameraComponent;
class UPlayerInventory;
class USpringArmComponent;
class ASoSPlayerController;

UCLASS()
class SEAOFSAND_API ASoSPlayerCharacter : public ACharacter
{
	GENERATED_BODY()	

public:
	// Set defaults
	ASoSPlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FollowCamera;

public:

	// The player controller
	UPROPERTY(BlueprintReadOnly, Category = "Controller")
	ASoSPlayerController* PlayerController;

	// The player inventory
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Player")
	UPlayerInventory* PlayerInventory;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsRolling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsDoubleJumping;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bInVehicle;

	UPROPERTY(BlueprintReadOnly, Category = "Vitals")
	float CurrentStamina; // Player stamina exposed to BPs for HUD

	// Turn collision back on
	void EnableCollsion();

	void SetPlayerSpeed(float SpeedMultiplier);

	void SetPlayerMovementType(bool bOrientRotationToMovement, bool bUseControllerDesiredRotation);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = OffsetCamera))
	void OffsetCamera(bool Forward);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = AimZoom))
	void AimZoom(bool Forward);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = SprintZoom))
	void SprintZoom(bool Forward);

	UFUNCTION() 
	void SprintEnd(); // stop/interrupt sprinting

	UFUNCTION()
	void AimEnd(); // stop/interrupt aiming

protected:

	bool bCanDoubleJump;

	bool bLastOrientRotationToMovement;

	FVector RollDirection;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float MaxStamina;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float BaseStaminaRegenRate;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float SprintStaminaDrainRate;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float RollStaminaCost;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BaseSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractTraceRange;

	void SetStaminaRate(float RatePerSecond);

	UFUNCTION()
	void IncrementStamina(float Amount);

	UFUNCTION()
	void MoveForward(float AxisValue);

	UFUNCTION()
	void MoveRight(float AxisValue);

	UFUNCTION()
	void SprintStart();	

	UFUNCTION()
	void CrouchStart();

	UFUNCTION()
	void CrouchEnd();

	UFUNCTION()
	void DoubleJump();

	UFUNCTION()
	void StartRoll();

	UFUNCTION()
	void Interact(); // Player interaction/use objects in world

	UFUNCTION()
	void Roll(FVector DodgeDirection, bool OrientRotationToMovement);

	UFUNCTION()
	void EndRoll(bool OrientRotationToMovement);

	UFUNCTION()
	void AimStart();

	void ResetAirControl(); // Reset air control after preforming double jump

	// Trace to see if hit interactable actor
	bool InteractTrace(AActor* &OutActor) const;
	FVector GetTraceDirection(FVector StartLocation) const;

	//Timer handles
	FTimerHandle TimerHandle_DoubleJump;
	FTimerHandle TimerHandle_DodgeEnd;
	FTimerHandle TimerHandle_Stamina;	
};
