// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Base/BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class ABaseWeapon;

UCLASS()
class SEAOFSANDS_API APlayerCharacter : public ABaseCharacter
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

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement")
	bool bIsAiming = false;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = AimZoom))
	void AimZoom(bool Forward);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = SprintZoom))
	void SprintZoom(bool Forward);

	ABaseWeapon* CurrentWeapon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BaseSpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintMultiplier = 1.6f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float AimMultiplier = .6f;

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
	void JumpAndFlip();

	// Player weapons
	void SpawnWeapon();
	void EquipWeapon();
	void HolsterWeapon();	

	// Player firing
	void AimStart();
	void AimEnd();
	bool bCanFire = false;

	// Movement state bools
	bool bIsSprinting = false;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }	
};
