// Fill out your copyright notice in the Description page of Project Settings.

#include "Rifle.h"

// Sets default values
ARifle::ARifle()
{
	// Firing
	FireRate = 0.075f;
	MaxRange = 30000.f;
	bIsAutomatic = true;

	// Aiming
	BaseBulletSpread = 2.5f;
	AimingSpreadMultiplier = 0.35f;

	// Ammo
	MaxAmmo = 999;
	StartAmmo = 999;
	MaxAmmoPerClip = 60;
	ReloadDuration = 2.1f;

	// Movement speed
	AimingSpeedMultiplier = 0.6f;
	WeaponDrawnSpeedMultiplier = 0.8f;
}




