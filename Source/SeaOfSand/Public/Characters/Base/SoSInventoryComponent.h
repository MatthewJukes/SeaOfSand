// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoSInventoryComponent.generated.h"

class ASoSCharacterBase;
class ASoSWeaponBase;
class ASoSRangedWeapon;
class ASoSMeleeWeapon;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponSwitch);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEAOFSAND_API USoSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USoSInventoryComponent();

protected:
	virtual void OnComponentCreated() override;

	virtual void BeginPlay() override;

public:

	void HolsterUnholster(bool bQuickSwitch = false);

	void CycleWeapons(bool bNextWeapon = true);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponSwitch OnWeaponSwitch;

private:

	int32 CurrentWeaponArrayID;

	TArray<ASoSWeaponBase*> EquippedWeapons;
	
	ASoSCharacterBase* OwningCharacter;

	ASoSWeaponBase* CurrentWeapon;

	ASoSRangedWeapon* RangedWeapon;

	ASoSMeleeWeapon* MeleeWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName RightHandAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName LeftHipAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName RightHipAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ASoSRangedWeapon> RangedWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ASoSMeleeWeapon> MeleeWeaponClass;

	void SpawnWeapon(TSubclassOf<ASoSWeaponBase> WeaponToSpawn);

	void AttachWeaponToSocket(ASoSWeaponBase* Weapon, bool bDrawWeapon = false);

public:

	/* Getters and Setters */
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ASoSWeaponBase* GetCurrentWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ASoSRangedWeapon* GetRangedWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ASoSMeleeWeapon* GetMeleeWeapon() const;
};
