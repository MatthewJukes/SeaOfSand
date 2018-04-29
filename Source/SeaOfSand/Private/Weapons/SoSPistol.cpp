// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSPistol.h"

// Sets default values
ASoSPistol::ASoSPistol()
{
	WeaponType = EWeaponType::Pistol;

	// Firing
	FireRate = 0.1f;
	MaxRange = 30000.f;
	bIsAutomatic = false;
	ProjectilesPerShot = 1;

	// Aiming
	BaseBulletSpread = 1.5f;
	AimingSpreadMultiplier = 0.4f;

	// Ammo
	MaxAmmo = 999;
	StartAmmo = 999;
	MaxAmmoPerClip = 24;
	ReloadDuration = 2.1f;

	// Movement speed
	AimingSpeedMultiplier = 0.75f;
	WeaponDrawnSpeedMultiplier = 1.f;
}


