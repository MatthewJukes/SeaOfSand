// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInventory.generated.h"

class APlayerCharacter;
class ABaseWeapon;
class ARifle;

enum class ECurrentWeaponType : uint8
{
	Pistol,
	Rifle,
	Shotgun,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEAOFSAND_API UPlayerInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:		

	void HolsterUnholster(); // Weapon holstering

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	bool bWeaponIsDrawn;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	ABaseWeapon* CurrentWeapon; // Currently selected weapon

private:

	ECurrentWeaponType CurrentWeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName RightHandAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName RifleHolsterAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ARifle> RifleBlueprint;
	
	APlayerCharacter* PlayerCharacter; // Inventory owner

	void SpawnWeapon();

	void AttachWeaponToSocket(bool bDrawWeapon = false);
};
