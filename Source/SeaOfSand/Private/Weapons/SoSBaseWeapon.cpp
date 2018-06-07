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
	WeaponState = EWeaponState::Idle;
}

// Called when the game starts or when spawned
void ASoSBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = Cast<ASoSPlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerCharacter = Cast<ASoSPlayerCharacter>(GetOwner());

	// Setup ammo
	CurrentAmmo = FMath::Min(StartAmmo, MaxAmmo);
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, StartAmmo);
}

void ASoSBaseWeapon::StartFiring()
{
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ASoSBaseWeapon::HandleFiring, FireRate, bIsAutomatic, 0.0f);
}

void ASoSBaseWeapon::StopFiring()
{
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
	if (WeaponState == EWeaponState::Firing)
	{
		WeaponState = EWeaponState::Idle;
	}
}

void ASoSBaseWeapon::HandleFiring()
{
	if (CheckIfWeaponCanFire(FireRate))
	{
		WeaponState = EWeaponState::Firing;
		UseAmmo();

		for (int i = 0; i < ProjectilesPerShot; i++)
		{
			FVector HitLocation; // Store hit location		
			FVector AimDirection = GetAimDirection();

			FireProjectile(AimDirection);

			if (WeaponTrace(HitLocation, AimDirection)) // Trace from muzzle to crosshair hit location
			{
				// Hit something
			}
		}

		// Play Audio
		ShotAudioComponent->Play();
	}
	else if (CurrentAmmoInClip == 0 && bCanReload && CurrentAmmo > 0)
	{
		StartReload();
	}
}

bool ASoSBaseWeapon::CheckIfWeaponCanFire(float FireRate)
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->bIsRolling || WeaponState == EWeaponState::Reloading)
		{
			return false;
		}
		else if (GetWorldTimerManager().GetTimerRemaining(RefireTimerHandle) <= 0.001f && CurrentAmmoInClip > 0)
		{
			// Reset refire timer
			GetWorldTimerManager().SetTimer(RefireTimerHandle, FireRate, false);
			return true;
		}
	}
	return false;
}

float ASoSBaseWeapon::CalculateBulletSpread()
{
	if (bAimingBonus)
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
		if (WeaponState != EWeaponState::Reloading && CurrentAmmoInClip < MaxAmmoPerClip && !PlayerCharacter->bIsRolling 
			&& !PlayerCharacter->bIsSprinting && PlayerCharacter->GetPlayerInventory()->GetWeaponIsDrawn())
		{
			WeaponState = EWeaponState::Reloading;
			GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &ASoSBaseWeapon::ReloadWeapon, ReloadDuration, false);
		}
	}
}

void ASoSBaseWeapon::InterruptReload()
{
	if (WeaponState == EWeaponState::Reloading)
	{
		GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
		WeaponState = EWeaponState::Idle;
	}
}

void ASoSBaseWeapon::ReloadWeapon()
{
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, CurrentAmmo);
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	WeaponState = EWeaponState::Idle;
}

void ASoSBaseWeapon::FireProjectile(FVector AimDirection)
{
	if (ProjectileBlueprint)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			// Spawn projectile at muzzle
			ABaseProjectile* Projectile = World->SpawnActor<ABaseProjectile>(ProjectileBlueprint, WeaponMesh->GetSocketLocation("MuzzleSocket"), AimDirection.Rotation(), SpawnParams);
		}
	}
}

bool ASoSBaseWeapon::WeaponTrace(FVector& OutHitlocation, FVector AimDirection) const
{
	const FName TraceTag("WeaponTraceTag");
	//GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.TraceTag = TraceTag;

	FHitResult RV_Hit;	
	FVector StartLocation = WeaponMesh->GetSocketLocation("MuzzleSocket");
	FVector EndLocation = StartLocation + (AimDirection * MaxRange);

	if (GetWorld()->LineTraceSingleByChannel(RV_Hit, StartLocation, EndLocation, ECC_Visibility, RV_TraceParams))
	{
		OutHitlocation = RV_Hit.Location;
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
	if (bAimingBonus)
	{
		BulletSpread = BaseBulletSpread * AimingSpreadMultiplier;
	}
	return FMath().VRandCone(AimDirection, FMath().DegreesToRadians(BulletSpread));
}



