// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSRangedWeapon.h"
#include "SeaOfSand.h"
#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
#include "SoSInventoryComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Math/UnrealMath.h"
#include "Public/TimerManager.h"
#include "DrawDebugHelpers.h"


// Sets default values
ASoSRangedWeapon::ASoSRangedWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; 

	ShotAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ShotAudio"));

	bCanReload = true;
}

// Called when the game starts or when spawned
void ASoSRangedWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60.f / FireRate;

	CurrentAmmo = FMath::Min(StartAmmo, MaxAmmo);
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, StartAmmo);
}

void ASoSRangedWeapon::StartAttack()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->GetTimeSeconds(), 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimerBetweenShots, this, &ASoSRangedWeapon::HandleFiring, TimeBetweenShots, bIsAutomatic, FirstDelay);
}

void ASoSRangedWeapon::EndAttack()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimerBetweenShots);
	if (GetWeaponState() == EWeaponState::Attacking)
	{
		SetWeaponState(EWeaponState::Idle);
	}
}

void ASoSRangedWeapon::HandleFiring()
{
	if (CheckIfWeaponCanFire())
	{
		SetWeaponState(EWeaponState::Attacking);
		UseAmmo();

		PlayerCharacter->UseAbility(WeaponAbilities.AbilityWeaponPrimary);

		LastFireTime = GetWorld()->GetTimeSeconds();

		PlayMuzzleEffect();

		ShotAudioComponent->Play();
	}
	else if (CurrentAmmoInClip == 0 && bCanReload && CurrentAmmo > 0)
	{
		StartReload();
	}
}

bool ASoSRangedWeapon::CheckIfWeaponCanFire()
{
	if (PlayerCharacter)
	{
		if (GetWeaponState() == EWeaponState::Reloading || CurrentAmmoInClip == 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}

void ASoSRangedWeapon::UseAmmo()
{
	CurrentAmmo--;
	CurrentAmmoInClip--;
}

void ASoSRangedWeapon::StartReload()
{
	if (PlayerCharacter)
	{
		if (GetWeaponState() != EWeaponState::Reloading && CurrentAmmoInClip < MaxAmmoPerClip
			&& WeaponState != EWeaponState::Holstered)
		{
			SetWeaponState(EWeaponState::Reloading);
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadTime, this, &ASoSRangedWeapon::ReloadWeapon, ReloadDuration, false);
		}
	}
}

void ASoSRangedWeapon::InterruptReload()
{
	if (GetWeaponState() == EWeaponState::Reloading)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);
		SetWeaponState(EWeaponState::Idle);
	}
}

void ASoSRangedWeapon::ReloadWeapon()
{
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, CurrentAmmo);
	GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);
	SetWeaponState(EWeaponState::Idle);
}

void ASoSRangedWeapon::PlayMuzzleEffect()
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponMesh, ProjectileOriginSocketName);
	}
}


///////////////////////////////////////////////////
// Getters and Setters


int32 ASoSRangedWeapon::GetCurrentAmmo() const
{
	return CurrentAmmo;
}


int32 ASoSRangedWeapon::GetCurrentAmmoInClip() const
{
	return CurrentAmmoInClip;
}


void ASoSRangedWeapon::SetCanReload(bool bReload)
{
	bCanReload = bReload;
}

