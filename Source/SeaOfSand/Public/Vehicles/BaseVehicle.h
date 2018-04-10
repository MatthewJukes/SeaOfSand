// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerInputsInterface.h"
#include "BaseVehicle.generated.h"

class APlayerCharacter;
class UHoverComponent;
class UArrowComponent;

UCLASS()
class SEAOFSAND_API ABaseVehicle : public APawn , public IPlayerInputsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseVehicle();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* VehicleMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hover, meta = (AllowPrivateAccess = "true"))
	UHoverComponent* HoverComponent1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hover, meta = (AllowPrivateAccess = "true"))
	UHoverComponent* HoverComponent2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hover, meta = (AllowPrivateAccess = "true"))
	UHoverComponent* HoverComponent3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hover, meta = (AllowPrivateAccess = "true"))
	UHoverComponent* HoverComponent4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* DriverMesh;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* ForwardArrow;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* UpArrow;

protected:	

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	// Interact while on vehicle/ exit vehicle
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool Interact();
	virtual bool Interact_Implementation() override;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName DriverAttachPoint;

	APlayerCharacter* CurrentDriver;

protected:	
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ForwardThrust = 2800000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BoostMultiplier = 1.6f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float TurningThrust = 45000000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RollOrientStrength = 12000000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float PitchOrientStrength = 30000000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	UCurveFloat* InclineTractionCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	TSubclassOf<APlayerCharacter> PlayerCharacterBP;

	// Movement
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void BoostStart();
	void BoostEnd();
	bool bIsBoosting = false;

	// Adjust linear damping based on traction
	void SetDamping();

	// Attempt to orient vehicle roll to ground plane, use world up vector if in air
	void RollCorrection();

	// Attempt to orient vehicle pitch to ground plane, use world up vector if in air
	void PitchCorrection();

	// Get forward vector of the ground based on hover component traces
	FVector GetGroundForwardVector();

	// Get up vector of the ground based on hover component traces
	FVector GetGroundUpVector();	

	float GetGroundIncline();

	// Get total compression ratio of the hover components
	float GetTotalShortCompressionRatio();
	float GetTotalLongCompressionRatio();

	// Calculate traction amount based on compression ratio
	float GetTractionRatio(bool bUseLongCompressionRatio = false);
};
