// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerInputsInterface.h"
#include "PlayerCharacter.generated.h"

class ABasePlayerController;
class ABaseWeapon;

UCLASS()
class SEAOFSANDS_API APlayerCharacter : public ABaseCharacter , public IPlayerInputsInterface
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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
	bool bWeaponIsDrawn = true;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = OffsetCamera))
	void OffsetCamera(bool Forward);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = AimZoom))
	void AimZoom(bool Forward);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = SprintZoom))
	void SprintZoom(bool Forward);
	
	// Weapon holstering
	void HolsterUnholster();

	// Player interaction/use objects in world
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool Interact();
	virtual bool Interact_Implementation() override;

	bool bCanFire = false;
	
	ABaseWeapon* CurrentWeapon;

protected:

private:

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
	void JumpAndFlip();

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

	ABasePlayerController* PlayerController;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }	
};
