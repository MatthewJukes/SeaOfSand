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

UENUM(BlueprintType)
enum class EASAbilityIndex : uint8
{
	ASAbilityOne,
	ASAbilityTwo,
	ASAbilityThree,
	ASAbilityFour,
	ASAbilityFive,
	ASAbilitySix,
	ASAbilitySeven,
	ASAbilityEight,
	ASSprint,
	ASSprintEnd,
	ASAim,
	ASAimEnd
};

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<USoSASAbilityBase> AbilityAim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<USoSASAbilityBase> AbilityAimEnd;
}; 

DECLARE_DELEGATE_OneParam(FUseAbilityDelegate, EASAbilityIndex);

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

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BaseSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractTraceRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FPlayerASAbilitiesData AbilityBar;

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
	void UseAbility(EASAbilityIndex Index);

	UFUNCTION()
	void EndRoll(bool OrientRotationToMovement);

	//Timer handles
	FTimerHandle TimerHandle_DoubleJump;
	FTimerHandle TimerHandle_DodgeEnd;

public:

	/* Getters and Setters */

	void SetPlayerSpeed(float SpeedMultiplier);

	void SetPlayerMovementType(bool bOrientRotationToMovement, bool bUseControllerDesiredRotation);

	UFUNCTION(BlueprintCallable, Category = "Player")
	USoSInventoryComponent* GetPlayerInventory() const;

	ASoSPlayerController* GetPlayerController() const;
};
