// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSShotgun.h"

// Sets default values
ASoSShotgun::ASoSShotgun()
{
	WeaponType = EWeaponType::Shotgun;

	// Firing
	FireRate = 60;
	BaseDamage = 4;
	MaxRange = 10000.f;
	bIsAutomatic = false;
	ProjectilesPerShot = 12;

	// Aiming
	BaseBulletSpread = 4.0f;
	AimingBulletSpread = 4.0f;
	AimingBulletSpreadLerpTime = 1;

	// Ammo
	MaxAmmo = 999;
	StartAmmo = 999;
	MaxAmmoPerClip = 8;
	ReloadDuration = 2.1f;

	// Movement speed
	AimingSpeedMultiplier = 0.6f;
	WeaponDrawnSpeedMultiplier = .8f;
}



