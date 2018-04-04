// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerInputsInterface.h"
#include "BaseVehicle.generated.h"

class APlayerCharacter;

UCLASS()
class SEAOFSANDS_API ABaseVehicle : public APawn , public IPlayerInputsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseVehicle();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* VehicleMesh;

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
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	TSubclassOf<APlayerCharacter> PlayerCharacterBP;
};
