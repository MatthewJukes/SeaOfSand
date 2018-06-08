// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSShotgun.h"

// Sets default values
ASoSShotgun::ASoSShotgun()
{
	WeaponType = EWeaponType::Shotgun;

	// Firing
	FireRate = 120;
	MaxRange = 10000.f;
	bIsAutomatic = false;
	ProjectilesPerShot = 6;

	// Aiming
	BaseBulletSpread = 2.0f;
	AimingSpreadMultiplier = 1.0f;

	// Ammo
	MaxAmmo = 999;
	StartAmmo = 999;
	MaxAmmoPerClip = 8;
	ReloadDuration = 2.1f;

	// Movement speed
	AimingSpeedMultiplier = 0.6f;
	WeaponDrawnSpeedMultiplier = .8f;
}



