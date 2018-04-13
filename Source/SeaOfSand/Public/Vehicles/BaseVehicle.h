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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true")) //TODO check if these still need to be BP accessable
	UStaticMeshComponent* VehicleMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hover, meta = (AllowPrivateAccess = "true"))
	UHoverComponent* HoverComponent1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hover, meta = (AllowPrivateAccess = "true"))
	UHoverComponent* HoverComponent2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hover, meta = (AllowPrivateAccess = "true"))
	UHoverComponent* HoverComponent3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hover, meta = (AllowPrivateAccess = "true"))
	UHoverComponent* HoverComponent4;

protected:	

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	APlayerCharacter * CurrentDriver;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName DriverAttachPoint;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	int32 Speed; // in km/h exposed to BPs for HUD

	UPROPERTY(BlueprintReadOnly, Category = "State")
	float Health; // vehicle health exposed to BPs for HUD // TODO switch to event based update

	UPROPERTY(BlueprintReadOnly, Category = "State")
	float CurrentHeat; // vehicles current heat exposed to BPs for HUD

	// Interact while on vehicle/ exit vehicle
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool Interact();
	virtual bool Interact_Implementation() override;	

protected:	
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BaseForwardThrust = 2800000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BaseTurningThrust = 45000000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	UCurveFloat* InclineTractionCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector2D BoostMultiplierRange = FVector2D(1.5f, 2.0f);	

	UPROPERTY(EditDefaultsOnly, Category = "Heat") //TODO maybe change system to use curves
	TArray<float> HeatThresholds = { 40.f, 100.f, 140.f, 160.f, 180.f }; //  temperature min at idle, normal running temperature, temperature for max boost, damage threshold and soft cap, in celsius

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	FVector HeatingRateRange = FVector(10.f, 8.f, 0.1f); // per second

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	FVector CoolingRateRange = FVector(4.f, 6.f, 10.f); // per second

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	UCurveFloat* DamageRateCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Corrections")
	float RollOrientStrength = 12000000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Corrections")
	float PitchOrientStrength = 30000000.f;


	// Movement variables
	float CurrentForwardThrust;
	float CurrentTurningThrust;
	float CurrentBoost = 1.f;
	bool bIsBoosting = false;

	// Movement functions
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void BoostStart();
	void BoostEnd();
	void Boost();
	void SetDamping(); // Adjust linear damping based on traction

	// Heat
	void CheckHeatLevels();
	void SetHeat();

	// Orientation corrections
	void RollCorrection(); // Attempt to orient vehicle roll to ground plane, use world up vector if in air	
	void PitchCorrection(); // Attempt to orient vehicle pitch to ground plane, use world up vector if in air

	// Ground plane calculations
	FVector GetGroundForwardVector() const; // Get forward vector of the ground based on hover component traces	
	FVector GetGroundUpVector() const; // Get up vector of the ground based on hover component traces	
	float GetGroundIncline() const; // Get the incline to the current slope the vehicle is on
	float GetTractionRatio(const bool bUseLongCompressionRatio = false) const; // Calculate traction amount based on compression ratio
	
	// Get total compression ratios of the hover components
	float GetTotalShortCompressionRatio() const;
	float GetTotalLongCompressionRatio() const;	

	// Timer handles
	FTimerHandle BoostTimerHandle;
	FTimerHandle HeatTimerHandle;
};
