// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSRifle.h"

// Sets default values
ASoSRifle::ASoSRifle()
{
	WeaponType = EWeaponType::Rifle;

	// Firing
	FireRate = 60;
	BaseDamage = 25;
	MaxRange = 30000.f;
	bIsAutomatic = false;
	ProjectilesPerShot = 1;

	// Aiming
	BaseBulletSpreadRange = FVector2D(2.0f, 5.0f);
	AimingBulletSpreadRange = FVector2D(0.1f, 2.5f);
	RecoilAmount = 0.8f;
	RecoilRecoveryTime = 2.0f;
	AimingBulletSpreadLerpTime = 1.2;

	// Ammo
	MaxAmmo = 999;
	StartAmmo = 999;
	MaxAmmoPerClip = 12;
	ReloadDuration = 2.1f;

	// Movement speed
	AimingSpeedMultiplier = 0.6f;
	WeaponDrawnSpeedMultiplier = 0.8f;
}




