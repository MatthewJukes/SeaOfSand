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
	BaseBulletSpread = 3.0f;
	AimingBulletSpread = 0.1f;
	AimingBulletSpreadLerpTime = 1.8;

	// Ammo
	MaxAmmo = 999;
	StartAmmo = 999;
	MaxAmmoPerClip = 12;
	ReloadDuration = 2.1f;

	// Movement speed
	AimingSpeedMultiplier = 0.6f;
	WeaponDrawnSpeedMultiplier = 0.8f;
}




