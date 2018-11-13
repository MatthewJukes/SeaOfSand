// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSCharacterBase.h"
#include "SoSPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASoSPlayerController;
class ASoSPlayerAbilityTarget;
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

	// Turn collision back on
	void EnableCollsion();

	void PrimaryAttackStart();

	void PrimaryAttackEnd();

	void AlternateAttackStart();

	void AlternateAttackEnd();
	
	void SprintEnd(); // stop/interrupt sprinting

	void AimEnd(); // stop/interrupt aiming

	UFUNCTION()
	void TargetingModeEnd();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = OffsetCamera))
	void OffsetCamera(bool Forward);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = AimZoom))
	void AimZoom(bool Forward);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = SprintZoom))
	void SprintZoom(bool Forward);

protected:

	ASoSPlayerController* PlayerController;

	bool bIsDoubleJumping;

	bool bCanDoubleJump;

	bool bCanAirDash;

	bool bTargetingModeActive;

	bool bInVehicle;

	float ClassSpecificFloat;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractTraceRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FPlayerAbilitiesData AbilityBar;

	USoSAbilityBase* CurrentAbilityTargeting;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ASoSPlayerAbilityTarget> BP_AbilityTarget;

	ASoSPlayerAbilityTarget* AbilityTarget;

	bool UseAbility(USoSAbilityBase* Ability, bool bReleased = false);

	void MoveForward(float AxisValue);

	void MoveRight(float AxisValue);

	void SprintStart();

	void CrouchStart();

	void CrouchEnd();

	void DoubleJump();

	void DashStart();

	void TargetingModeStart();

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

	UCameraComponent* GetFollowCamera();

	UFUNCTION(BlueprintCallable, Category = "Player")
	FPlayerAbilitiesData& GetASAbilityBar();

	UFUNCTION(BlueprintCallable, Category = "Player")
	bool GetIsDoubleJumping() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
	bool GetInVehicle() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
	bool GetTargetingModeActive() const;

	float GetClassSpecificFloat() const;

	void SetInVehicle(bool NewBool);
};
