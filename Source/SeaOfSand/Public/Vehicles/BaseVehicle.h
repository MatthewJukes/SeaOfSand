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

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* ForwardArrow;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* UpArrow;

protected:

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

protected:	
	
private:

	UPROPERTY(EditDefaultsOnly)
	float ForwardThrust = 2000000.f;

	UPROPERTY(EditDefaultsOnly)
	float BoostMultiplier = 1.6f;

	UPROPERTY(EditDefaultsOnly)
	float TurningThrust = 20000000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	TSubclassOf<APlayerCharacter> PlayerCharacterBP;

	void MoveForward(float AxisValue);

	void MoveRight(float AxisValue);

	void BoostStart();
	void BoostEnd();

	// Get forward vector of the ground based on hover component traces
	FVector CalculateGroundForwardVector();

	// Get up vector of the ground based on hover component traces
	FVector CalculateGroundUpVector();

	void SetDamping();

	float CalculateTotalCompressionRatio();

	// Calculate traction amount based on compression ratio
	float GetTractionRatio();	
};
