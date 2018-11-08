// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SoSCharacterBase.generated.h"

class USoSInventoryComponent;
class USoSCombatComponent;

UCLASS()
class SEAOFSAND_API ASoSCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASoSCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USoSInventoryComponent* InventoryComp;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USoSCombatComponent* CombatComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	FHitResult AimHitResult;

	FVector TargetedLocation;

public:

	/* Getters and Setters */

	UFUNCTION(BlueprintCallable, Category = "Player")
	USoSInventoryComponent* GetCharacterInventory() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
	USoSCombatComponent* GetCharacterCombatComponent() const;

	FHitResult GetAimHitResult() const;

	FVector GetTargetedLocation() const;

	void SetCharacterMovementType(bool bOrientRotationToMovement, bool bUseControllerDesiredRotation);
};
