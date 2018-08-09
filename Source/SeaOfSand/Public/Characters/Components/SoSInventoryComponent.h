// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoSInventoryComponent.generated.h"

class ASoSPlayerCharacter;
class ASoSWeaponBase;
class ASoSRangedWeapon;
class ASoSMeleeWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEAOFSAND_API USoSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USoSInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	void HolsterUnholster();

	void CycleWeapons(bool bNextWeapon = true);

private:

	bool bWeaponIsDrawn;

	int32 CurrentWeaponArrayID;

	TArray<ASoSWeaponBase*> EquippedWeapons;
	
	ASoSPlayerCharacter* PlayerCharacter;

	ASoSWeaponBase* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName RightHandAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName LeftHipAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName RightHipAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ASoSRangedWeapon> RangedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ASoSMeleeWeapon> MeleeWeapon;

	void SpawnWeapon(TSubclassOf<ASoSWeaponBase> WeaponToSpawn);

	void AttachWeaponToSocket(ASoSWeaponBase* Weapon, bool bDrawWeapon = false);

public:

	/* Getters and Setters */

	void SetWeaponIsDrawn(bool WeaponIsDrawn);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ASoSWeaponBase* GetCurrentWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool GetWeaponIsDrawn() const;
};
