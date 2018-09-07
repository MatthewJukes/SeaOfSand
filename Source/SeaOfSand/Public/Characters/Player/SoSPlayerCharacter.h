// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SoSPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASoSPlayerController;
class USoSInventoryComponent;
class USoSASComponent;
class USoSASAbilityBase;

DECLARE_DELEGATE_OneParam(FUseAbilityDelegate, int32);

USTRUCT(BlueprintType)
struct FPlayerASAbilitiesData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<USoSASAbilityBase> AbilityOne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<USoSASAbilityBase> AbilityTwo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<USoSASAbilityBase> AbilityThree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<USoSASAbilityBase> AbilityFour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<USoSASAbilityBase> AbilityFive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<USoSASAbilityBase> AbilitySix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<USoSASAbilityBase> AbilitySeven;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<USoSASAbilityBase> AbilityEigth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<USoSASAbilityBase> AbilitySprint; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<USoSASAbilityBase> AbilitySprintEnd;
}; 

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USoSInventoryComponent* InventoryComp;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USoSASComponent* ASComp;

public:

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

	// Turn collision back on
	void EnableCollsion();
	
	void SprintEnd(); // stop/interrupt sprinting

	void AimEnd(); // stop/interrupt aiming

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = OffsetCamera))
	void OffsetCamera(bool Forward);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = AimZoom))
	void AimZoom(bool Forward);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = SprintZoom))
	void SprintZoom(bool Forward);

protected:

	ASoSPlayerController* PlayerController;

	FVector RollDirection;

	bool bCanDoubleJump;

	bool bLastOrientRotationToMovement;

	float CurrentStamina;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FPlayerASAbilitiesData AbilityBar;

	void SetStaminaRate(float RatePerSecond);

	void MoveForward(float AxisValue);

	void MoveRight(float AxisValue);

	void SprintStart();

	void CrouchStart();

	void CrouchEnd();

	void DoubleJump();

	void StartRoll();

	void Interact(); // Player interaction/use objects in world

	void Roll(FVector DodgeDirection, bool OrientRotationToMovement);

	void AimStart();

	// Reset air control after preforming double jump
	void ResetAirControl();

	// Trace to see if hit interact-able actor
	bool InteractTrace(AActor* &OutActor) const;

	FVector GetTraceDirection(FVector StartLocation) const;

	UFUNCTION()
	void UseAbility(int32 Index);

	UFUNCTION()
	void IncrementStamina(float Amount);

	UFUNCTION()
	void EndRoll(bool OrientRotationToMovement);

	//Timer handles
	FTimerHandle TimerHandle_DoubleJump;
	FTimerHandle TimerHandle_DodgeEnd;
	FTimerHandle TimerHandle_Stamina;	

public:

	/* Getters and Setters */

	void SetPlayerSpeed(float SpeedMultiplier);

	void SetPlayerMovementType(bool bOrientRotationToMovement, bool bUseControllerDesiredRotation);

	UFUNCTION(BlueprintCallable, Category = "Player")
	USoSInventoryComponent* GetPlayerInventory() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetStamina() const;

	ASoSPlayerController* GetPlayerController() const;
};
