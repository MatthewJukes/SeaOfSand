// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerInputsInterface.h"
#include "PlayerCharacter.generated.h"

class ABasePlayerController;
class ABaseWeapon;

UCLASS()
class SEAOFSAND_API APlayerCharacter : public ABaseCharacter , public IPlayerInputsInterface
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

	ABaseWeapon * CurrentWeapon;

	bool bCanFire;

	UPROPERTY(BlueprintReadOnly, Category = "Controller")
	ABasePlayerController* PlayerController;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
	bool bWeaponIsDrawn;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
	bool bIsRolling;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
	bool bIsSprinting;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
	bool bIsDoubleJumping;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
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

	// Player interaction/use objects in world
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction") // TODO learn way to do without BlueprintCallable,etc
	bool Interact();
	virtual bool Interact_Implementation() override;

	// Weapon holstering
	void HolsterUnholster();

	// Turn collision back on
	void EnableCollsion();

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

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float AimMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WeaponDrawnMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractTraceRange;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName WeaponAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName BackHolsterAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ABaseWeapon> RifleBlueprint;

	// Player vitals
	void SetStaminaRate(float RatePerSecond);

	UFUNCTION()
	void IncrementStamina(float Amount);

	// Player movement
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void SprintStart();	
	void CrouchStart();
	void CrouchEnd();
	void DoubleJump();
	void ResetAirControl();
	void StartRoll();		

	UFUNCTION()
	void Roll(FVector DodgeDirection, bool OrientRotationToMovement);

	UFUNCTION()
	void EndRoll(bool OrientRotationToMovement);

	// Player weapon
	void SpawnWeapon();	

	// Player firing
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
