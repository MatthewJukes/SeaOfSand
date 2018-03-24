// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon.h"


// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseWeapon::StartFiring(){}
void ABaseWeapon::StopFiring(){}

void ABaseWeapon::WeaponAimLocation(FVector MuzzleLocation, float MaxRange, float BulletSpread)
{

}

bool ABaseWeapon::CheckIfWeaponCanFire(float ElaspedTime, float LastElaspedTime, float FireRate) const
{
	return true;
}

