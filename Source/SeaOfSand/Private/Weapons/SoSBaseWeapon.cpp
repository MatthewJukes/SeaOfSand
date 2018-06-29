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

	GetWorldTimerManager().SetTimer(TimerHandle_ReduceRecoil, this, &ASoSBaseWeapon::UpdateRecoil, 1.0f / 60.0f, true, 0.0f);
}

void ASoSBaseWeapon::StartFiring()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->GetTimeSeconds(), 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimerBetweenShots, this, &ASoSBaseWeapon::HandleFiring, TimeBetweenShots, bIsAutomatic, FirstDelay);
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
				if (SurfaceType == SURFACE_FLESHVULNERABLE)
				{
					ActualDamage *= 2.5f;
				}

				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, PlayerController, PlayerCharacter, DamageType);
				
				PlayImpactEffect(SurfaceType, Hit.ImpactPoint);

				TracerEndPoint = Hit.ImpactPoint;
			}

			PlayTracerEffect(TracerEndPoint);
		}

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

void ASoSBaseWeapon::UseAmmo()
{
	CurrentAmmo--;
	CurrentAmmoInClip--;
}

void ASoSBaseWeapon::UpdateRecoil()
{
	CurrentRecoil -= (1.0f / RecoilRecoveryTime) / 60;
	CurrentRecoil = FMath::Clamp(CurrentRecoil, 0.0f, 1.0f);
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
		FVector ShotDirection = ImpactPoint - WeaponMesh->GetSocketLocation(MuzzleSocketName);
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

	if (GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, COLLISION_WEAPON, TraceParams))
	{
		return true;
	}
	return false; // Line-trace didn't hit anything
}

FVector ASoSBaseWeapon::GetAimDirection()
{
	FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);

	FVector AimDirection = PlayerController->GetCrosshairHitLocation(true, MuzzleLocation) - MuzzleLocation;
	AimDirection.Normalize();	

	return FMath().VRandCone(AimDirection, FMath().DegreesToRadians(GetBulletSpread()));
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
	if (bGettingBonus)
	{
		AimingStartTime = GetWorld()->GetTimeSeconds();
	}

	bGettingAccuracyBonus = bGettingBonus;
}

void ASoSBaseWeapon::SetCanReload(bool bReload)
{
	bCanReload = bReload;
}

float ASoSBaseWeapon::GetBulletSpread() const
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

