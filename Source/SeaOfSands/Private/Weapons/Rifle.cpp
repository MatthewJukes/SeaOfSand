// Fill out your copyright notice in the Description page of Project Settings.

#include "Rifle.h"
#include "Components/SkeletalMeshComponent.h"
#include "Public/TimerManager.h"

// Sets default values
ARifle::ARifle()
{
	Rifle = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Rifle"));
}

void ARifle::StartFiring()
{
	UE_LOG(LogTemp, Warning, TEXT("Rifle Firing"));
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ARifle::FireBullet, FireRate, bIsAutomatic, 0.0f);
}

void ARifle::StopFiring()
{
	UE_LOG(LogTemp, Warning, TEXT("Rifle Stopping"));
	LastElaspedTime += GetWorldTimerManager().GetTimerRemaining(FireRateTimerHandle);
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
}

void ARifle::FireBullet()
{
	float ElaspedTime = GetWorldTimerManager().GetTimerElapsed(FireRateTimerHandle);
	if (CheckIfWeaponCanFire(ElaspedTime, LastElaspedTime, FireRate))
	{
		FVector MuzzleLocation = Rifle->GetSocketLocation("MuzzleSocket");
		WeaponTrace(MuzzleLocation, MaxRange, BulletSpread);
		LastElaspedTime = 0.f;
	}
}




