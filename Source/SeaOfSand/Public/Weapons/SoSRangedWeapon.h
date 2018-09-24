// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSWeaponBase.h"
#include "SoSRangedWeapon.generated.h"

class UAudioComponent;
class USoSASAbilityBase;


UCLASS()
class SEAOFSAND_API ASoSRangedWeapon : public ASoSWeaponBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoSRangedWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UAudioComponent* ShotAudioComponent;

public:	

	void StartAttack() override;

	void StartReload();

	void InterruptReload();

protected:	

	int32 CurrentAmmo;

	int32 CurrentAmmoInClip;

	bool bCanReload;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	int32 MaxAmmoPerClip;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	int32 StartAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo")
	float ReloadDuration;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Effects")
	UParticleSystem* MuzzleEffect;

	void HandleFiring();

	bool CheckIfWeaponCanFire();

	void UseAmmo();
	
	void ReloadWeapon();

	void PlayMuzzleEffect();	

	FTimerHandle TimerHandle_ReloadTime;

public:

	/* Getters and Setters */

	void SetCanReload(bool bReload);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentAmmoInClip() const;
};
