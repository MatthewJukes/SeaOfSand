// Fill out your copyright notice in the Description page of Project Settings.

#include "Rifle.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Public/TimerManager.h"

// Sets default values
ARifle::ARifle()
{
	Rifle = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Rifle"));
	RifleShotAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("RIfleShotAudio"));
}

void ARifle::StartFiring()
{
	UE_LOG(LogTemp, Warning, TEXT("Rifle Firing"));
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ARifle::FireBullet, FireRate, bIsAutomatic, 0.0f);	
}

void ARifle::StopFiring()
{
	UE_LOG(LogTemp, Warning, TEXT("Rifle Stopping"));
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
}

void ARifle::FireBullet()
{
	if (GetWorldTimerManager().GetTimerRemaining(RefireTimerHandle) <= 0.001f)
	{
		FVector MuzzleLocation = Rifle->GetSocketLocation("MuzzleSocket");
		WeaponTrace(MuzzleLocation, MaxRange, BulletSpread);
		GetWorldTimerManager().SetTimer(RefireTimerHandle, FireRate, false, FireRate);
		RifleShotAudioComponent->Play();
	}
}




