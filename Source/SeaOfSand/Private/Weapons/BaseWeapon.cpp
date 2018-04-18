// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon.h"
#include "BasePlayerController.h"
#include "PlayerCharacter.h"
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
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = Cast<ABasePlayerController>(GetWorld()->GetFirstPlayerController());
	Player = Cast<APlayerCharacter>(PlayerController->GetPawn());

	// Setup ammo
	CurrentAmmo = FMath::Min(StartAmmo, MaxAmmo);
	CurrentAmmoInClip = FMath::Min(MaxAmmoPerClip, StartAmmo);

	bCanReload = true;
	bIsReloading = false;
}

void ABaseWeapon::StartFiring()
{
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ABaseWeapon::HandleFiring, FireRate, bIsAutomatic, 0.0f);
}

void ABaseWeapon::StopFiring()
{
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
}

void ABaseWeapon::HandleFiring()
{
	if (CheckIfWeaponCanFire(FireRate))
	{
		UseAmmo();

		FVector HitLocation; // Store hit location
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation("MuzzleSocket");				
		if (WeaponTrace(HitLocation, MuzzleLocation, MaxRange, CalculateBulletSpread())) // Trace from muzzle to crosshair hit location
		{
			// Hit something
		}

		// Play Audio
		ShotAudioComponent->Play();
	}
	else if (CurrentAmmoInClip == 0 && bCanReload)
	{
		StartReload();
	}
}

bool ABaseWeapon::CheckIfWeaponCanFire(float FireRate)
{
	if (Player)
	{
		if (Player->bIsRolling)
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
	if (!bIsReloading)
	{
		bIsReloading = true;
		GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &ABaseWeapon::ReloadWeapon, ReloadDuration, false);
	}
}

void ABaseWeapon::ReloadWeapon()
{
	bIsReloading = false;
	CurrentAmmoInClip = MaxAmmoPerClip;
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
}

bool ABaseWeapon::WeaponTrace(FVector& OutHitlocation, FVector MuzzleLocation, float MaxRange, float BulletSpread) const
{
	const FName TraceTag("WeaponTraceTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.TraceTag = TraceTag;

	FHitResult RV_Hit;	
	FVector StartLocation = MuzzleLocation;
	FVector EndLocation = StartLocation + (GetAimDirection(StartLocation, BulletSpread) * MaxRange);

	if (GetWorld()->LineTraceSingleByChannel(RV_Hit, StartLocation, EndLocation, ECC_Visibility, RV_TraceParams))
	{
		OutHitlocation = RV_Hit.Location;
		return true;
	}
	OutHitlocation = EndLocation;
	return false; // Line-trace didn't hit anything
}

FVector ABaseWeapon::GetAimDirection(FVector StartLocation, float BulletSpread) const
{
	FVector AimDirection = PlayerController->GetCrosshairHitLocation() - StartLocation;
	AimDirection.Normalize();	
	return FMath().VRandCone(AimDirection, FMath().DegreesToRadians(BulletSpread));
}



