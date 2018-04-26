// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class ABasePlayerController;
class UPlayerInventory;

UCLASS()
class SEAOFSAND_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	// Set defaults
	APlayerCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// The player controller
	UPROPERTY(BlueprintReadOnly, Category = "Controller")
	ABasePlayerController* PlayerController;

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
	float CurrentHealth; // Player health exposed to BPs for HUD

	UPROPERTY(BlueprintReadOnly, Category = "Vitals")
	float CurrentStamina; // Player stamina exposed to BPs for HUD

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = OffsetCamera))
	void OffsetCamera(bool Forward);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = AimZoom))
	void AimZoom(bool Forward);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = SprintZoom))
	void SprintZoom(bool Forward);

	// Turn collision back on
	void EnableCollsion();

	void SetPlayerSpeed(float SpeedMultiplier);

	void SetPlayerMovementType(bool bOrientRotationToMovement, bool bUseControllerDesiredRotation);

	// stop/interrupt sprinting
	void SprintEnd();

protected:

private:

	UPROPERTY(EditDefaultsOnly, Category = "Vitals")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Vitals")
	float MaxStamina;

	UPROPERTY(EditDefaultsOnly, Category = "Vitals")
	float BaseStaminaRegenRate;

	UPROPERTY(EditDefaultsOnly, Category = "Vitals")
	float SprintStaminaDrainRate;

	UPROPERTY(EditDefaultsOnly, Category = "Vitals")
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

	void ResetAirControl(); // Reset air control after preforming double jump

	// Player aiming
	void AimStart();
	void AimEnd();

	// Trace to see if hit interactable actor
	bool InteractTrace(AActor* &OutActor) const;
	FVector GetTraceDirection(FVector StartLocation) const;

	// Movement state bools
	bool bCanDoubleJump;

	//Timer handles
	FTimerHandle DoubleJumpTimerHandle;
	FTimerHandle DodgeTimerHandle;
	FTimerHandle DodgeEndTimerHandle;
	FTimerHandle StaminaTimerHandle;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }	
};
