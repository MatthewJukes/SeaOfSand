// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSRangedWeapon.h"
#include "SeaOfSand.h"
#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
#include "SoSInventoryComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Public/TimerManager.h"


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

	CurrentAmmo = FMath::Min(StartAmmo, MaxAmmo);
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, StartAmmo);
}

void ASoSRangedWeapon::StartAttack()
{
	HandleFiring();
}

void ASoSRangedWeapon::HandleFiring()
{
	if (CheckIfWeaponCanFire())
	{
		if (PlayerCharacter->UseAbility(WeaponAbilities.AbilityWeaponPrimaryInstance))
		{
			SetWeaponState(EWeaponState::Attacking);

			UseAmmo();		

			PlayMuzzleEffect();

			ShotAudioComponent->Play();
		}
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
		if (WeaponState != EWeaponState::Reloading && CurrentAmmoInClip < MaxAmmoPerClip && WeaponState != EWeaponState::Holstered)
		{
			WeaponState = EWeaponState::Reloading;
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadTime, this, &ASoSRangedWeapon::ReloadWeapon, ReloadDuration, false);
		}
	}
}

void ASoSRangedWeapon::InterruptReload()
{
	if (WeaponState == EWeaponState::Reloading)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);
		WeaponState =EWeaponState::Idle;
	}
}

void ASoSRangedWeapon::ReloadWeapon()
{
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, CurrentAmmo);
	GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);
	WeaponState = EWeaponState::Idle;
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

