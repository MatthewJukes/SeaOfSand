// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSShotgun.h"

// Sets default values
ASoSShotgun::ASoSShotgun()
{
	WeaponType = EWeaponType::Shotgun;

	// Firing
	FireRate = 60;
	MaxRange = 10000.f;
	bIsAutomatic = false;
	ProjectilesPerShot = 12;

	// Damage
	BaseDamage = 4.0f;
	VulnerableHitBonusDamage = 1.5f;

	// Aiming
	BaseBulletSpreadRange = FVector2D(4.0f, 4.0f);
	AimingBulletSpreadRange = FVector2D(4.0f, 4.0f);
	RecoilAmount = 0;
	RecoilRecoveryTime = 0;
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



