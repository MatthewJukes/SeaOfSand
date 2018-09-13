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

	TracerTargetName = "Target";

	bCanReload = true;
}

// Called when the game starts or when spawned
void ASoSRangedWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60.f / FireRate;

	CurrentAmmo = FMath::Min(StartAmmo, MaxAmmo);
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, StartAmmo);

	GetWorldTimerManager().SetTimer(TimerHandle_ReduceRecoil, this, &ASoSRangedWeapon::UpdateRecoil, 1.0f / 60.0f, true, 0.0f);
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
		/*
		for (int i = 0; i < ProjectilesPerShot; i++)
		{ 
			FHitResult Hit; // Store hit
			
			FVector ShotDirection = GetAimDirection();
			FVector TraceStart = WeaponMesh->GetSocketLocation("MuzzleSocket");
			FVector TraceEnd = TraceStart + (ShotDirection * MaxRange);

			EPhysicalSurface SurfaceType = SurfaceType_Default;

			FVector TracerEndPoint = TraceEnd;

			if (WeaponTrace(Hit, TraceStart, TraceEnd))
			{
				AActor* HitActor = Hit.GetActor();

				SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

				float ActualDamage = BaseDamage;
				if (SurfaceType == SURFACE_FLESHVULNERABLE  || SurfaceType == SURFACE_CONSTRUCTVULNERABLE)
				{
					ActualDamage *= VulnerableHitBonusDamage;
				}				

				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, PlayerController, PlayerCharacter, DamageType);

				if (SurfaceType != SurfaceType_Default)
				{
					DrawDebugString(GetWorld(), Hit.Location, FString::SanitizeFloat(ActualDamage), nullptr, FColor::White, 0.2f);
				}
				
				PlayImpactEffect(SurfaceType, Hit.ImpactPoint);

				TracerEndPoint = Hit.ImpactPoint;
			}

			PlayTracerEffect(TracerEndPoint); 
		}  */

		LastFireTime = GetWorld()->GetTimeSeconds();

		CurrentRecoil += RecoilAmount;
		CurrentRecoil = FMath::Clamp(CurrentRecoil, 0.0f, 1.0f);

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

void ASoSRangedWeapon::UpdateRecoil()
{
	CurrentRecoil -= (1.0f / RecoilRecoveryTime) / 60;
	CurrentRecoil = FMath::Clamp(CurrentRecoil, 0.0f, 1.0f);
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

void ASoSRangedWeapon::PlayTracerEffect(FVector TraceEnd)
{
	if (TracerEffect)
	{
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation(ProjectileOriginSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}
}

void ASoSRangedWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector ShotDirection = ImpactPoint - WeaponMesh->GetSocketLocation(ProjectileOriginSocketName);
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

FVector ASoSRangedWeapon::GetAimDirection()
{
	FVector MuzzleLocation = WeaponMesh->GetSocketLocation(ProjectileOriginSocketName);

	FVector AimDirection = PlayerController->GetCrosshairHitLocation(true, MuzzleLocation) - MuzzleLocation;
	AimDirection.Normalize();	

	return FMath().VRandCone(AimDirection, FMath().DegreesToRadians(GetBulletSpread()));
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


float ASoSRangedWeapon::GetWeaponDrawnSpeedMultiplier() const
{
	return WeaponDrawnSpeedMultiplier;
}


float ASoSRangedWeapon::GetAimingSpeedMultiplier() const
{
	return AimingSpeedMultiplier;
}


void ASoSRangedWeapon::SetGettingAccuracyBonus(bool bGettingBonus)
{
	if (bGettingBonus)
	{
		AimingStartTime = GetWorld()->GetTimeSeconds();
	}

	bGettingAccuracyBonus = bGettingBonus;
}


void ASoSRangedWeapon::SetCanReload(bool bReload)
{
	bCanReload = bReload;
}


float ASoSRangedWeapon::GetBulletSpread() const
{
	float BulletSpread = FMath::Lerp(BaseBulletSpreadRange.X, BaseBulletSpreadRange.Y, CurrentRecoil);

	if (bGettingAccuracyBonus)
	{
		FVector2D InputRange = FVector2D(0.0f, AimingBulletSpreadLerpTime);
		FVector2D OutputRange = FVector2D(0.0f, 1.0f);

		float Alpha = FMath::GetMappedRangeValueClamped(InputRange, OutputRange, GetWorld()->GetTimeSeconds() - AimingStartTime);
		BulletSpread = FMath::Lerp(BulletSpread, FMath::Lerp(AimingBulletSpreadRange.X, AimingBulletSpreadRange.Y, CurrentRecoil), Alpha);
	}

	return BulletSpread;
}

