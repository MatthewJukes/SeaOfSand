// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon.h"
#include "BaseProjectile.h"
#include "SoSPlayerController.h"
#include "PlayerCharacter.h"
#include "PlayerInventory.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "Math/UnrealMath.h"
#include "Public/TimerManager.h"


// Sets default values
ABaseWeapon::ABaseWeapon()
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
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = Cast<ASoSPlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());

	// Setup ammo
	CurrentAmmo = FMath::Min(StartAmmo, MaxAmmo);
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, StartAmmo);
}

void ABaseWeapon::StartFiring()
{
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ABaseWeapon::HandleFiring, FireRate, bIsAutomatic, 0.0f);
}

void ABaseWeapon::StopFiring()
{
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
	if (WeaponState == EWeaponState::Firing)
	{
		WeaponState = EWeaponState::Idle;
	}
}

void ABaseWeapon::HandleFiring()
{
	if (CheckIfWeaponCanFire(FireRate))
	{
		WeaponState = EWeaponState::Firing;
		UseAmmo();

		FVector HitLocation; // Store hit location		
		FVector AimDirection = GetAimDirection();

		FireProjectile(AimDirection);

		if (WeaponTrace(HitLocation, AimDirection)) // Trace from muzzle to crosshair hit location
		{
			// Hit something
		}

		// Play Audio
		ShotAudioComponent->Play();
	}
	else if (CurrentAmmoInClip == 0 && bCanReload && CurrentAmmo > 0)
	{
		StartReload();
	}
}

bool ABaseWeapon::CheckIfWeaponCanFire(float FireRate)
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

float ABaseWeapon::CalculateBulletSpread()
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

void ABaseWeapon::UseAmmo()
{
	CurrentAmmo--;
	CurrentAmmoInClip--;
}

void ABaseWeapon::StartReload()
{
	if (PlayerCharacter)
	{
		if (WeaponState != EWeaponState::Reloading && CurrentAmmoInClip < MaxAmmoPerClip && !PlayerCharacter->bIsRolling 
			&& !PlayerCharacter->bIsSprinting && PlayerCharacter->PlayerInventory->bWeaponIsDrawn)
		{
			WeaponState = EWeaponState::Reloading;
			GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &ABaseWeapon::ReloadWeapon, ReloadDuration, false);
		}
	}
}

void ABaseWeapon::InterruptReload()
{
	if (WeaponState == EWeaponState::Reloading)
	{
		GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
		WeaponState = EWeaponState::Idle;
	}
}

void ABaseWeapon::ReloadWeapon()
{
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, CurrentAmmo);
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	WeaponState = EWeaponState::Idle;
}

void ABaseWeapon::FireProjectile(FVector AimDirection)
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

bool ABaseWeapon::WeaponTrace(FVector& OutHitlocation, FVector AimDirection) const
{
	const FName TraceTag("WeaponTraceTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;

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

FVector ABaseWeapon::GetAimDirection() const
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



