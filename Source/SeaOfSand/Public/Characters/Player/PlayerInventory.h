// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInventory.generated.h"

class APlayerCharacter;
class ABaseWeapon;
class ASoSRifle;
class ASoSPistol;
class ASoSShotgun;

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
	void CycleWeapons(bool bNextWeapon = true);

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	bool bWeaponIsDrawn;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	bool bIsThing;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	ABaseWeapon* CurrentWeapon; // Currently selected weapon

private:

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName RightHandAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName RifleHolsterAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName PistolHolsterAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName ShotgunHolsterAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ASoSRifle> RifleBlueprint;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ASoSPistol> PistolBlueprint;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ASoSShotgun> ShotgunBlueprint;

	int32 CurrentWeaponArrayID;

	TArray<ABaseWeapon*> EquippedWeapons;
	
	APlayerCharacter* PlayerCharacter; // Inventory owner

	void SpawnWeapon(TSubclassOf<ABaseWeapon> WeaponToSpawn);

	void AttachWeaponToSocket(ABaseWeapon* Weapon, bool bDrawWeapon = false);
};
