// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSBaseWeapon.h"
#include "BaseProjectile.h"
#include "SoSPlayerController.h"
#include "SoSPlayerCharacter.h"
#include "SoSPlayerInventory.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
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
			FVector HitLocation; // Store hit location		
			FVector AimDirection = GetAimDirection();

			if (WeaponTrace(HitLocation, AimDirection)) // Trace from muzzle to crosshair hit location
			{
				// Hit something
			}
		}

		LastFireTime = GetWorld()->TimeSeconds;

		// Play Audio
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

bool ASoSBaseWeapon::WeaponTrace(FVector& OutHitlocation, FVector AimDirection) const
{
	const FName TraceTag("WeaponTraceTag");
	//GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.TraceTag = TraceTag;

	FHitResult Hit;	
	FVector StartLocation = WeaponMesh->GetSocketLocation("MuzzleSocket");
	FVector EndLocation = StartLocation + (AimDirection * MaxRange);

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, TraceParams))
	{
		OutHitlocation = Hit.Location;
		return true;
	}
	OutHitlocation = EndLocation;
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

