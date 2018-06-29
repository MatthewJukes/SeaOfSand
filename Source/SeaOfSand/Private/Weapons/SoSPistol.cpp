// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSPistol.h"

// Sets default values
ASoSPistol::ASoSPistol()
{
	WeaponType = EWeaponType::Pistol;

	// Firing
	FireRate = 240;
	BaseDamage = 8;
	MaxRange = 30000.f;
	bIsAutomatic = false;
	ProjectilesPerShot = 1;

	// Aiming
	BaseBulletSpreadRange = FVector2D(1.5f, 3.0f);
	AimingBulletSpreadRange = FVector2D(0.25f, 2.0f);
	RecoilAmount = 0.4f;
	RecoilRecoveryTime = 1.2f;
	AimingBulletSpreadLerpTime = 0.6f;

	// Ammo
	MaxAmmo = 999;
	StartAmmo = 999;
	MaxAmmoPerClip = 24;
	ReloadDuration = 2.1f;

	// Movement speed
	AimingSpeedMultiplier = 0.75f;
	WeaponDrawnSpeedMultiplier = 1.f;
}


