// Fill out your copyright notice in the Description page of Project Settings.

#include "Rifle.h"

// Sets default values
ARifle::ARifle()
{
	FireRate = 0.075f;
	MaxRange = 30000.f;
	bIsAutomatic = true;
	BaseBulletSpread = 2.5f;
	AimingSpreadMultiplier = 0.35;
	MaxAmmo = 999;
	StartAmmo = 999;
	MaxAmmoPerClip = 60;
	ReloadDuration = 2.1f;
}




