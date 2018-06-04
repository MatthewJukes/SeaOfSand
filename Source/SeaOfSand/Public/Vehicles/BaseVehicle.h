// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseVehicle.generated.h"

class ASoSPlayerCharacter;
class UHoverComponent;
class UArrowComponent;

UCLASS()
class SEAOFSAND_API ABaseVehicle : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* VehicleMesh;

	UPROPERTY(VisibleAnywhere, Category = Hover)
	UHoverComponent* HoverComponent1;

	UPROPERTY(VisibleAnywhere, Category = Hover)
	UHoverComponent* HoverComponent2;

	UPROPERTY(VisibleAnywhere, Category = Hover)
	UHoverComponent* HoverComponent3;

	UPROPERTY(VisibleAnywhere, Category = Hover)
	UHoverComponent* HoverComponent4;

public:

	// Sets default values for this pawn's properties
	ABaseVehicle();

protected:	

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	ASoSPlayerCharacter * CurrentDriver;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName DriverAttachPoint;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	int32 Speed; // in km/h exposed to BPs for HUD

	UPROPERTY(BlueprintReadOnly, Category = "State")
	float Health; // vehicle health exposed to BPs for HUD // TODO switch to event based update

	UPROPERTY(BlueprintReadOnly, Category = "State")
	float CurrentHeat; // vehicles current heat exposed to BPs for HUD

protected:	
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BaseForwardThrust;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BaseTurningThrust;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	UCurveFloat* InclineTractionCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector2D BoostMultiplierRange;	

	UPROPERTY(EditDefaultsOnly, Category = "Heat") //TODO maybe change system to use curves
	TArray<float> HeatThresholds;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	FVector HeatingRateRange;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	FVector CoolingRateRange;

	UPROPERTY(EditDefaultsOnly, Category = "Heat")
	UCurveFloat* DamageRateCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Corrections")
	float RollOrientStrength;

	UPROPERTY(EditDefaultsOnly, Category = "Corrections")
	float PitchOrientStrength;

	// Movement variables
	float CurrentForwardThrust;
	float CurrentTurningThrust;
	float CurrentBoost;
	bool bIsBoosting;

	UFUNCTION()
	void ExitVehicle();

	// Movement functions
	UFUNCTION()
	void MoveForward(float AxisValue);

	UFUNCTION()
	void MoveRight(float AxisValue);

	UFUNCTION()
	void BoostStart();

	UFUNCTION()
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
