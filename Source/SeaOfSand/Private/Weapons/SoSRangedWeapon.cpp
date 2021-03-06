// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSRangedWeapon.h"
#include "SeaOfSand.h"
#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
#include "SoSInventoryComponent.h"
#include "SoSCombatComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Public/TimerManager.h"


// Sets default values
ASoSRangedWeapon::ASoSRangedWeapon()
{
	PrimaryActorTick.bCanEverTick = false; 

	WeaponType = EWeaponType::Ranged;

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


void ASoSRangedWeapon::EndAttack()
{
	if (GetWeaponState() == EWeaponState::Attacking)
	{
		if (OwningCharacter->GetCharacterCombatComponent()->UseAbility(WeaponAbilities.AbilityWeaponPrimary, true))
		{
			SetWeaponState(EWeaponState::Idle);

			UseAmmo();

			PlayMuzzleEffect();

			ShotAudioComponent->Play();
		}
	}
}


void ASoSRangedWeapon::HandleFiring()
{
	if (CheckIfWeaponCanFire())
	{
		if (OwningCharacter->GetCharacterCombatComponent()->UseAbility(WeaponAbilities.AbilityWeaponPrimary))
		{
			SetWeaponState(EWeaponState::Attacking);
		}
	}
	else if (CurrentAmmoInClip == 0 && bCanReload && CurrentAmmo > 0)
	{
		StartReload();
	}
}


bool ASoSRangedWeapon::CheckIfWeaponCanFire()
{
	if (OwningCharacter)
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
	if (OwningCharacter)
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

