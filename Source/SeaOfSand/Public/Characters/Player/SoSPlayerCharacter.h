// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSCharacterBase.h"
#include "SoSPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASoSPlayerController;
class USoSAbilityBase;


USTRUCT(BlueprintType)
struct FPlayerAbilitiesData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilitySprintClass; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilitySprintEndClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityAimEndClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<USoSAbilityBase> AbilityDashClass;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* MeleeAbilityOne;
	
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* MeleeAbilityTwo;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* MeleeAbilityThree;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* RangedAbilityOne;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* RangedAbilityTwo;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* RangedAbilityThree;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* OffhandAbilityOne;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* OffhandAbilityTwo;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AuxAbilityOne;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AuxAbilityTwo;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AuxAbilityThree;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilitySprint;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilitySprintEnd;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilityAimEnd;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	USoSAbilityBase* AbilityDash;
}; 

DECLARE_DELEGATE_TwoParams(FUseAbilityDelegate, int32, bool);

UCLASS()
class SEAOFSAND_API ASoSPlayerCharacter : public ASoSCharacterBase
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

public:

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

	bool bCanDoubleJump;

	bool bCanAirDash;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractTraceRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FPlayerAbilitiesData AbilityBar;

	virtual bool UseAbility(USoSAbilityBase* Ability, bool bReleashed = false);

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
	void UseAbilityActionBinding(int32 Index, bool bReleased);

	//Timer handles
	FTimerHandle TimerHandle_DoubleJump;

public:

	/* Getters and Setters */

	ASoSPlayerController* GetPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
	FPlayerAbilitiesData& GetASAbilityBar();
};
