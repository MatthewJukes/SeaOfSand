// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSBaseWeapon.h"
#include "SeaOfSand.h"
#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
#include "SoSPlayerInventory.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Math/UnrealMath.h"
#include "Public/TimerManager.h"


// Sets default values
ASoSBaseWeapon::ASoSBaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; 

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	ShotAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ShotAudio"));

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	bCanReload = true;
	SetWeaponState(EWeaponState::Idle);
}

// Called when the game starts or when spawned
void ASoSBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = Cast<ASoSPlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerCharacter = Cast<ASoSPlayerCharacter>(GetOwner());

	TimeBetweenShots = 60.f / FireRate;

	CurrentAmmo = FMath::Min(StartAmmo, MaxAmmo);
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, StartAmmo);
}

void ASoSBaseWeapon::StartFiring()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimerBetweenShots, this, &ASoSBaseWeapon::HandleFiring, TimeBetweenShots, bIsAutomatic, 0.0f);
}

void ASoSBaseWeapon::StopFiring()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimerBetweenShots);
	if (GetCurrentWeaponState() == EWeaponState::Firing)
	{
		SetWeaponState(EWeaponState::Idle);
	}
}

void ASoSBaseWeapon::HandleFiring()
{
	if (CheckIfWeaponCanFire())
	{
		SetWeaponState(EWeaponState::Firing);
		UseAmmo();

		for (int i = 0; i < ProjectilesPerShot; i++)
		{
			FHitResult Hit; // Store hit
			
			FVector TraceStart = WeaponMesh->GetSocketLocation("MuzzleSocket");
			FVector TraceEnd = TraceStart + (GetAimDirection() * MaxRange);

			EPhysicalSurface SurfaceType = SurfaceType_Default;

			FVector TracerEndPoint = TraceEnd;

			if (WeaponTrace(Hit, TraceStart, TraceEnd))
			{
				SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
				
				PlayImpactEffect(SurfaceType, Hit.ImpactPoint);

				TracerEndPoint = Hit.ImpactPoint;
			}

			PlayTracerEffect(TracerEndPoint);
		}

		LastFireTime = GetWorld()->TimeSeconds;

		PlayMuzzleEffect();

		ShotAudioComponent->Play();
	}
	else if (CurrentAmmoInClip == 0 && bCanReload && CurrentAmmo > 0)
	{
		StartReload();
	}
}

bool ASoSBaseWeapon::CheckIfWeaponCanFire()
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->bIsRolling || GetCurrentWeaponState() == EWeaponState::Reloading || CurrentAmmoInClip == 0)
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

float ASoSBaseWeapon::CalculateBulletSpread()
{
	if (bGettingAccuracyBonus)
	{
		return BaseBulletSpread * AimingSpreadMultiplier;
	}
	else
	{
		return BaseBulletSpread;
	}
}

void ASoSBaseWeapon::UseAmmo()
{
	CurrentAmmo--;
	CurrentAmmoInClip--;
}

void ASoSBaseWeapon::StartReload()
{
	if (PlayerCharacter)
	{
		if (GetCurrentWeaponState() != EWeaponState::Reloading && CurrentAmmoInClip < MaxAmmoPerClip && !PlayerCharacter->bIsRolling 
			&& !PlayerCharacter->bIsSprinting && PlayerCharacter->GetPlayerInventory()->GetWeaponIsDrawn())
		{
			SetWeaponState(EWeaponState::Reloading);
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadTime, this, &ASoSBaseWeapon::ReloadWeapon, ReloadDuration, false);
		}
	}
}

void ASoSBaseWeapon::InterruptReload()
{
	if (GetCurrentWeaponState() == EWeaponState::Reloading)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);
		SetWeaponState(EWeaponState::Idle);
	}
}

void ASoSBaseWeapon::ReloadWeapon()
{
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, CurrentAmmo);
	GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);
	SetWeaponState(EWeaponState::Idle);
}

void ASoSBaseWeapon::PlayMuzzleEffect()
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponMesh, MuzzleSocketName);
	}
}

void ASoSBaseWeapon::PlayTracerEffect(FVector TraceEnd)
{
	if (TracerEffect)
	{
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}
}

void ASoSBaseWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint)
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
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

bool ASoSBaseWeapon::WeaponTrace(FHitResult& OutHit, FVector StartLocation, FVector EndLocation) const
{
	const FName TraceTag("WeaponTraceTag");
	//GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.TraceTag = TraceTag;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, ECC_Visibility, TraceParams))
	{
		return true;
	}
	return false; // Line-trace didn't hit anything
}

FVector ASoSBaseWeapon::GetAimDirection() const
{
	FVector AimDirection = PlayerController->GetCrosshairHitLocation() - WeaponMesh->GetSocketLocation("MuzzleSocket");
	AimDirection.Normalize();	

	float BulletSpread = BaseBulletSpread;
	if (bGettingAccuracyBonus)
	{
		BulletSpread = BaseBulletSpread * AimingSpreadMultiplier;
	}
	return FMath().VRandCone(AimDirection, FMath().DegreesToRadians(BulletSpread));
}

/////////////////////////
/* Getters and Setters */
/////////////////////////

EWeaponState ASoSBaseWeapon::GetCurrentWeaponState() const
{
	return CurrentWeaponState;
}

void ASoSBaseWeapon::SetWeaponState(EWeaponState NewState)
{
	if (CurrentWeaponState == NewState)
	{
		return;
	}

	CurrentWeaponState = NewState;
}

EWeaponType ASoSBaseWeapon::GetWeaponType() const
{
	return WeaponType;
}

int32 ASoSBaseWeapon::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

int32 ASoSBaseWeapon::GetCurrentAmmoInClip() const
{
	return CurrentAmmoInClip;
}

float ASoSBaseWeapon::GetWeaponDrawnSpeedMultiplier() const
{
	return WeaponDrawnSpeedMultiplier;
}

float ASoSBaseWeapon::GetAimingSpeedMultiplier() const
{
	return AimingSpeedMultiplier;
}

void ASoSBaseWeapon::SetGettingAccuracyBonus(bool bGettingBonus)
{
	bGettingAccuracyBonus = bGettingBonus;
}

void ASoSBaseWeapon::SetCanReload(bool bReload)
{
	bCanReload = bReload;
}

