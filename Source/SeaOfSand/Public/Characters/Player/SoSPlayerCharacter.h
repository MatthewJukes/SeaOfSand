// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SoSPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASoSPlayerController;
class USoSInventoryComponent;
class USoSCombatComponent;
class USoSAbilityBase;


USTRUCT(BlueprintType)
struct FPlayerAbilitiesData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityOneClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityTwoClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityThreeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityFourClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityFiveClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilitySixClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilitySevenClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityEightClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilitySprintClass; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilitySprintEndClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityAimEndClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityDashClass;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilityOne;
	
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilityTwo;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilityThree;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilityFour;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilityFive;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilitySix;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilitySeven;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilityEight;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilitySprint;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilitySprintEnd;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilityAimEnd;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilityDash;
}; 

DECLARE_DELEGATE_OneParam(FUseAbilityDelegate, int32);

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
	USoSCombatComponent* CombatComp;

public:

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsDoubleJumping;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bInVehicle;

	// Turn collision back on
	void EnableCollsion();
	
	void SprintEnd(); // stop/interrupt sprinting

	void AimEnd(); // stop/interrupt aiming

	bool UseAbility(USoSAbilityBase* Ability);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = OffsetCamera))
	void OffsetCamera(bool Forward);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = AimZoom))
	void AimZoom(bool Forward);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = SprintZoom))
	void SprintZoom(bool Forward);

protected:

	ASoSPlayerController* PlayerController;

	FVector AimHitLocation;

	bool bCanDoubleJump;

	bool bCanAirDash;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractTraceRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FPlayerAbilitiesData AbilityBar;

	void MoveForward(float AxisValue);

	void MoveRight(float AxisValue);

	void SprintStart();

	void CrouchStart();

	void CrouchEnd();

	void DoubleJump();

	void StartDash();

	void Interact(); // Player interaction/use objects in world

	void AimStart();

	// Reset air control after preforming double jump
	void ResetAirControl();

	// Trace to see if hit interact-able actor
	bool InteractTrace(AActor* &OutActor) const;

	FVector GetTraceDirection(FVector StartLocation) const;

	UFUNCTION()
	void UseAbilityActionBinding(int32 Index);

	//Timer handles
	FTimerHandle TimerHandle_DoubleJump;

public:

	/* Getters and Setters */

	ASoSPlayerController* GetPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
	USoSInventoryComponent* GetPlayerInventory() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
	USoSCombatComponent* GetPlayerCombatComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
	FPlayerAbilitiesData& GetASAbilityBar() ;

	void SetPlayerMovementType(bool bOrientRotationToMovement, bool bUseControllerDesiredRotation);
};
