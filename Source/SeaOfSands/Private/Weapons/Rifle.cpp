// Fill out your copyright notice in the Description page of Project Settings.

#include "Rifle.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Public/TimerManager.h"

// Sets default values
ARifle::ARifle()
{	
	//Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Rifle"));
	ShotAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("RIfleShotAudio"));
	//Beam = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RifleBeam"));
}

void ARifle::StartFiring()
{
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ARifle::FireBullet, FireRate, bIsAutomatic, 0.0f);	
}

void ARifle::StopFiring()
{
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
}

void ARifle::FireBullet()
{
	if (GetWorldTimerManager().GetTimerRemaining(RefireTimerHandle) <= 0.001f)
	{
		FVector HitLocation; // Store hit location
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation("MuzzleSocket");

		// Trace from muzzle to crosshair hit location
		if (WeaponTrace(HitLocation, MuzzleLocation, MaxRange, BulletSpread))
		{
			// Hit something
		}

		//Beam->SetBeamSourcePoint(0, MuzzleLocation, 0);
		//Beam->SetBeamTargetPoint(0, HitLocation, 0);

		// Reset refire timer
		GetWorldTimerManager().SetTimer(RefireTimerHandle, FireRate, false, FireRate);

		// Play Audio
		ShotAudioComponent->Play();
	}
}




