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

	bool bCanFire = false;

	UPROPERTY(BlueprintReadOnly, Category = "Controller")
	ABasePlayerController* PlayerController;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
	bool bWeaponIsDrawn = true;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
	bool bInVehicle = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
	bool bIsDoubleJumping = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
	bool bIsRolling = false;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	float Health; // Player health exposed to BPs for HUD

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

protected:

private:

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 250.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BaseSpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintMultiplier = 1.6f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float AimMultiplier = .6f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WeaponDrawnMultiplier = .8f;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractTraceRange = 250.f;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName WeaponAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName BackHolsterAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ABaseWeapon> RifleBlueprint;

	// Player movement
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void SprintStart();
	void SprintEnd();
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
	bool bIsSprinting = false;	
	bool bCanDoubleJump = true;

	//Timer handles
	FTimerHandle DoubleJumpTimerHandle;
	FTimerHandle DodgeTimerHandle;
	FTimerHandle DodgeEndTimerHandle;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }	
};
