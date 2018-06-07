// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoSPlayerInventory.generated.h"

class ASoSPlayerCharacter;
class ASoSBaseWeapon;
class ASoSRifle;
class ASoSPistol;
class ASoSShotgun;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEAOFSAND_API USoSPlayerInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USoSPlayerInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	ASoSBaseWeapon* GetCurrentWeapon();

	bool GetWeaponIsDrawn();

	void SetWeaponIsDrawn(bool WeaponIsDrawn);

	void StartFiring();

	void StopFiring();

	void StartReload();

	void HolsterUnholster();

	void CycleWeapons(bool bNextWeapon = true);

private:

	bool bWeaponIsDrawn;

	int32 CurrentWeaponArrayID;

	TArray<ASoSBaseWeapon*> EquippedWeapons;
	
	ASoSPlayerCharacter* PlayerCharacter;

	ASoSBaseWeapon* CurrentWeapon;

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

	void SpawnWeapon(TSubclassOf<ASoSBaseWeapon> WeaponToSpawn);

	void AttachWeaponToSocket(ASoSBaseWeapon* Weapon, bool bDrawWeapon = false);
};
